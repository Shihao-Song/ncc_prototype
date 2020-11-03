#include <fstream>
#include <string>
#include <boost/tokenizer.hpp>

#include "unroll.h"

namespace Unrolling
{
/*
Neuron::Neuron(Neuron &_copy) : neuron_id(_copy.neuron_id),
                                input_neurons(_copy.input_neurons),
                                output_neurons(_copy.output_neurons) {}
*/

UINT64 Model::extractMaxNeuronId(const std::string file_name)
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
void Model::readConnections(const std::string connection_file_name)
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

// TODO, upload an example picture to explain the codes
// A simple proof-of-concept version of unroll
void Model::unroll(unsigned max_fanin)
{
    // Initialize the unrolled neurons
    for (auto i = 0; i < snn.size(); i++)
    {
        // emplace_back should be more space-efficient than push_back
        usnn.emplace_back(i);
    }

    // Look for all the neurons that have more than max_fanin number of inputs
    for (auto &neuron : snn)
    {
        UINT64 prev_unrolling_neuron_id = usnn.size();
        UINT64 cur_unrolling_neuron_id = usnn.size();

        auto &input_neurons = neuron.getInputNeuronList();
        // Check if the number of inputs exceed max_fanin
        if (input_neurons.size() > max_fanin)
        {
            // std::cout << neuron.getNeuronId() << "\n";

            // We need total sizeof(input_neurons)-1 neurons to unroll (the current neuron)
            for (auto i = 0; i < input_neurons.size() - 1; i++)
            {
                if (i == 0)
                {
                    usnn.emplace_back(cur_unrolling_neuron_id);

                    // the first unrolling neuron takes the first two input neurons as inputs
                    usnn[input_neurons[0]].getOutputNeuronList().push_back(cur_unrolling_neuron_id);
                    usnn[input_neurons[1]].getOutputNeuronList().push_back(cur_unrolling_neuron_id);

                    usnn[cur_unrolling_neuron_id].getInputNeuronList().push_back(input_neurons[0]);
                    usnn[cur_unrolling_neuron_id].getInputNeuronList().push_back(input_neurons[1]);

                    cur_unrolling_neuron_id++;

                }
                // The last unrolling neuron
                else if (i == (input_neurons.size() - 1 - 1))
                {
                    usnn[prev_unrolling_neuron_id].getOutputNeuronList().push_back(
                        neuron.getNeuronId());
                    usnn[input_neurons[1 + i]].getOutputNeuronList().push_back(
                        neuron.getNeuronId());

                    usnn[neuron.getNeuronId()].getInputNeuronList().push_back(
                        prev_unrolling_neuron_id);
                    usnn[neuron.getNeuronId()].getInputNeuronList().push_back(
                        input_neurons[1 + i]);
                }
                // All middle unrolling neurons
                else
                {
                    usnn.emplace_back(cur_unrolling_neuron_id);

                    usnn[prev_unrolling_neuron_id].getOutputNeuronList().push_back(
                        cur_unrolling_neuron_id);
                    usnn[input_neurons[1 + i]].getOutputNeuronList().push_back(
                        cur_unrolling_neuron_id);

                    usnn[cur_unrolling_neuron_id].getInputNeuronList().push_back(
                        prev_unrolling_neuron_id);
                    usnn[cur_unrolling_neuron_id].getInputNeuronList().push_back(
                        input_neurons[1 + i]);

                    prev_unrolling_neuron_id = cur_unrolling_neuron_id;
                    cur_unrolling_neuron_id++;
                    
                }
	    }
        }
    }

    // Check for the disconnected neurons
     

    // for (auto &neuron : usnn) { neuron.print_connections(); } exit(0);
}

void Model::output(const std::string out_name)
{
    std::fstream file;
    file.open(out_name, std::fstream::out);

    for (auto &neuron : usnn)
    {
        file << neuron.getNeuronId() << " ";

        auto &output_neurons = neuron.getOutputNeuronList();
        for (auto &output : output_neurons) { file << output << " "; } file << "\n";
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
    Unrolling::Model model;
    model.readConnections(argv[1]);
    model.unroll(2);
    model.output("unrolled_out.txt");

    return 0;
}
