#ifndef __MODEL_H__
#define __MODEL_H__

#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>      // std::pair
#include <vector>

#include "hdf5.h"

namespace NCC
{
namespace NCC_FrontEnd
{
#define MAX_NAME 1024
class Model
{
  protected:
    class Layer
    {
      public:
        // TODO, more layer type:
        // https://towardsdatascience.com/a-comprehensive-introduction-to-different-types-of-convolutions-in-deep-learning-669281e58215
	// https://medium.com/@zurister/depth-wise-convolution-and-depth-wise-separable-convolution-37346565d4ec
	// https://machinelearningmastery.com/introduction-to-1x1-convolutions-to-reduce-the-complexity-of-convolutional-neural-networks/
        enum class Layer_Type : int
        {
            Input, // the input layer
            Conv2D, // convolution layer
            MaxPooling2D, // max polling layer
            AveragePooling2D,
            Flatten, // flatten layer
            Dense, // dense (fully-connected) layer
            MAX
        }layer_type = Layer_Type::MAX;

        Layer() {}
        Layer(std::string &_name, Layer_Type &_type) : name(_name), layer_type(_type) {}

        // std::string& getName() { return name; }
        // Layer_Type& getLayerType() { return layer_type; }

        void setWeights(std::vector<unsigned> &_w_dims,
                        std::vector<float> &_weights)
        {
            w_dims = _w_dims;
            weights = _weights;
        }
        void setBiases(std::vector<unsigned> &_b_dims,
                       std::vector<float> &_bias)
        {
            b_dims = _b_dims;
            bias = _bias;
        }
        void setStrides(std::vector<unsigned> &_strides)
        {
            strides = _strides;
        }
        void setOutputDim(std::vector<unsigned> &_dims)
        {
            output_dims = _dims;
        }

        std::string name; // Name of the layer
	std::vector<unsigned> w_dims; // dims of the weights
        std::vector<float> weights; // all the weight
        std::vector<unsigned> b_dims; // dims of the bias
        std::vector<float> bias; // all the bias

        std::vector<unsigned> strides;

        std::vector<unsigned> output_dims; // dimension of output
        std::vector<uint64_t> output_neuron_ids;
    };

    // Model - Architecture
    class Architecture
    {
      protected:
        std::vector<Layer> layers;

      protected:
        struct ConnEntry
        {
            ConnEntry(uint64_t _id, float _w)
            {
                out_neurons_ids.push_back(_id);
                weights.push_back(_w);
            }

            std::vector<uint64_t> out_neurons_ids;
            std::vector<float> weights;
        };
        std::unordered_map<uint64_t, ConnEntry> connections;

        void connToConv(unsigned, unsigned);
        void connToPool(unsigned, unsigned);
        void connToFlat(unsigned, unsigned);
        void connToDense(unsigned, unsigned);

      public:
        Architecture() {}

        void addLayer(std::string &_name, Layer::Layer_Type &_type)
        {
            layers.emplace_back(_name, _type);
        }

        Layer& getLayer(std::string &name)
        {
            for (auto &layer : layers)
            {
                if (layer.name == name) { return layer; }
            }
            std::cerr << "Error: layer is not found.\n";
            exit(0);
        }

        void connector();

        void printConns();

        void printLayers() // Only used for small network debuggings.
        {
            for (auto &layer : layers)
            {
                auto name = layer.name;
                auto type = layer.layer_type;

                std::cout << "Layer name: " << name << "; ";
                if (type == Layer::Layer_Type::Conv2D) { std::cout << "Layer type: Conv2D"; }
                else if(type == Layer::Layer_Type::MaxPooling2D) { std::cout << "Layer type: MaxPooling2D"; }
                else if(type == Layer::Layer_Type::AveragePooling2D) { std::cout << "Layer type: AveragePooling2D"; }
                else if(type == Layer::Layer_Type::Flatten) { std::cout << "Layer type: Flatten"; }
                else if(type == Layer::Layer_Type::Dense) { std::cout << "Layer type: Dense"; }
                std::cout << "\n";
/*
                std::cout << "Dimension: ";
                auto &w_dims = layer.w_dims;
                auto &weights = layer.weights;

                for (auto dim : w_dims) { std::cout << dim << " "; }
                std::cout << "\n";

                unsigned i = 0;
                for (auto weight : weights)
                {
                    std::cout << weight << " ";
                    if ((i + 1) % w_dims[w_dims.size() - 1] == 0)
                    {
                        std::cout << "\n";
                    }
                    i++;
                }

                auto &strides = layer.strides;
                std::cout << "Strides: ";
                for (auto stride : strides) { std::cout << stride << " "; }
                std::cout << "\n";
*/
                auto &output_dims = layer.output_dims;
                std::cout << "Output shape: ";
                for (auto dim : output_dims) { std::cout << dim << " "; }
                std::cout << "\n\n";
/*
                auto &out_neuro_ids = layer.output_neuron_ids;
                std::cout << "Output neuron id: ";
                std::cout << "\n";
                for (int k = 0; k < output_dims[2]; k++)
                {
                    for (int i = 0; i < output_dims[0]; i++)
                    {
                        for (int j = 0; j < output_dims[1]; j++)
                        {
                            std::cout << out_neuro_ids[k * output_dims[0] * output_dims[1] + 
                                                       i * output_dims[1] + j] << " ";
                        }
                        std::cout << "\n";
                    }
                    std::cout << "\n";
                }
		
                std::cout << "\n";
                // exit(0);
*/
            }
        }
    };

    Architecture arch;

  public:
    Model(std::string &arch_file, std::string &weight_file)
    {
        loadArch(arch_file);
        loadWeights(weight_file);
    }

    void printLayers() { arch.printLayers(); }

    void connector() { arch.connector(); } 

    void printConns() { arch.printConns(); }

  protected:
    void loadArch(std::string &arch_file);
    void loadWeights(std::string &weight_file);

  protected:
    void scanGroup(hid_t);
    void extrWeights(hid_t);
};
}
}

#endif
