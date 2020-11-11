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
    std::set<UINT64> connected_clusters;

  public:
    Neuron_Status() {}

    std::set<UINT64> &getConnectedClustersRef() { return connected_clusters; }
    std::set<UINT64> getConnectedClustersCopy() { return connected_clusters; }
    void addConnectedCluster(UINT64 _cluster) { connected_clusters.insert(_cluster); }
};
        
static const unsigned MIN_FANIN = 2; // A cross-bar needs to be at least 
                                     // have 2 input ports available
static const unsigned CROSSBAR_SIZE = 4;

typedef EXT::Unrolling::Neuron Neuron;
class Clusters
{
  protected:
    class Cluster
    {
      protected:
        UINT64 cluster_id = INVALID_ID;

        std::set<UINT64> inputs;
        std::set<UINT64> outputs;

      // public:
      public:
        Cluster(UINT64 _id) : cluster_id(_id) {}

        UINT64 getClusterId() { return cluster_id; }

        void addInput(UINT64 _input) { inputs.insert(_input); }
        void addOutput(UINT64 _output) { outputs.insert(_output); }

        std::set<UINT64> &getInputsListRef() { return inputs; }
        std::set<UINT64> getInputsListCopy() { return inputs; }

        std::set<UINT64> &getOutputsListRef() { return outputs; }
        std::set<UINT64> getOutputsListCopy() { return outputs; }

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
    Clusters(){}
    void fcfs(std::vector<Neuron>&);

  protected: // Helper function
    UINT64 addCluster()
    {
        UINT64 cid = clusters.size();

        std::unique_ptr<Cluster> cluster = std::make_unique<Cluster>(cid);

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
    void debugPrint()
    {
        for (auto &cluster : clusters)
        {
            std::cout << "Cluster ID: " << cluster->getClusterId() << "\n";
            std::cout << "Input: ";
            for (auto &input : cluster->getInputsListRef()) { std::cout << input << " "; }
            std::cout << "\nOutput: ";
            for (auto &output : cluster->getOutputsListRef()) { std::cout << output << " "; }
            std::cout << "\n\n";
        }

        for (auto i = 0; i < neuron_status.size(); i++)
        {
            std::cout << "Neuron ID: " << i << "; Clusters: ";
            for (auto &cluster : neuron_status[i].getConnectedClustersRef())
            {
                std::cout << cluster << " ";
            }
            std::cout << "\n";
        }

        std::cout << "-------------------------------\n\n";
    }
};
}
}
#endif
