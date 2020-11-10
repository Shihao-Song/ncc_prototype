#ifndef __CLUSTERING_HH__
#define __CLUSTERING_HH__

#include <cstdint>
#include <set>
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
    // UINT64 cluster_id = INVALID_ID;

    /*
    enum class Status : int
    {
        MAPPED,
        UNMAPPED
    }status = Status::UNMAPPED;
    */

    std::vector<UINT64> connected_clusters;

  public:
    Neuron_Status() {}

    std::vector<UINT64> &getConnectedClustersRef() { return connected_clusters; }
    std::vector<UINT64> getConnectedClustersCopy() { return connected_clusters; }
    void addCluster(UINT64 _cluster) { connected_clusters.push_back(_cluster); }
    // UINT64 getClusterId() { return cluster_id; }
    // bool isMapped() { return status == Status::MAPPED; }
    // void setMapped() { status = Status::MAPPED; }
};

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

      protected:
        const unsigned CROSSBAR_SIZE = 4;

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
    };

    std::vector<Cluster> clusters;

  public:
    Clusters(){}
    void fcfs(std::vector<Neuron>&);

  protected: // Helper function
    std::vector<Neuron_Status> neuron_status;

    std::vector<std::pair<UINT64,
        std::vector<UINT64>>> candidateClusterWithUnmappedInputNeurons(std::vector<UINT64>&);
};
}
}
#endif
