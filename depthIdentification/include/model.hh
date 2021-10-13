#ifndef __MODEL_H__
#define __MODEL_H__

#include <boost/multiprecision/cpp_int.hpp> 

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace DI
{
typedef uint64_t UINT64;

const UINT64 INVALID_ID = (UINT64) - 1;

class Neuron
{
  protected:
    UINT64 neuron_id;
    std::vector<UINT64> input_neuron_ids;
    std::vector<UINT64> output_neuron_ids;
    UINT64 depth = -1;

  public:
    Neuron() {}
    Neuron(UINT64 _id) : neuron_id(_id) {}
    Neuron(const Neuron &_copy) : neuron_id(_copy.neuron_id),
                                  input_neuron_ids(_copy.input_neuron_ids),
                                  output_neuron_ids(_copy.output_neuron_ids) {}
                                  // input_neurons(_copy.input_neurons),
                                  // output_neurons(_copy.output_neurons){}

    void addInputNeuronID(UINT64 _in_neuron)
    {
        input_neuron_ids.push_back(_in_neuron);
    }
    void addOutputNeuronID(UINT64 _out_neuron)
    {
        output_neuron_ids.push_back(_out_neuron);
    }
    // void addInputNeuron(Neuron _in_neuron)
    // {
    //     input_neurons.push_back(_in_neuron);
    // }
    // void addInputNeuron(UINT64 _in_neuron_id)
    // {
    //     output_neurons.emplace_back(_in_neuron_id);
    // }
    // void addOutputNeuron(Neuron _out_neuron)
    // {
    //     output_neurons.push_back(_out_neuron);
    // }
    // void addOutputNeuron(UINT64 _out_neuron_id)
    // {
    //     output_neurons.emplace_back(_out_neuron_id);
    // }

    void addInputNeuronIDList(std::vector<UINT64> &_list)
    {
        input_neuron_ids = _list;
    }
    void addOutputNeuronIDList(std::vector<UINT64> &_list)
    {
        output_neuron_ids = _list;
    }
    // void addInputNeuronList(std::vector<Neuron> &_list)
    // {
    //     input_neurons = _list;
    // }
    // void addOutputNeuronList(std::vector<Neuron> &_list)
    // {
    //     output_neurons = _list;
    // }
    
    void setNeuronId(UINT64 _id) { neuron_id = _id; }
    UINT64 getNeuronId() { return neuron_id; };

    void setDepth(UINT64 _depth) { depth = _depth; }
    int getDepth() { return depth; };

    // TODO, change this to ...Ref()
    std::vector<UINT64> &getInputNeuronIDList() { return input_neuron_ids; };
    std::vector<UINT64> &getOutputNeuronIDList() { return output_neuron_ids; };
    // std::vector<Neuron> &getInputNeuronList() { return input_neurons; };
    // std::vector<Neuron> &getOutputNeuronList() { return output_neurons; };
    unsigned numInputNeurons() { return input_neuron_ids.size(); }
    unsigned numOutputNeurons() { return output_neuron_ids.size(); }

    std::vector<UINT64> getInputNeuronIDListCopy() { return input_neuron_ids; };
    std::vector<UINT64> getOutputNeuronIDListCopy() { return output_neuron_ids; };
    // std::vector<Neuron> getInputNeuronListCopy() { return input_neurons; };
    // std::vector<Neuron> getOutputNeuronListCopy() { return output_neurons; };

    void print_connections()
    {
        std::cout << "Neuron ID: " << neuron_id << "\n";
        std::cout << "Input Neuron IDs: ";
        for (auto &input : input_neuron_ids) { std::cout << input << " "; } std::cout << "\n";
        std::cout << "Output Neuron IDs: ";
        for (auto &output : output_neuron_ids) { std::cout << output << " "; } std::cout << "\n";
        std::cout << "\n";
    }
};

class Model
{
  protected:
    std::vector<Neuron> snn;
    std::set<UINT64> snn_neuron_ids;

    unsigned max_fanin = (unsigned) - 1; // unrolling

  public:
    Model(const std::string&);
    ~Model();
    void labelNeuronWithDepth(UINT64 starting_depth, std::set<UINT64>&);
    void outputNeuronDepthIR(const std::string&);
    void printNeuronConnDepth(const std::string&);
    void debugOutput(const std::string &out_name)
    {
        std::fstream file;
        file.open(out_name, std::fstream::out);

        for (auto &neuron : snn)
        {
            file << "Neuron ID: " << neuron.getNeuronId() << "\n";

            auto &input_neurons = neuron.getInputNeuronIDList();
            auto &output_neurons = neuron.getOutputNeuronIDList();

            file << "Input Neuron IDs: ";
            for (auto &input : input_neurons) { file << input << " "; } file << "\n";
            file << "Output Neuron IDs: ";
            for (auto &output : output_neurons) { file << output << " "; } file << "\n";
            file << "\n";
        }
        file.close();
        return;
    }

  protected:
    UINT64 extractMaxNeuronId(const std::string&);    
    void readConnections(const std::string&);

  // Clustering
  protected:
    void* clusters = nullptr;

  public:
    std::pair<UINT64, UINT64> getIrregularMetric(); // Not immplemented
};

};

#endif