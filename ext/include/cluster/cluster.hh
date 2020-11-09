#ifndef __CLUSTERING_HH__
#define __CLUSTERING_HH__

#include <cstdint>
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
    UINT64 cluster_id = INVALID_ID;

    enum class Status : int
    {
        MAPPED,
        UNMAPPED
    }status = Status::UNMAPPED;

  public:
    Neuron_Status() {}

    bool isMapped() { return status == Status::MAPPED; }
    void setMapped() { status = Status::MAPPED; }
};

typedef EXT::Unrolling::Neuron Neuron;
class Clusters
{
  protected:
    class Cluster
    {
      protected:
        UINT64 cluster_id = INVALID_ID;

        std::vector<UINT64> inputs;
        std::vector<UINT64> outputs;

      protected:
        const unsigned CROSSBAR_SIZE = 4;

      public:
        Cluster(UINT64 _id) : cluster_id(_id) {}

        bool canBePacked(unsigned num_inputs)
        {
            return (((inputs.size() + num_inputs) < CROSSBAR_SIZE) && 
                     (outputs.size() < CROSSBAR_SIZE));
        }
    };

    std::vector<Cluster> clusters;

  public:
    Clusters(){}
    void fcfs(std::vector<Neuron>&);
};
}
}
#endif
