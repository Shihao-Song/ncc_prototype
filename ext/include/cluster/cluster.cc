#include "cluster/cluster.hh"

#include <algorithm>
#include <iostream>

namespace EXT
{
namespace Clustering
{
// The goal is to minimize the number of clusters
void Clusters::fcfs(std::vector<Neuron>& snn)
{
    // Record the clustering status for each neuron
    neuron_status.resize(snn.size());
    // Initialize the first cluster
    clusters.push_back(0);
    clusters[0].addInput(0);
    neuron_status[0].addCluster(0);

    clusters.push_back(1);
    clusters[1].addInput(0);
    clusters[1].addInput(1);
    neuron_status[0].addCluster(1);
    neuron_status[1].addCluster(1);

    clusters.push_back(2);
    clusters[2].addInput(0);
    clusters[2].addInput(1);
    clusters[2].addInput(2);
    // clusters[2].addInput(5);
    neuron_status[0].addCluster(2);
    neuron_status[1].addCluster(2);
    neuron_status[2].addCluster(2);
    // neuron_status[5].addCluster(2);

    snn[6].addInputNeuron(2);
    snn[6].addInputNeuron(3);

    for (auto &cluster : clusters)
    {
        std::cout << "Cluster ID: " << cluster.getClusterId() << "\n";
        std::cout << "Input: ";
        for (auto &input : cluster.getInputsListRef()) { std::cout << input << " "; }
        std::cout << "\nOutput: ";
        for (auto &output : cluster.getOutputsListRef()) { std::cout << output << " "; }
        std::cout << "\n\n";
    }
    std::cout << "\n";

    for (auto i = 0; i < snn.size(); i++)
    {
        if (snn[i].numInputNeurons())
        {
            // Case 1: the neuron is unrolled
            if (auto children = snn[i].getChildrenCopy();
                    children.size() > 0)
            {
                std::cout << "On-going... \n";
                // std::cout << "Neuron ID: " << snn[i].getNeuronId() << "\n";
                // for (auto child : children) { std::cout << child << " "; }
                // std::cout << "\n\n";
            }
            // Case 2: the neuron is not unrolled
            else
            {
                auto priority_cluster_lists = 
                    candidateClusterWithUnmappedInputNeurons(snn[i].getInputNeuronList());

                for (auto &[cid, all_unmapped] : priority_cluster_lists)
                {
                    std::cout << "Cluster ID: " << cid << "; ";
                    if (clusters[cid].canBePacked(all_unmapped.size()))
                    {
                        std::cout << "PASS; ";
                    }
                    else { std::cout << "FAIL; "; }
                    std::cout << "Unmapped: ";
                    for (auto &unmapped : all_unmapped) { std::cout << unmapped << " "; }
                    std::cout << "\n";
                }
                std::cout << "\n";

                auto chosen_cluster = -1;
                for (auto &[cid, all_unmapped] : priority_cluster_lists)
                {
                    if (clusters[cid].canBePacked(all_unmapped.size()))
                    {
                        chosen_cluster = cid;
                        break;
                    }
                }

                if (chosen_cluster != -1)
                {
                    auto &[cid, all_unmapped] = priority_cluster_lists[chosen_cluster];
                    for (auto &unmapped : all_unmapped)
                    {
                        clusters[chosen_cluster].addInput(unmapped);
                        neuron_status[unmapped].addCluster(chosen_cluster);
                    }

                    clusters[chosen_cluster].addOutput(snn[i].getNeuronId());
                }

                for (auto &cluster : clusters)
                {
                    std::cout << "Cluster ID: " << cluster.getClusterId() << "\n";
                    std::cout << "Input: ";
                    for (auto &input : cluster.getInputsListRef()) { std::cout << input << " "; }
                    std::cout << "\nOutput: ";
                    for (auto &output : cluster.getOutputsListRef()) { std::cout << output << " "; }
                    std::cout << "\n\n";
                }
                std::cout << "\n";


                //else
                //{
                
                //}
                exit(0);
            }
        }
    }
}

std::vector<std::pair<UINT64, std::vector<UINT64>>> 
Clusters::candidateClusterWithUnmappedInputNeurons(std::vector<UINT64>& input_neurons)
{
    std::set<UINT64> candidate_clusters;
    // Has the input already mapped to a cluster?
    for (auto input : input_neurons)
    {
        auto &candidates = neuron_status[input].getConnectedClustersRef();
        for (auto &candidate : candidates) { candidate_clusters.insert(candidate); }
    }

    // pair<cluster ID, unmapped inputs>
    std::vector<std::pair<UINT64,
                std::vector<UINT64>>> priority_cluster_lists;
    for (auto &candidate : candidate_clusters)
    {
        std::vector<UINT64> unmapped_inputs;

        for (auto input : input_neurons)
        {
            if (!(clusters[candidate].isInputMapped(input)))
            {
                unmapped_inputs.push_back(input);
            }
        }

        priority_cluster_lists.push_back(std::make_pair(candidate, unmapped_inputs));
    }

    std::sort(priority_cluster_lists.begin(),
              priority_cluster_lists.end(),
              [](auto &left, auto &right)
    {
        return left.second.size() < right.second.size();
    });

    return priority_cluster_lists;
}

}
}
