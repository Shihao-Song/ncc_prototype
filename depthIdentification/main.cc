#include "include/args.hh"
// #include "unroll/unroll.hh"
// #include "cluster/cluster.hh"
#include "include/model.hh"

typedef DI::Argument Argument;
typedef DI::Model Model;

int main(int argc, char **argv)
{
    Argument args(argc, argv);

    Model model(args.getConnFile());
    if (auto [enable, outputIRFile] = args.getNeuronDepthIROutputFile(); enable) {
        model.outputNeuronDepthIR(outputIRFile);
    }
    if (auto [enable, connDepthFile] = args.getLongtermConnStatsFile(); enable) {
        model.printNeuronConnDepth(connDepthFile);
    }

    // if (auto [enable, fanin] = args.getFanin(); enable)
    // {
    //     model.setFanin(fanin);
    //     model.unroll();
    // }
    // else
    // {
    //     std::cerr << "---------------------------------------\n";
    //     std::cerr << "Warning: fanin is not provided.\n";
    //     std::cerr << "Warning: model is not unrolled.\n";
    // }

    // if (auto [enable, algo] = args.getClusteringAlgo(); enable)
    // {
    //     if (auto [valid, crossbar_size] = args.getCrossbarSize(); valid)
    //     {
    //         model.clustering(algo,crossbar_size);
    //     }
    //     else
    //     {
    //         std::cerr << "---------------------------------------\n";
    //         std::cerr << "Warning: crossbar size is not provided.\n";
    //         std::cerr << "Warning: clustering is disabled.\n";
    //     }
    // }
    // else
    // {
    //     std::cerr << "---------------------------------------\n";
    //     std::cerr << "Warning: clustering algorithm is not provided.\n";
    //     std::cerr << "Warning: clustering is disabled.\n";
    // }

    // if (auto [valid, unrolled_ir] = args.getUnrolledIROutputFile(); valid)
    // {
    //     model.outputUnrolledIR(unrolled_ir);
    // }
    // if (auto [valid, parent_neu_out] = args.getUnrolledParentOutputFile(); valid)
    // {
    //     model.parentNeuronOutput(parent_neu_out);
    // }
    // if (auto [valid, cluster_ir] = args.getClusterIROutputFile(); valid)
    // {
    //     model.printClusterIR(cluster_ir);
    // }
    // if (auto [valid, cluster_stats] = args.getClusterStatsFile(); valid)
    // {
    //     // std::cout << cluster_stats << "\n";
    //     int mode = EXT::Clustering::NEURON_SPECIFIC;
    //     model.printClusterStats(cluster_stats, mode);
    // }

    // if (auto [valid, cluster_conn_stats] = args.getClusterConnStatsFile(); valid)
    // {
    //     std::cout << "Printing cluster connection stats\n"; 
    //     int mode = EXT::Clustering::NUM_CONNECTIONS;
    //     mode = mode | EXT::Clustering::CONNECTION_SUMMARY;
    //     model.printClusterStats(cluster_conn_stats, mode);
    // }
    
    return 0;
}