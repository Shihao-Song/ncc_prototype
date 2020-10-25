#ifndef __UNROLL_H__
#define __UNROLL_H__
#include <vector>

class Spike
{
private:
    int source_id;
    int nspikes;
    std::vector<int> spike_times;

public:
    Spike();
    Spike(int);
    int get_source_id() { return source_id; };
    int get_nspikes() { return nspikes; };
    std::vector<int> get_spike_times() { return spike_times; };
    void set_spike_times(std::vector<int>);
    void add_spike_times(std::vector<int>);
};

Spike::Spike()
{
    source_id = -1;
    nspikes = 0;
    spike_times = std::vector<int>();
}

Spike::Spike(int sid)
{
    source_id = sid;
}

void Spike::set_spike_times(std::vector<int> spikes)
{
    spike_times = spikes;
    nspikes = spike_times.size();
}

void Spike::add_spike_times(std::vector<int> spikes)
{
    spike_times.insert(spike_times.end(), spikes.begin(), spikes.end());
    nspikes = spike_times.size();
}

class Neuron
{
private:
    int neuron_id;
    std::vector<int> inEdges = {};
    std::vector<int> outEdges = {};
    Spike spk;
    int parent;

public:
    Neuron();
    Neuron(int);
    void add_input(int);
    void add_output(int);
    void add_input_list(std::vector<int>);
    void add_output_list(std::vector<int>);
    void add_spike(Spike);
    void add_parent(int);
    void set_id(int);
    int get_id() { return neuron_id; };
    int get_parent() { return parent; };
    Spike get_spike() { return spk; };
    std::vector<int> get_input_list() { return inEdges; };
    std::vector<int> get_output_list() { return outEdges; };
    std::vector<int> get_spike_times() { return spk.get_spike_times(); };
};

Neuron::Neuron()
{
    neuron_id = -1;
    inEdges = std::vector<int>();
    outEdges = std::vector<int>();
    parent = -1;
    spk = Spike();
}

Neuron::Neuron(int id)
{
    neuron_id = id;
    parent = -1;
    spk = Spike();
}

void Neuron::add_input(int in)
{
    inEdges.push_back(in);
}

void Neuron::set_id(int id)
{
    neuron_id = id;
}

void Neuron::add_output(int out)
{
    outEdges.push_back(out);
}

void Neuron::add_input_list(std::vector<int> ins)
{
    inEdges.insert(inEdges.end(), ins.begin(), ins.end());
}

void Neuron::add_output_list(std::vector<int> outs)
{
    outEdges.insert(outEdges.end(), outs.begin(), outs.end());
}

void Neuron::add_spike(Spike s)
{
    spk = s;
}

void Neuron::add_parent(int p)
{
    parent = p;
}

#endif
