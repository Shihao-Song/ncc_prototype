#include <boost/program_options.hpp>
#include <iostream>

#include "args.hh"

namespace DI
{
Argument::Argument(int argc, char **argv)
{
    namespace po = boost::program_options;
    po::options_description desc("Options");
    desc.add_options()
        ("help", "Print help messages")
        ("conn-file", po::value<std::string>(&connection_file)->required(),
                 "Connection file")
        ("neuron-depth-ir", po::value<std::string>(&neuron_depth_ir),
                   "Neuron depth IR output file")
        ("longterm-conn-stats", po::value<std::string>(&longterm_conn_stats),
                   "Clustering raw stats of number of connections");

    po::variables_map vm;

    try
    {
        po::store(po::parse_command_line(argc, argv, desc), vm); // can throw 

        if (vm.count("help"))
        {
            std::cout << "Depth identification args: \n"
                      << desc << "\n";
            exit(0);
        }

        po::notify(vm);
    }
    catch(po::error& e)
    {
        std::cerr << "ERROR: " << e.what() << "\n\n";
        std::cerr << desc << "\n";
        exit(0);
    }
}
}
