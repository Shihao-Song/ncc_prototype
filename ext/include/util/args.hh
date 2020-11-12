#include <string>

namespace EXT
{
class Argument
{
  public:
    static const unsigned INVALID = (unsigned) - 1;

  protected:
    std::string connection_file = "N/A";
    std::string spike_file = "N/A";
    std::string unrolled_ir_output = "N/A";
    std::string unrolled_parent_output = "N/A"; // Parent of each unrolled neurons

    std::string cluster_ir_output = "N/A";
    std::string cluster_stats = "N/A";

    unsigned unrolled_fanin = INVALID;
    unsigned crossbar_size = INVALID;

  public:
    Argument(int argc, char **argv);

    auto getFanin() { return unrolled_fanin; }
    auto &getConnFile() { return connection_file; }
    auto &getSpikeFile() { return spike_file; }
    auto &getUnrolledIROutputFile() { return unrolled_ir_output; }
    auto &getUnrolledParentOutputFile() { return unrolled_parent_output; }
    auto &getClusterIROutputFile() { return cluster_ir_output; }
    auto &getClusterStatsFile() { return cluster_stats; }
};
}
