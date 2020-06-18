#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "protobuf/proto_graph/graph.pb.h"

int main(int argc, char* argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    if (argc != 2)
    {
        std::cerr << "Usage example:  " << argv[0] << " input.graph" << std::endl;
        return -1;
    }

    NCC_Graph_Proto::Graph graph;

    std::fstream input(argv[1], std::ios::in | std::ios::binary);
    if (!graph.ParseFromIstream(&input))
    {
        std::cerr << "Failed to parse the graph." << std::endl;
        return -1;
    }

    // Parse the nodes by neuron type
    std::vector<NCC_Graph_Proto::Node> bias_neurons;
    std::vector<NCC_Graph_Proto::Node> io_neurons;

    for (int i = 0; i < graph.nodes_size(); i++)
    {
        const NCC_Graph_Proto::Node &node = graph.nodes(i);

        switch (node.type())
	{
            case NCC_Graph_Proto::Node::IO:
                io_neurons.push_back(node);
                break;
            case NCC_Graph_Proto::Node::BIAS:
                bias_neurons.push_back(node);
                break;
        }
    }

    // Output the graph (1) IO neurons
    for (auto neuron : io_neurons)
    {
        std::cout << "Neuron ID: " << neuron.id() << std::endl;
        std::cout << "Neuron Type: IO" << std::endl;
        for (int j = 0; j < neuron.adjs_size(); j++)
        {
            std::cout << "Dest. Neuron ID: " << neuron.adjs(j) << " (";
            std::cout << neuron.weights(j) << ")" << std::endl;
        }

	std::cout << std::endl;
    }

    // Output the graph (2) Bias neurons
    auto bias_id = 0;
    for (auto neuron : bias_neurons)
    {
        std::string id = "b_" + std::to_string(bias_id);
        std::cout << "Neuron ID: " << id << std::endl;
        std::cout << "Neuron Type: BIAS" << std::endl;
        for (int j = 0; j < neuron.adjs_size(); j++)
        {
            std::cout << "Dest. Neuron ID: " << neuron.adjs(j) << " (";
            std::cout << neuron.weights(j) << ")" << std::endl;
        }
	bias_id++;

	std::cout << std::endl;
    }
}
