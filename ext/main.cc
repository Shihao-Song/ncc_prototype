#include "util/args.hh"
#include "unroll/unroll.hh"

typedef EXT::Argument Argument;
typedef EXT::Unrolling::Model Model;

int main(int argc, char **argv)
{
    Argument args(argc, argv);

    Model model(args.getConnFile(),
                args.getSpikeFile());

    if (auto fanin = args.getFanin();
        fanin != EXT::Argument::INVALID)
    {
        model.setFanin(args.getFanin());
        model.unroll();
    }

    if (auto &unrolled_ir = args.getUnrolledIROutputFile();
        unrolled_ir != "N/A")
    {
        model.outputUnrolledIR(unrolled_ir);
    }

    if (auto &parent_neu_out = args.getUnrolledParentOutputFile();
        parent_neu_out != "N/A")
    {
        model.parentNeuronOutput(parent_neu_out);
    }

    // Clustering
    std::string test = "fcfs";
    model.clustering(test);    

    if (auto &cluster_ir = args.getClusterIROutputFile();
        cluster_ir != "N/A")
    {
        // std::cout << cluster_ir << "\n";
        model.printClusterIR(cluster_ir);
    }
    
    if (auto &cluster_stats = args.getClusterStatsFile();
        cluster_stats != "N/A")
    {
        // std::cout << cluster_stats << "\n";
        model.printClusterStats(cluster_stats);
    }

    return 0;
}
