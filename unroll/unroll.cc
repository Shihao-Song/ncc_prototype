#include <iostream>
#include <fstream>
#include <string>
#include <boost/tokenizer.hpp>
#include <vector>
#include "unroll.h"

/* extract_max_neuron_id accepts an the connection file name and obtains the max neuron id */
int extract_max_neuron_id(const std::string file_name)
{
    std::fstream file;
    file.open(file_name, std::ios::in);

    int max_id = -1;
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
            int nid = std::stoi(*i);
            if (nid > max_id)
            {
                max_id = nid;
            }
        }
    }
    file.close();
    return max_id;
}

/* read_spike_info reads the spike file data and returns a data structure containing 
information about the spikes */
std::vector<Spike> read_spike_info(const std::string spike_file_name)
{
    std::fstream file;
    file.open(spike_file_name, std::ios::in);

    std::string line;
    typedef boost::tokenizer<boost::char_separator<char>> tok_t;
    boost::char_separator<char> sep(" ", "", boost::keep_empty_tokens);

    std::vector<Spike> spikes;
    while (std::getline(file, line))
    {
        int source_neuron;
        tok_t tok(line, sep);
        bool source = true;
        std::vector<int> spike_times;
        for (tok_t::iterator i = tok.begin(); i != tok.end(); ++i)
        {
            if (*i == "")
            {
                continue;
            }
            /* the first element of each line is the sourc neuron */
            if (source)
            {
                source_neuron = std::stoi(*i);
                source = false;
                continue;
            }

            spike_times.push_back(std::stoi(*i));
        }
        Spike new_spike = Spike(source_neuron);
        new_spike.add_spike_times(spike_times);
        spikes.push_back(new_spike);
    }

    file.close();
    return spikes;
}

/* read_connection_info reads in the connection_file and returns a data structure containing 
information about the neurons */
std::vector<Neuron> read_connection_info(const std::string connection_file_name, const std::string spike_file_name)
{
    int max_neuron_id = extract_max_neuron_id(connection_file_name);
    std::vector<Neuron> snn;
    int i;
    for (i = 0; i < max_neuron_id + 1; i++)
    {
        Neuron new_neuron = Neuron(i);
        snn.push_back(new_neuron);
    }
    std::vector<Spike> spikes = read_spike_info(spike_file_name);
    for (i = 0; i < spikes.size(); i++)
    {
        snn[spikes[i].get_source_id()].add_spike(spikes[i]);
    }

    std::fstream file;
    file.open(connection_file_name, std::ios::in);

    std::string line;
    typedef boost::tokenizer<boost::char_separator<char>> tok_t;
    boost::char_separator<char> sep(" ", "", boost::keep_empty_tokens);

    int lcount = 0;
    while (std::getline(file, line))
    {
        int source_neuron;
        tok_t tok(line, sep);
        std::vector<int> edgeList;
        bool first = true;
        for (tok_t::iterator i = tok.begin(); i != tok.end(); ++i)
        {
            if (*i == "")
            {
                continue;
            }
            if (first)
            {
                first = false;
                continue;
            }

            int edge = std::stoi(*i);
            edgeList.push_back(edge);
        }

        int j;

        for (j = 0; j < edgeList.size(); j++)
        {
            snn[source_neuron].add_output(edgeList[j]);
            snn[edgeList[j]].add_input(source_neuron);
        }
        lcount++;
    }
    file.close();
    return snn;
}

std::vector<Neuron> unroll_neuron(Neuron n, int start_idx)
{
    std::vector<Neuron> unrolled_neurons;

    std::vector<int> inputs = n.get_input_list();
    std::vector<int> outputs = n.get_output_list();

    unsigned long number_unrolled_neurons = inputs.size() - 1;

    unsigned long i;
    for (i = 0; i < number_unrolled_neurons; i++)
    {
        Neuron new_neuron = Neuron(start_idx + 1);
        unrolled_neurons.push_back(new_neuron);
    }

    unrolled_neurons[0].add_input(inputs[0]);
    unrolled_neurons[0].add_input(inputs[1]);
    unrolled_neurons[0].add_output(unrolled_neurons[1].get_id());
    unrolled_neurons[0].add_spike(n.get_spike());

    for (i = number_unrolled_neurons - 2; i > 0; i--)
    {
        unrolled_neurons[i].add_input(inputs[i + 1]);
        unrolled_neurons[i].add_input(unrolled_neurons[i - 1].get_id());
        unrolled_neurons[i].add_output(unrolled_neurons[i + 1].get_id());
        unrolled_neurons[i].add_spike(n.get_spike());
        unrolled_neurons[i].add_parent(unrolled_neurons[i - 1].get_id());
    }

    unrolled_neurons[number_unrolled_neurons - 1].add_input(inputs[number_unrolled_neurons]);
    unrolled_neurons[number_unrolled_neurons - 1].add_input(unrolled_neurons[number_unrolled_neurons - 2].get_id());

    unsigned long a;
    for (a = 0; a < outputs.size(); a++)
    {
        unrolled_neurons[number_unrolled_neurons - 1].add_output(a);
    }
    unrolled_neurons[number_unrolled_neurons - 1].add_spike(n.get_spike());

    return unrolled_neurons;
}

std::vector<int> argsort(std::vector<int> arg)
{
    /* reverse, then sort */
    return arg;
}

void unroll_snn(std::vector<Neuron> snn)
{
    std::vector<Neuron> usnn;
    int i;
    std::vector<int> fanins;

    for (i = 0; i < snn.size(); i++)
    {
        std::vector<int> n = snn[i].get_input_list();
        int j;
        for (j = 0; j < n.size(); j++)
        {
            fanins.push_back(n[j]);
        }
    }

    std::vector<int> sorted_neuron_id = agrsort(fanins);
    std::vector<Neuron> unrolled_neurons;
    int next_neuron_id = snn.size();
    for (i = 0; i < sorted_neuron_id.size(); i++)
    {
        int idx = sorted_neuron_id[i];
        Neuron n = snn[idx];
        std::vector<int> fanin = n.get_input_list();
        std::vector<int> fanout = n.get_output_list();

        if (fanin.size() > 2 && fanout.size() > 0)
        {
            std::vector<Neuron> more = unroll_neuron(n, next_neuron_id);
            unrolled_neurons.insert(unrolled_neurons.end(), more.begin(), more.end());
                }
    }

    return;
}

void unroll_generic(std::vector<Neuron> snn, int max_fanin)
{

    return;
}

int main()
{
    std::vector<Neuron> snn = read_connection_info("LeNet.connection_info.txt", "LeNet.weight_info.txt");
    printf("%lu\n", snn[100].get_input_list().size());
}