#ifndef __CLUSTERING_HH__
#define __CLUSTERING_HH__

#include <algorithm>
#include <cstdint>
#include <list>
#include <memory>
#include <set>
#include <stack>
#include <unordered_map>
#include <vector>

#include "unroll/unroll.hh"

namespace EXT
{
namespace Clustering
{
typedef uint64_t UINT64;

const UINT64 INVALID_ID = (UINT64) - 1;

class Neuron_Status
{
  protected:
    unsigned num_spikes;

    std::set<UINT64> connected_clusters;

  public:
    Neuron_Status() {}

    void setNumOfSpikes(unsigned _spikes) { num_spikes = _spikes; }
    unsigned numOfSpikes() { return num_spikes; }

    std::set<UINT64> &getConnectedClustersRef() { return connected_clusters; }
    std::set<UINT64> getConnectedClustersCopy() { return connected_clusters; }
    void addConnectedCluster(UINT64 _cluster) { connected_clusters.insert(_cluster); }
};

typedef EXT::Unrolling::Neuron Neuron;
class Clusters
{
  protected:
    const unsigned INVALID_FANIN = (unsigned) - 1;

    unsigned MIN_FANIN; 
    const unsigned CROSSBAR_SIZE;

  protected:
    class Cluster
    {
      protected:
        const unsigned MIN_FANIN; 
        const unsigned CROSSBAR_SIZE;

      protected:
        UINT64 cluster_id = INVALID_ID;

        std::set<UINT64> inputs;
        std::set<UINT64> outputs;

        std::set<UINT64> connected_clusters_out;
        std::set<UINT64> connected_clusters_in;

        std::unordered_map<UINT64, UINT64> cluster_spikes_map;
      // public:
      public:
        Cluster(UINT64 _id, unsigned _fanin, unsigned _crossbar_size)
            : MIN_FANIN(_fanin)
            , CROSSBAR_SIZE(_crossbar_size)
            , cluster_id(_id) {}

        UINT64 getClusterId() { return cluster_id; }

        void addInput(UINT64 _input) { inputs.insert(_input); }
        void addOutput(UINT64 _output) { outputs.insert(_output); }

        std::set<UINT64> &getInputsListRef() { return inputs; }
        std::set<UINT64> getInputsListCopy() { return inputs; }

        std::set<UINT64> &getOutputsListRef() { return outputs; }
        std::set<UINT64> getOutputsListCopy() { return outputs; }

        std::set<UINT64> &getConnectedClustersOutRef() { return connected_clusters_out; }
        std::set<UINT64> getConnectedClustersOutCopy() { return connected_clusters_out; }
        void addConnectedClusterOut(UINT64 _cluster) { connected_clusters_out.insert(_cluster); }

        std::set<UINT64> &getConnectedClustersInRef() { return connected_clusters_in; }
        std::set<UINT64> getConnectedClustersInCopy() { return connected_clusters_in; }
        void addConnectedClusterIn(UINT64 _cluster) { connected_clusters_in.insert(_cluster); }


        void addNumSpikes(UINT64 cid, unsigned _spikes)
        {
            // std::cout << "\nAdding: " << cid << " " << _spikes << "\n";

            if (auto iter = cluster_spikes_map.find(cid);
                     iter != cluster_spikes_map.end())
            {
                iter->second += _spikes;
            }
            else
            {
                cluster_spikes_map.insert({cid, _spikes});
            }
	}

        UINT64 numOfSpikes(UINT64 cid)
        {
            auto iter = cluster_spikes_map.find(cid);
            assert(iter != cluster_spikes_map.end());
            return iter->second;
        }

        bool isInputMapped(UINT64 _input)
        {
            return (inputs.find(_input) != inputs.end());
        }

        bool canBePacked(unsigned num_inputs)
        {
            return (((inputs.size() + num_inputs) <= CROSSBAR_SIZE) && 
                     (outputs.size() < CROSSBAR_SIZE));
        }

        unsigned getUtilization() { return (inputs.size() + outputs.size()); }
        unsigned numAvailInputPorts() { return (CROSSBAR_SIZE - inputs.size()); }
    };

    std::vector<std::unique_ptr<Cluster>> clusters;
    std::vector<Cluster*> sorted_clusters;

