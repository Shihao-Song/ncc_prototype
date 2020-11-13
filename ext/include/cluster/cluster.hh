#ifndef __CLUSTERING_HH__
#define __CLUSTERING_HH__

#include <algorithm>
#include <cstdint>
#include <list>
#include <memory>
#include <set>
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
    const unsigned MIN_FANIN; // A cross-bar needs to be at least 
                              // have 2 input ports available
    const unsigned CROSSBAR_SIZE;

  protected:
    class Cluster
    {
      protected:
        const unsigned MIN_FANIN; // A cross-bar needs to be at least 
                                  // have 2 input ports available
        const unsigned CROSSBAR_SIZE;

      protected:
        UINT64 cluster_id = INVALID_ID;

        std::set<UINT64> inputs;
        std::set<UINT64> outputs;

        std::set<UINT64> connected_clusters;

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

        std::set<UINT64> &getConnectedClustersRef() { return connected_clusters; }
        std::set<UINT64> getConnectedClustersCopy() { return connected_clusters; }
        void addConnectedCluster(UINT64 _cluster) { connected_clusters.insert(_cluster); }

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
    : MIN_FANIN(_fanin)
    , CROSSBAR_SIZE(_crossbar_size)
    {}
    void fcfs(std::vector<Neuron>&);

    void printClusterIR(std::string &_out)
    {
        std::fstream file;
        file.open(_out, std::fstream::out);

        for (auto &cluster : clusters)
        {
            UINT64 cid = cluster->getClusterId();

            for (auto &conn_cluster : cluster->getConnectedClustersRef())
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

    void packToCluster(UINT64 cur_neuron_idx,
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
                    cluster->addNumSpikes(conn_cluster, neuron_status[output].numOfSpikes());
                    cluster->addConnectedCluster(conn_cluster);
                }
            }
        }
    }

    void debugPrint()
    {
        std::cout << "-------------------------------\n";
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
            for (auto &conn_cluster : cluster->getConnectedClustersRef())
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
