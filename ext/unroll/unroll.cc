#include <algorithm>
#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>
#include <cmath>
#include <fstream>
#include <string>

#include "unroll.h"

namespace Unrolling
{
Argument::Argument(int argc, char **argv)
{
    namespace po = boost::program_options;
    po::options_description desc("Options"); 
    desc.add_options() 
        ("help", "Print help messages")
        ("conn-file", po::value<std::string>(&connection_file)->required(),
                 "Connection file")
        ("out-file", po::value<std::string>(&unrolled_output)->required(),
                   "Unrolled SNN output file")
        ("debug-out-file", po::value<std::string>(&debug_output),
                   "Details of the unrolled SNN")
        ("fanin", po::value<unsigned>(&fanin)->required(),
                   "fanin");

   po::variables_map vm;

    try 
    { 
        po::store(po::parse_command_line(argc, argv, desc), vm); // can throw 
 
        if (vm.count("help")) 
        { 
            std::cout << "SNN unrolling extension.\n" 
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

UINT64 Model::extractMaxNeuronId(const std::string &file_name)
{
    std::fstream file;
    file.open(file_name, std::ios::in);

    UINT64 max_id = 0;
    std::string line;
    typedef boost::tokenizer<boost::char_separator<char>> tok_t;
    boost::char_separator<char> sep(" ", "", boost::keep_empty_tokens);
    while (std::getline(file, line))
    {
        tok_t tok(line, sep);
        for (tok_t::iterator i = tok.begin(); i != tok.end(); ++i)
        {
            if (*i == "")
            {
                continue;
            }
            UINT64 nid = std::stoull(*i);
            if (nid > max_id)
            {
                max_id = nid;
            }
        }
    }
    file.close();
    return max_id;
}

// Edits
// Basically a copy from Jacob's codes
void Model::readConnections(const std::string &connection_file_name)
{
    int max_neuron_id = extractMaxNeuronId(connection_file_name);
    // std::cout << "Max neuron ID: " << max_neuron_id << "\n";

    // Initialize neurons with IDs 
    for (auto i = 0; i <= max_neuron_id; i++)
    {
        snn.emplace_back(i);
    }

    // TODO, consider weights/spikes in the future

    // Parse the connection information
    std::fstream file;
    file.open(connection_file_name, std::ios::in);

    std::string line;
    typedef boost::tokenizer<boost::char_separator<char>> tok_t;
    boost::char_separator<char> sep(" ", "", boost::keep_empty_tokens);

    while (std::getline(file, line))
    {
        UINT64 source_neuron;
        tok_t tok(line, sep);
        std::vector<UINT64> out_neuron_list;
        bool first = true;
        for (tok_t::iterator i = tok.begin(); i != tok.end(); ++i)
        {
            if (*i == "")
            {
                continue;
            }
            if (first)
            {
                source_neuron = std::stoull(*i);
                first = false;
                continue;
            }

            UINT64 out_neuron = std::stoull(*i);
            out_neuron_list.push_back(out_neuron);
        }

        for (auto i = 0; i < out_neuron_list.size(); i++)
        {
            snn[source_neuron].addOutputNeuron(out_neuron_list[i]);
            snn[out_neuron_list[i]].addInputNeuron(source_neuron);
        }
    }
    file.close();
}

// A simple proof-of-concept version of unroll
void Model::unroll()
{
    // Initialize the unrolled neurons
    for (auto i = 0; i < snn.size(); i++)
    {
        // emplace_back should be more space-efficient than push_back
        usnn.emplace_back(snn[i]);
    }
 
    // Look for all the neurons that have more than max_fanin number of inputs
    for (auto idx = 0; idx < snn.size(); idx++)
    {
        UINT64 prev_unrolling_neuron_id = usnn.size();
        UINT64 cur_unrolling_neuron_id = usnn.size();

        auto input_neurons_copy = usnn[idx].getInputNeuronListCopy();

        // Check if the number of inputs exceed max_fanin
        if (input_neurons_copy.size() > max_fanin)
        {
            // Step one, reset the output neuron of all the input neurons
            for (auto &input_neuron : input_neurons_copy)
            {
                auto &out = usnn[input_neuron].getOutputNeuronList();
                out.erase(std::remove(out.begin(), out.end(), usnn[idx].getNeuronId()));
            }

            // Step two, reset the input neurons of the currently processing neuron
            usnn[idx].getInputNeuronList().clear();
            usnn[idx].getInputNeuronList().shrink_to_fit();

            // Step three, unrolling
            // The number of intermediate neurons to unroll the current neuron
            //     = ceil((#inputs - #fanin) / (#fanin - 1)) + 1
            unsigned num_inputs = input_neurons_copy.size();
            unsigned num_inter_neurons = std::ceil(((float)num_inputs - (float)max_fanin) / 
                                                   ((float)max_fanin - 1)) + 1;
            // std::cout << num_inter_neurons << "\n"; exit(0);
            for (auto inter_neu_idx = 0; inter_neu_idx < num_inter_neurons; inter_neu_idx++)
            {
                if (inter_neu_idx == 0)
                {
                    usnn.emplace_back(cur_unrolling_neuron_id);

                    for (auto i = 0; i < max_fanin; i++)
                    {
                        usnn[input_neurons_copy[i]].getOutputNeuronList().push_back(
                            cur_unrolling_neuron_id);

                        usnn[cur_unrolling_neuron_id].getInputNeuronList().push_back(
                            input_neurons_copy[i]);
                    }

                    cur_unrolling_neuron_id++;
                }
                else if (inter_neu_idx == num_inter_neurons - 1)
                {
                    usnn[prev_unrolling_neuron_id].getOutputNeuronList().push_back(
                        usnn[idx].getNeuronId());
                    usnn[usnn[idx].getNeuronId()].getInputNeuronList().push_back(
                        prev_unrolling_neuron_id);

                    for (auto i = max_fanin + (inter_neu_idx - 1) * (max_fanin - 1);
                              i < num_inputs;
                              i++)
                    {

                        usnn[input_neurons_copy[i]].getOutputNeuronList().push_back(
                            usnn[idx].getNeuronId());
                        usnn[usnn[idx].getNeuronId()].getInputNeuronList().push_back(
                            input_neurons_copy[i]);
                    }
                }
                else
                {
                    usnn.emplace_back(cur_unrolling_neuron_id);

                    usnn[prev_unrolling_neuron_id].getOutputNeuronList().push_back(
                        cur_unrolling_neuron_id);
                    usnn[cur_unrolling_neuron_id].getInputNeuronList().push_back(
                        prev_unrolling_neuron_id);

                    
                    for (auto i = max_fanin + (inter_neu_idx - 1) * (max_fanin - 1); 
                              i < max_fanin + inter_neu_idx * (max_fanin - 1);
                              i++)
                    {
                        usnn[input_neurons_copy[i]].getOutputNeuronList().push_back(
                            cur_unrolling_neuron_id);
                        usnn[cur_unrolling_neuron_id].getInputNeuronList().push_back(
                            input_neurons_copy[i]);
                    }

                    prev_unrolling_neuron_id = cur_unrolling_neuron_id;
                    cur_unrolling_neuron_id++;
                }
            }
        }
    }

    // Check for the disconnected neurons
     
    // for (auto &neuron : usnn) { neuron.print_connections(); }
}

void Model::output(const std::string &out_name)
{
    std::fstream file;
    file.open(out_name, std::fstream::out);

    for (auto &neuron : usnn)
    {
        assert(neuron.getInputNeuronList().size() <= max_fanin);
        auto &output_neurons = neuron.getOutputNeuronList();
        for (auto &output : output_neurons)
        { 
            file << neuron.getNeuronId() << " ";
            file << output << " ";
            file << "\n";
        }
        // if (output_neurons.size() == 0) { std::cout << neuron.getNeuronId() << std::endl; }
    }

    file.close();
    return;
}
}

// First of all, you don't want to return back a large vector
// The frequent copy and free can kill the performance and memory
// A better alternative way is to put those rolled/unrolled SNNs into another class.
int main(int argc, char **argv)
{
    Unrolling::Argument args(argc, argv);

    Unrolling::Model model;
    model.readConnections(args.getConnFile());

    model.setFanin(args.getFanin());
    model.unroll();
    model.output(args.getOutputFile());

    if (auto &debug_out = args.getDebugOutputFile();
        debug_out != "N/A")
    {
        model.debugOutput(debug_out);
    }

    return 0;
}