  public:
    Clusters(unsigned _fanin, unsigned _crossbar_size)
    : CROSSBAR_SIZE(_crossbar_size)
    {
        if (_fanin == INVALID_FANIN) { MIN_FANIN = CROSSBAR_SIZE; }
        else { MIN_FANIN = _fanin; }
        assert(MIN_FANIN <= CROSSBAR_SIZE); // When the unrolling fanin larger than crossbar,
                                            // it is not supported.
    }

    void clustering(std::vector<Neuron> &snn, std::string &mode)
    {
        if (mode == "min-clusters")
        {
            minClusters(snn);
        }
        else if (mode == "random")
        {
            random(snn);
        }
        else
        {
            std::cerr << "---------------------------------------\n";
            std::cerr << "Error: unsupported clustering algorithm\n";
            return;
        }
    }

    void printClusterIR(std::string &_out)
    {
        std::fstream file;
        file.open(_out, std::fstream::out);

        for (auto &cluster : clusters)
        {
            UINT64 cid = cluster->getClusterId();

            for (auto &conn_cluster : cluster->getConnectedClustersOutRef())
            {
                file << cid << " "
                     << conn_cluster << " "
                     << cluster->numOfSpikes(conn_cluster) << "\n";
            }
        }
        file.close();
    }
    void printClusterStats(std::string &_out)
    {
        std::fstream file;
        file.open(_out, std::fstream::out);

        for (auto &cluster : clusters)
        {
            UINT64 cid = cluster->getClusterId();

            unsigned num_inputs = cluster->getInputsListRef().size();
            unsigned num_outputs = cluster->getOutputsListRef().size();

            file << cid << " " << num_inputs << " " << num_outputs << "\n";
        }

        file.close();
    }

  protected: // Helper function
    void minClusters(std::vector<Neuron>&);
    // void minClustersV2(std::vector<Neuron>&);
    void random(std::vector<Neuron>&);

    UINT64 addCluster()
    {
        UINT64 cid = clusters.size();

        std::unique_ptr<Cluster> cluster = 
            std::make_unique<Cluster>(cid, MIN_FANIN, CROSSBAR_SIZE);

        clusters.push_back(std::move(cluster));

        sorted_clusters.push_back(clusters[cid].get());

        return cid;
    }

    std::vector<Neuron_Status> neuron_status;

    unsigned numCanBePacked(UINT64 cid,
                            std::list<UINT64> &non_unrolled_inputs);

    void packToCluster(unsigned total_inputs_can_be_packed,
                       UINT64 cur_neuron_idx,
                       UINT64 cid,
                       std::unordered_map<UINT64, UINT64> &input_to_output_map,
                       std::list<UINT64> &non_unrolled_inputs);

  protected:
    void postClustering()
    {
        for (auto &cluster : clusters)
        {
            for (auto &output : cluster->getOutputsListRef())
            {
                for (auto &conn_cluster : neuron_status[output].getConnectedClustersRef())
                {
                    // cluster->addNumSpikes(conn_cluster, neuron_status[output].numOfSpikes());
                    cluster->addConnectedClusterOut(conn_cluster);
                    clusters[conn_cluster]->addConnectedClusterIn(cluster->getClusterId());
                }
            }
        }
    }

