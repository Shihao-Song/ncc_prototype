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

    std::vector<UINT64> spike_times;

  public:
    Neuron() {}
    Neuron(UINT64 _id) : neuron_id(_id) {}
    Neuron(const Neuron &_copy) : neuron_id(_copy.neuron_id),
                                  input_neurons(_copy.input_neurons),
                                  output_neurons(_copy.output_neurons),
                                  spike_times(_copy.spike_times) {}

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

    void addSpikeTimeList(std::vector<UINT64> &_list)
    {
        spike_times = _list;
    }

    void setNeuronId(UINT64 _id) { neuron_id = _id; }
    int getNeuronId() { return neuron_id; };

    std::vector<UINT64> &getInputNeuronList() { return input_neurons; };
    std::vector<UINT64> &getOutputNeuronList() { return output_neurons; };

    std::vector<UINT64> &getSpikeTimes() { return spike_times; }
    unsigned numOfSpikes() { return spike_times.size(); }

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

    unsigned max_fanin;

  public:
    Model(const std::string&, const std::string&);

    void setFanin(unsigned _fanin) { max_fanin = _fanin; }
    void unroll();

    void output(const std::string&);

    void debugOutput(const std::string &out_name)
    {
        std::fstream file;
        file.open(out_name, std::fstream::out);

        for (auto &neuron : usnn)
        {
            file << "Neuron ID: " << neuron.getNeuronId() << "\n";

            auto &input_neurons = neuron.getInputNeuronList();
            auto &output_neurons = neuron.getOutputNeuronList();

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
    void readSpikes(const std::string&);
};

class Argument
{
  protected:
    std::string connection_file = "N/A";
    std::string spike_file = "N/A";
    std::string unrolled_output = "N/A";
    std::string debug_output = "N/A";

    unsigned fanin = 0;

  public:
    Argument(int argc, char **argv);

    auto getFanin() { return fanin; }
    auto &getConnFile() { return connection_file; }
    auto &getSpikeFile() { return spike_file; }
    auto &getOutputFile() { return unrolled_output; }
    auto &getDebugOutputFile() { return debug_output; }
};

}

#endif
