#ifndef __MODEL_H__
#define __MODEL_H__

#include <iostream>
#include <string>
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
        enum class Layer_Type : int
        {
            Conv2D, // convolution layer
            Flatten, // flatten layer
            Dense, // dense (fully-connected) layer
            MAX
        }layer_type = Layer_Type::MAX;

        Layer() {}
        Layer(std::string &_name, Layer_Type &_type) : name(_name), layer_type(_type) {}

        std::string& getName() { return name; }
        Layer_Type& getLayerType() { return layer_type; }

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
        
        std::string name; // Name of the layer
	std::vector<unsigned> w_dims; // dims of the weights
        std::vector<float> weights; // all the weight
        std::vector<unsigned> b_dims; // dims of the bias
        std::vector<float> bias; // all the bias
    };

    // Model - Architecture
    class Architecture
    {
      protected:
        std::vector<Layer> layers;

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
                if (layer.getName() == name) { return layer; }
            }
            std::cerr << "Error: layer is not found.\n";
            exit(0);
        }

        void printLayers()
        {
            for (auto &layer : layers)
            {
                auto name = layer.getName();
                auto type = layer.getLayerType();

                std::cout << "Layer name: " << name << "; ";
                if (type == Layer::Layer_Type::Conv2D) { std::cout << "Layer type: Conv2D\n"; }
                else if(type == Layer::Layer_Type::Flatten) { std::cout << "Layer type: Flatten\n"; }
                else if(type == Layer::Layer_Type::Dense) { std::cout << "Layer type: Dense\n"; }

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
                std::cout << "\n";
                exit(0);
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