    // The following codes should better be standalone.
    void connectedComponents()
    {
        postClustering();

        std::cout << "---------------------------------------\n";
        std::vector<bool> visited(clusters.size(), false);

        std::vector<std::vector<UINT64>> cc;
        for (auto cid = 0; cid < clusters.size(); cid++)
        {
            // std::vector<UINT64> neighbors;
            // for (auto id : clusters[cid]->getConnectedClustersOutRef()) {neighbors.push_back(id);}
            // for (auto id : clusters[cid]->getConnectedClustersInRef()) {neighbors.push_back(id);}

            if (visited[cid] == false)
            // if ((visited[cid] == false) && (neighbors.size() > 0))
            {
                iterativeDFS(cc, cid, visited);
            }
        }

        if (cc.size() > 1)
        {
            std::cout << "Number of disconnect graphs: " << cc.size() << "\n";

            // Connect all the disconnected graph
            std::list<UINT64> neurons_to_connect;
            for (auto &c : cc)
            {
                for (auto ele : c)
                {
                    auto &conn_clusters_out = clusters[ele]->getConnectedClustersOutRef();
                    if (conn_clusters_out.size() == 0)
                    {
                        auto &outputs = clusters[ele]->getOutputsListRef();
                        assert(outputs.size());
                        neurons_to_connect.push_back(*(outputs.begin()));
                        // std::cout << ele << "\n";
                        break;
                    }
                }
            }

            auto new_cid = INVALID_ID;
            unsigned num_new_neurons = 0;
            while (true)
            // for (auto neuron_to_connect : neurons_to_connect)
            {
                if (neurons_to_connect.size() == 0) { break; }

                if (num_new_neurons % CROSSBAR_SIZE == 0)
                {
                    if (num_new_neurons > 0)
                    {
                        // Connect previous cid
                        auto &outputs = clusters[new_cid]->getOutputsListRef();
                        assert(outputs.size());
                        neurons_to_connect.push_front(*(outputs.begin()));
                        // std::cout << neuron_status[*(outputs.begin())].numOfSpikes() << "\n";
                    }

                    new_cid = addCluster();
                }

                auto neuron_to_connect = *(neurons_to_connect.begin());
                num_new_neurons++;

                UINT64 new_neuron_id = neuron_status.size();
                neuron_status.push_back(Neuron_Status());
                neuron_status[new_neuron_id].
                    setNumOfSpikes(neuron_status[neuron_to_connect].numOfSpikes());

                clusters[new_cid]->addInput(neuron_to_connect);
                neuron_status[neuron_to_connect].
                    addConnectedCluster(clusters[new_cid]->getClusterId());
                clusters[new_cid]->addOutput(new_neuron_id);

                neurons_to_connect.pop_front();
            }

            postClustering();
        }

        // Calculate inter-cluster spikes
        for (auto &cluster : clusters)
        {
            for (auto &output : cluster->getOutputsListRef())
            {
                for (auto &conn_cluster : neuron_status[output].getConnectedClustersRef())
                {
                    cluster->addNumSpikes(conn_cluster, neuron_status[output].numOfSpikes());
                }
            }
        }
    }

    // Better to iterative here, recursive can crash when the graph is large,
    void iterativeDFS(std::vector<std::vector<UINT64>> &cc,
                      UINT64 cid,
                      std::vector<bool> &visited)
    {
        std::stack<UINT64> s;

        s.push(cid);

        std::vector<UINT64> c;
        while (true)
        {
            if (s.size() == 0) { break; }
            
            auto v = s.top();
            s.pop();
            if (visited[v]) { continue; }

            visited[v] = true;
            c.push_back(v);

            std::vector<UINT64> neighbors;
            for (auto id : clusters[v]->getConnectedClustersOutRef()) {neighbors.push_back(id);}
            for (auto id : clusters[v]->getConnectedClustersInRef()) {neighbors.push_back(id);}

            for (auto neighbor : neighbors)
            {
                if (!visited[neighbor]) { s.push(neighbor); }
            }
        }

	cc.push_back(c);
    }

    void debugPrint()
    {
        std::cout << "---------------------------------------\n";
        for (auto &cluster : clusters)
        {
            std::cout << "Cluster ID: " << cluster->getClusterId() << "\n";
            std::cout << "Input Neurons: ";
            for (auto &input : cluster->getInputsListRef()) { std::cout << input << " "; }
            std::cout << "\nOutput Neurons: ";
            for (auto &output : cluster->getOutputsListRef()) { std::cout << output << " "; }
            /*
            for (auto &output : cluster->getOutputsListRef())
            {
                for (auto &conn_cluster : neuron_status[output].getConnectedClustersRef())
                {
                    cluster->addNumSpikes(conn_cluster, neuron_status[output].numOfSpikes());
                    cluster->addConnectedCluster(conn_cluster);
                }
            }
            */
            std::cout << "\nConnected Clusters: ";
            for (auto &conn_cluster : cluster->getConnectedClustersOutRef())
            {
                std::cout << conn_cluster << "(";
                std::cout << cluster->numOfSpikes(conn_cluster) << "), ";
            }
            std::cout << "\n\n";
        }

        /*
        for (auto i = 0; i < neuron_status.size(); i++)
        {
            std::cout << "Neuron ID: " << i << "; Clusters: ";
            for (auto &cluster : neuron_status[i].getConnectedClustersRef())
            {
                std::cout << cluster << " ";
            }
            std::cout << "\n";
        }
        */
    }
};
}
}
#endif
