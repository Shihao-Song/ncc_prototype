#ifndef __CLUSTERING_H__
#define __CLUSTERING_H__

#include <vector>
#include "unroll.h"
bool isPresent(int needle, std::vector<int> haystack)
{
    int i;
    for (i = 0; i < haystack.size(); i++)
    {
        if (i == haystack[i])
        {
            return true;
        }
    }
    return false;
}

class Node
{
private:
    int nodeID;
    std::vector<int> edges;
    int token;

public:
    Node();
    Node(int);
    int get_node() { return nodeID; };
    void add_edges(std::vector<int>);
    std::vector<int> get_edges() { return edges; };
    void add_token(int);
    int get_token() { return token; };
} Node::Node()
{
    nodeID = -1;
}

Node::Node(int id)
{
    nodeID = id;
}

void add_edges(std::vector<int> new_edges)
{
    edges.insert(edges.end(), new_edges.begin(), new_edges.end());
}

void add_token(int to)
{
    token = to;
}

class Cluster
{
private:
    int cID;
    std::vector<Neuron> neurons;
    std::vector<int> inputs;
    std::vector<int> outputs;
    std::vector<int> parents;

public:
    Cluster();
    Cluster(int);
    void AddNeuron(Neuron);
    void AddParent(int);
    void AddInput(std::vector<int>);
    void AddOutput(std::vector<int>);
    std::vector<int> get_parent { return parents; };
    std::vector<int> get_input { return inputs; };
    std::vector<int> get_output { return outputs; };
    std::vector<Neuron> get_neurons { return neurons; };
}

Cluster::Cluster()
{
    cID = -1;
}

Cluster::Cluster(int id)
{
    cID = id;
}

void AddParent(int parent)
{
    parent.push_back(parent);
    std::unique(parent.begin(), parent.end());
}

void AddInput(std::vector<int> new_input)
{
    inputs.insert(inputs.end(), new_input.begin(), new_input.end());
    std::unique(inputs);
}

void AddOutput(std::vector<int> new_output)
{
    outputs.insert(outputs.end(), new_output.begin(), new_output.end());
    std::unique(outputs);
}

void AddNeuron(Neuron n)
{
    std::vector<int> n_inputs = n.get_input_list();
    int i;
    for (i = 0; i < n_inputs.size(); i++)
    {
        int ip = n_inputs[i];
        if (isPresent(ip, n_inputs) || isPresent(ip, outputs))
        {
            continue;
        }
        else
        {
            inputs.push_back(ip);
        }
    }

    neurons.push_back(n);
    outputs.insert(outputs.end(), n.get_output_list().begin(), n.get_output_list.end());
    std::unqiue(outputs.begin(), outputs.end());
}
