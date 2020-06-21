#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "proto_graph/graph.pb.h"

#include <boost/filesystem.hpp>

int main(int argc, char* argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    if (argc != 2)
    {
        std::cerr << "Usage example:  " << argv[0] << " input.graph" << std::endl;
        return -1;
    }

    std::string conns_out_txt = "check.connection_info.txt";
    std::ofstream conns_out(conns_out_txt);

    std::string weights_out_txt = "check.weight_info.txt";
    std::ofstream weights_out(weights_out_txt);

    boost::filesystem::path graph_root(argv[1]);

    unsigned sub_graph_idx = 0;
    boost::filesystem::path sub_graph = std::to_string(sub_graph_idx) + ".graph";
    boost::filesystem::path sub_graph_full = graph_root / sub_graph;

    while (boost::filesystem::exists(sub_graph_full))
    {
        std::ifstream input(sub_graph_full.string());
        NCC_Graph_Proto::Graph graph;
        if (!graph.ParseFromIstream(&input))
        {
            std::cerr << "Failed to parse the graph." << std::endl;
            return -1;
        }
        input.close();

        for (int i = 0; i < graph.nodes_size(); i++)
        {
            const NCC_Graph_Proto::Node &node = graph.nodes(i);

            weights_out << node.id() << " ";
            conns_out << node.id() << " ";

            for (int j = 0; j < node.adjs_size(); j++)
            {
                conns_out << node.adjs(j) << " ";
                weights_out << node.weights(j) << " ";
            }
            conns_out << "\n";
            weights_out << "\n";
        }

        sub_graph_idx++;
        sub_graph = std::to_string(sub_graph_idx) + ".graph";
        sub_graph_full = graph_root / sub_graph;
    }

    weights_out.close();
    conns_out.close();
}
