#include <string>

namespace DI
{
class Argument
{
  public:
    static const unsigned INVALID = (unsigned) - 1;

  protected:
    std::string connection_file = "N/A";
    std::string neuron_depth_ir = "N/A"; 
    std::string longterm_conn_stats = "N/A";

    // unsigned unrolled_fanin = INVALID;

  public:
    Argument(int argc, char **argv);

    std::string& getConnFile() // This file is required
    { 
        return connection_file;
    }

    std::pair<bool,std::string&> getNeuronDepthIROutputFile()
    {
        bool valid = (neuron_depth_ir != "N/A") ? true : false; 
        return std::pair<bool,std::string&>(valid, neuron_depth_ir);
    }

    std::pair<bool,std::string&> getLongtermConnStatsFile()
    {
        bool valid = (longterm_conn_stats != "N/A") ? true : false;
        return std::pair<bool,std::string&>(valid, longterm_conn_stats);
    }
};
}
