#include "cluster/cluster.hh"

#include <iostream>

namespace EXT
{
namespace Clustering
{

void Clusters::fcfs(std::vector<Neuron>& snn)
{
    // Record the clustering status for each neuron
    std::vector<Neuron_Status> neuron_status(snn.size());
    // Initialize the first cluster
    clusters.push_back(0);

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
                // All the fanin must be able to fix into one crossbar size
                for (auto input : snn[i].getInputNeuronListCopy())
                {}
            }
        }
    }
}

}
}
