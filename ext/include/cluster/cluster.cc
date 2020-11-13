#include "cluster/cluster.hh"

#include <iostream>

namespace EXT
{
namespace Clustering
{
// The goal is to minimize the number of clusters
void Clusters::minClusters(std::vector<Neuron>& snn)
{
    std::cout << "---------------------------------------\n";
    std::cout << "Clustering Step - Minimize Clusters#\n";
    std::cout << "Minimum fanin: " << MIN_FANIN << "\n";
    std::cout << "Crossbar size: " << CROSSBAR_SIZE << "\n";
    std::cout << "---------------------------------------\n";

    // Record the clustering status for each neuron
    neuron_status.resize(snn.size());
    for (auto &neuron : snn)
    {
        UINT64 id = neuron.getNeuronId();
        unsigned spikes = neuron.numOfSpikes();

        neuron_status[id].setNumOfSpikes(spikes);
    }

    // debugPrint();

    for (auto cur_neuron_idx = 0;
              cur_neuron_idx < snn.size();
              cur_neuron_idx++)
    {
        if (snn[cur_neuron_idx].hasParent()) { break; }

        if (snn[cur_neuron_idx].numInputNeurons()) 
        {
            std::cout << "\nMapping neuron id: " << snn[cur_neuron_idx].getNeuronId() << "\n";
            // All the input neurons before unrolling
            std::list<UINT64> non_unrolled_inputs;
            // Input neuron -> Output neuron mapping
            std::unordered_map<UINT64, UINT64> input_to_output_map;

            // Case 1: the neuron is unrolled
            if (auto &children = snn[cur_neuron_idx].getChildrenRef();
                    children.size() > 0)
            {
                std::set<UINT64> intermediate_neurons;
                for (auto &child : children) { intermediate_neurons.insert(child); }

                for (auto &inter_neuron : children)
                {
                    for (auto &raw_input : snn[inter_neuron].getInputNeuronList())
                    {
                        if (intermediate_neurons.find(raw_input) == intermediate_neurons.end())
                        {
                            non_unrolled_inputs.push_back(raw_input);
                            input_to_output_map.insert({raw_input, inter_neuron});
                        }
                    }
                }
                for (auto &raw_input : snn[cur_neuron_idx].getInputNeuronList())
                {
                    if (intermediate_neurons.find(raw_input) == intermediate_neurons.end())
                    {
                        non_unrolled_inputs.push_back(raw_input);
                        input_to_output_map.insert({raw_input, cur_neuron_idx});
                    }
		}
            }
            // Case 2: the neuron is not unrolled
            else
            {
                auto &inputs_to_pack = snn[cur_neuron_idx].getInputNeuronList();
                for (auto i = 0; i < inputs_to_pack.size(); i++)
                {
                    auto input_to_pack = inputs_to_pack[i];
                    if (i == CROSSBAR_SIZE) { break; }

                    non_unrolled_inputs.push_back(input_to_pack);
                    input_to_output_map.insert({input_to_pack,
                                                snn[cur_neuron_idx].getNeuronId()});
                }
            }
            std::sort(sorted_clusters.begin(),
                      sorted_clusters.end(),
                      [](auto &left, auto &right)
                      {
                          return left->getUtilization() > right->getUtilization();
                      });

            for (auto &cluster : sorted_clusters)
            {
                if (non_unrolled_inputs.size() == 0) { break; }

                packToCluster(snn[cur_neuron_idx].getNeuronId(),
                              cluster->getClusterId(),
                              input_to_output_map,
                              non_unrolled_inputs);
            }
            // std::cout << "Inputs: ";
            // for (auto &input : non_unrolled_inputs) { std::cout << input << " "; }
            // std::cout << "\n";

            // Need new clusters to map the rest
            while (true)
            {
                static unsigned counter = 0;

                if (non_unrolled_inputs.size() == 0) { break; }

                auto cid = addCluster();
                packToCluster(snn[cur_neuron_idx].getNeuronId(),
                              cid,
                              input_to_output_map,
                              non_unrolled_inputs);
            }
            // std::cout << "Mapped neuron id: " << snn[cur_neuron_idx].getNeuronId() << "\n";
            // debugPrint();
        }
    }
    postClustering();
    // debugPrint();
}

void Clusters::packToCluster(UINT64 cur_neuron_idx, 
                             UINT64 cid,
                             std::unordered_map<UINT64, UINT64> &input_to_output_map,
                             std::list<UINT64> &non_unrolled_inputs)
{
    unsigned total_inputs_can_be_packed = 0;
    unsigned new_inputs_to_be_packed = 0;
    for (auto &pending_input : non_unrolled_inputs)
    {
        if (!clusters[cid]->isInputMapped(pending_input))
        {
            new_inputs_to_be_packed++;
        }

        if (clusters[cid]->canBePacked(new_inputs_to_be_packed))
        {
            total_inputs_can_be_packed++;
        }
        else
	{
	    break;
	}
    }

    // It must be able to provide MIN_FANIN number of input ports
    if (total_inputs_can_be_packed >= MIN_FANIN && 
        non_unrolled_inputs.size() >= MIN_FANIN ||
        non_unrolled_inputs.size() < MIN_FANIN &&
        total_inputs_can_be_packed == non_unrolled_inputs.size())
    {
        UINT64 last_input_packed = INVALID_ID;
        unsigned cur_packed = 0;
        for (auto &input_to_pack : non_unrolled_inputs)
        {
            if (cur_packed == total_inputs_can_be_packed) { break; }

            clusters[cid]->addInput(input_to_pack);
            neuron_status[input_to_pack].
                addConnectedCluster(clusters[cid]->getClusterId());

            last_input_packed = input_to_pack;
            cur_packed++;
	}

        auto inter_neuron_iter = input_to_output_map.find(last_input_packed);
        assert(inter_neuron_iter != input_to_output_map.end());
        UINT64 inter_neuron_id = inter_neuron_iter->second;

        clusters[cid]->addOutput(inter_neuron_id);

        auto begin_to_remove = non_unrolled_inputs.begin();
        auto end_to_remove = std::next(non_unrolled_inputs.begin(), 
                                       total_inputs_can_be_packed);
        non_unrolled_inputs.erase(begin_to_remove, end_to_remove);

        if (inter_neuron_id != cur_neuron_idx)
        {
            non_unrolled_inputs.push_front(inter_neuron_id);
        }
        assert(clusters[cid]->getInputsListRef().size() <= CROSSBAR_SIZE);
        assert(clusters[cid]->getOutputsListRef().size() <= CROSSBAR_SIZE);
    }
}
}
}
