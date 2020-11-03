#ifndef __UNROLL_H__
#define __UNROLL_H__

#include <iostream>
#include <vector>

namespace Unrolling
{
typedef uint64_t UINT64;

class Neuron
{
  protected:
    UINT64 neuron_id;
    std::vector<UINT64> input_neurons;
    std::vector<UINT64> output_neurons;

  public:
    Neuron() {}
    Neuron(UINT64 _id) : neuron_id(_id) {}
    Neuron(const Neuron &_copy) : neuron_id(_copy.neuron_id),
                                  input_neurons(_copy.input_neurons),
                                  output_neurons(_copy.output_neurons) {}

    void addInputNeuron(UINT64 _in_neuron)
    {
        input_neurons.push_back(_in_neuron);
    }
    void addOutputNeuron(UINT64 _out_neuron)
    {
        output_neurons.push_back(_out_neuron);
    }

    void addInputNeuronList(std::vector<UINT64> &_list)
    {
        input_neurons = _list;
    }
    void addOutputNeuronList(std::vector<UINT64> &_list)
    {
        output_neurons = _list;
    }

    void setNeuronId(UINT64 _id) { neuron_id = _id; }
    int getNeuronId() { return neuron_id; };

    std::vector<UINT64> &getInputNeuronList() { return input_neurons; };
    std::vector<UINT64> &getOutputNeuronList() { return output_neurons; };

    std::vector<UINT64> getInputNeuronListCopy() { return input_neurons; };
    std::vector<UINT64> getOutputNeuronListCopy() { return output_neurons; };


    void print_connections()
    {
        std::cout << "Neuron ID: " << neuron_id << "\n";
        std::cout << "Input Neuron IDs: ";
        for (auto &input : input_neurons) { std::cout << input << " "; } std::cout << "\n";
        std::cout << "Output Neuron IDs: ";
        for (auto &output : output_neurons) { std::cout << output << " "; } std::cout << "\n";
        std::cout << "\n";
    }
};

class Model
{
  protected:
    std::vector<Neuron> snn;
    std::vector<Neuron> usnn;

  public:
    Model() {}
    void readConnections(const std::string);
    void unroll(unsigned);

    void output(const std::string out_name);

  protected:
    UINT64 extractMaxNeuronId(const std::string file_name);    
};
}

#endif
