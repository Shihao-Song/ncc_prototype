#include "model.h"

// boost library to parse json architecture file
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include <sstream>

namespace NCC
{
namespace NCC_FrontEnd
{
void Model::Architecture::connector()
{
    for (int i = 0; i < layers.size() - 1; i++)
    {
        if (layers[i + 1].layer_type == Layer::Layer_Type::Conv2D)
        {
            connToConv(i, i + 1);
        }
        if (layers[i + 1].layer_type == Layer::Layer_Type::MaxPooling2D)
        {
            connToPool(i, i + 1);
        }
    }
}

void Model::Architecture::connToConv(unsigned cur_layer_id, unsigned next_layer_id)
{
    auto &cur_neurons_dims = layers[cur_layer_id].output_dims;
    auto &cur_neurons_ids = layers[cur_layer_id].output_neuron_ids;

    auto &conv_kernel_dims = layers[next_layer_id].w_dims;
    auto &conv_kernel_weights = layers[next_layer_id].weights;
    auto &conv_strides = layers[next_layer_id].strides;
    auto &conv_output_dims = layers[next_layer_id].output_dims;
    auto &conv_output_neuron_ids = layers[next_layer_id].output_neuron_ids;

    uint64_t conv_neuron_id_track = cur_neurons_ids[cur_neurons_ids.size() - 1] + 1;
    // std::cout << conv_neuron_id_track << "\n";

    unsigned conv_output_dims_x = 0;
    unsigned conv_output_dims_y = 0;
    // For each filter
    for (int filter = 0; filter < conv_kernel_dims[3]; filter++)
    {
        conv_output_dims_x = 0;
        for (int row = conv_kernel_dims[0] - 1; row < cur_neurons_dims[0]; row += conv_strides[1])
        {
            conv_output_dims_x++;

            conv_output_dims_y = 0;
            for (int col = conv_kernel_dims[1] - 1; col < cur_neurons_dims[1]; col += conv_strides[0])
            {
                conv_output_dims_y++;

                // All neurons inside the current kernel
                int starting_row = row + 1 - conv_kernel_dims[0];
                int ending_row = row;
                int starting_col = col + 1 - conv_kernel_dims[1];
                int ending_col = col;

		// std::cout << starting_row << " " << ending_row << " " << starting_col << " " << ending_col << "\n";
                for (int k = 0; k < cur_neurons_dims[2]; k++)
                {
                    for (int i = starting_row; i <= ending_row; i++)
                    {
                        for (int j = starting_col; j <= ending_col; j++)
                        {
                            uint64_t cur_neuron_id = cur_neurons_ids[k * cur_neurons_dims[0] * cur_neurons_dims[1] +
                                                                     i * cur_neurons_dims[1] + j];

                            float weight = 
                            conv_kernel_weights[(i - starting_row) *  conv_kernel_dims[1] * conv_kernel_dims[2] * conv_kernel_dims[3]
                                                + (j - starting_col) * conv_kernel_dims[2] * conv_kernel_dims[3]
                                                + k * conv_kernel_dims[3] + filter];

                            // Record the connection information
                            if (auto iter = connections.find(cur_neuron_id);
                                     iter != connections.end())
                            {
                                (*iter).second.out_neurons_ids.push_back(conv_neuron_id_track);
                                (*iter).second.weights.push_back(weight);
                            }
                            else
                            {
                                connections.insert({cur_neuron_id, {conv_neuron_id_track, weight}});
                            }

                            std::cout << cur_neuron_id << " ";
                        }
                    }
                }
                std::cout << "-> " << conv_neuron_id_track << "\n";
                conv_output_neuron_ids.push_back(conv_neuron_id_track);
                conv_neuron_id_track++;
                // std::cout << "\n";
            }
        }
    }
    std::cout << "\n";
    conv_output_dims.push_back(conv_output_dims_x);
    conv_output_dims.push_back(conv_output_dims_y);
    conv_output_dims.push_back(conv_kernel_dims[3]);
}

void Model::Architecture::connToPool(unsigned cur_layer_id, unsigned next_layer_id)
{
    auto &cur_neurons_dims = layers[cur_layer_id].output_dims;
    auto &cur_neurons_ids = layers[cur_layer_id].output_neuron_ids;

    auto &pool_kernel_dims = layers[next_layer_id].w_dims;
    auto &pool_strides = layers[next_layer_id].strides;
    auto &pool_output_dims = layers[next_layer_id].output_dims;
    auto &pool_output_neuron_ids = layers[next_layer_id].output_neuron_ids;

    uint64_t pool_neuron_id_track = cur_neurons_ids[cur_neurons_ids.size() - 1] + 1;
    // std::cout << conv_neuron_id_track << "\n";

    unsigned pool_output_dims_x = 0;
    unsigned pool_output_dims_y = 0;

    pool_kernel_dims.push_back(cur_neurons_dims[2]); // Number of filters equals to that of the prev. layer.

    // For each filter
    for (int filter = 0; filter < pool_kernel_dims[3]; filter++)
    {
        pool_output_dims_x = 0;
        for (int row = pool_kernel_dims[0] - 1; row < cur_neurons_dims[0]; row += pool_strides[1])
        {
            pool_output_dims_x++;

            pool_output_dims_y = 0;
            for (int col = pool_kernel_dims[1] - 1; col < cur_neurons_dims[1]; col += pool_strides[0])
            {
                pool_output_dims_y++;

                // All neurons inside the current kernel
                int starting_row = row + 1 - pool_kernel_dims[0];
                int ending_row = row;
                int starting_col = col + 1 - pool_kernel_dims[1];
                int ending_col = col;

                for (int i = starting_row; i <= ending_row; i++)
                {
                    for (int j = starting_col; j <= ending_col; j++)
                    {
                        uint64_t cur_neuron_id = cur_neurons_ids[filter * cur_neurons_dims[0] * cur_neurons_dims[1] +
                                                                 i * cur_neurons_dims[1] + j];

                        // Record the connection information
                        if (auto iter = connections.find(cur_neuron_id);
                                 iter != connections.end())
                        {
                            (*iter).second.out_neurons_ids.push_back(pool_neuron_id_track);
                            (*iter).second.weights.push_back(-1);
                        }
                        else
                        {
                            connections.insert({cur_neuron_id, {pool_neuron_id_track, -1}});
                        }

                        std::cout << cur_neuron_id << " ";
                    }
                }
                std::cout << "-> " << pool_neuron_id_track << "\n";
                pool_output_neuron_ids.push_back(pool_neuron_id_track);
                pool_neuron_id_track++;
                // std::cout << "\n";
            }
        }
    }

    pool_output_dims.push_back(pool_output_dims_x);
    pool_output_dims.push_back(pool_output_dims_y);
    pool_output_dims.push_back(pool_kernel_dims[3]);
}

void Model::Architecture::printConns()
{
    for (int i = 0; i < layers.size() - 1; i++)
    {
        auto &output_neurons = layers[i].output_neuron_ids;

        for (auto neuron : output_neurons)
        {
            std::cout << "Input neuron id: " << neuron << "\n";
            std::cout << "Output neuron id: ";

            auto iter = connections.find(neuron);
            assert(iter != connections.end());
            auto &out_neurons_ids = (*iter).second.out_neurons_ids;
            auto &weights = (*iter).second.weights;

            for (auto out_id : out_neurons_ids)
            {
                std::cout << out_id << " ";
            }
            std::cout << "\nWeights: ";
            for (auto weight : weights)
            {
                std::cout << weight << " ";
            }
            std::cout << "\n\n";
        }
    }
}

void Model::loadArch(std::string &arch_file)
{
    try
    {
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(arch_file, pt);

        unsigned layer_counter = 0;
        // Iterate through the layers
        BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("config.layers"))
        {
            // We need to construct the input layer first
            if (layer_counter == 0)
            {
                std::vector<std::string> input_shape;
                std::vector<unsigned> output_dims;
                for (boost::property_tree::ptree::value_type &cell : v.second.get_child("config.batch_input_shape"))
                {
                    input_shape.push_back(cell.second.get_value<std::string>());
                }

                input_shape.erase(input_shape.begin());
                for (auto dim : input_shape) { output_dims.push_back(stoll(dim)); }

                std::string name = "input";
                Layer::Layer_Type layer_type = Layer::Layer_Type::Input;
                arch.addLayer(name, layer_type);
                arch.getLayer(name).setOutputDim(output_dims);

                auto &out_neuro_ids = arch.getLayer(name).output_neuron_ids;
                for (int k = 0; k < output_dims[2]; k++)
                {
                    for (int i = 0; i < output_dims[0]; i++)
                    {
                        for (int j = 0; j < output_dims[1]; j++)
                        {
                            out_neuro_ids.push_back(k * output_dims[0] * output_dims[1] + 
                                                    i * output_dims[1] + j);
                        }
                    }
                }

                layer_counter++;
            }

            std::string class_name = v.second.get<std::string>("class_name");
            std::string name = v.second.get<std::string>("config.name");

            Layer::Layer_Type layer_type = Layer::Layer_Type::MAX;
            if (class_name == "Conv2D") { layer_type = Layer::Layer_Type::Conv2D; }
            else if (class_name == "MaxPooling2D") { layer_type = Layer::Layer_Type::MaxPooling2D; }
            else if (class_name == "Flatten") { layer_type = Layer::Layer_Type::Flatten; }
            else if (class_name == "Dense") { layer_type = Layer::Layer_Type::Dense; }
            else { std::cerr << "Error: Unsupported layer type.\n"; exit(0); }

            arch.addLayer(name, layer_type);

            if (class_name == "Conv2D" || class_name == "MaxPooling2D")
            {
                std::vector<std::string> strides_str;
                std::vector<unsigned> strides;
                for (boost::property_tree::ptree::value_type &cell : v.second.get_child("config.strides"))
                {
                    strides_str.push_back(cell.second.get_value<std::string>());
                }
  
                for (auto stride : strides_str) { strides.push_back(stoll(stride)); }
                arch.getLayer(name).setStrides(strides);
            }

            if (class_name == "MaxPooling2D")
            {
                // We need pool_size since Conv2D's kernel size can be extracted from h5 file
                std::vector<std::string> pool_size_str;
                auto &pool_size = arch.getLayer(name).w_dims;
                for (boost::property_tree::ptree::value_type &cell : v.second.get_child("config.pool_size"))
                {
                    pool_size_str.push_back(cell.second.get_value<std::string>());
                }

                for (auto size : pool_size_str) { pool_size.push_back(stoll(size)); }
                pool_size.push_back(1); // depth is 1
            }

            layer_counter++;
        }
    }
    catch (std::exception const& e)
    {
        std::cerr << e.what() << std::endl;
        exit(0);
    }
}

void Model::loadWeights(std::string &weight_file)
{
    // Example on parsing H5 format
    hid_t file;
    hid_t gid; // group id
    herr_t status;

    // char model_path[MAX_NAME];

    // Open h5 model
    file = H5Fopen(weight_file.c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
    gid = H5Gopen(file, "/", H5P_DEFAULT); // open root
    scanGroup(gid);

    status = H5Fclose(file);
}

void Model::scanGroup(hid_t gid)
{
    ssize_t len;
    hsize_t nobj;
    herr_t err;
    int otype;
    hid_t grpid, dsid;
    char group_name[MAX_NAME];
    char memb_name[MAX_NAME];
    char ds_name[MAX_NAME];

    // Get number of objects in the group
    len = H5Iget_name(gid, group_name, MAX_NAME);
    err = H5Gget_num_objs(gid, &nobj);

    // Iterate over every object in the group
    for (int i = 0; i < nobj; i++)
    {
        // Get object type
        len = H5Gget_objname_by_idx(gid, (hsize_t)i, memb_name, (size_t)MAX_NAME);
        otype = H5Gget_objtype_by_idx(gid, (size_t)i);

        switch (otype)
        {
            // If it's a group, recurse over it
        case H5G_GROUP:
            grpid = H5Gopen(gid, memb_name, H5P_DEFAULT);
            scanGroup(grpid);
            H5Gclose(grpid);
            break;
            // If it's a dataset, that means group has a bias and kernel dataset
        case H5G_DATASET:
            dsid = H5Dopen(gid, memb_name, H5P_DEFAULT);
            H5Iget_name(dsid, ds_name, MAX_NAME);
            extrWeights(dsid);
            break;
        default:
            break;
        }
    }
}

void Model::extrWeights(hid_t id)
{
    hid_t datatype_id, space_id;
    herr_t status;
    hsize_t size;
    char ds_name[MAX_NAME];

    H5Iget_name(id, ds_name, MAX_NAME);
    space_id = H5Dget_space(id);
    datatype_id = H5Dget_type(id);

    // Get dataset dimensions to create buffer of same size
    const int ndims = H5Sget_simple_extent_ndims(space_id);
    hsize_t dims[ndims];
    H5Sget_simple_extent_dims(space_id, dims, NULL);

    // Calculating total 1D size
    unsigned data_size = 1;
    for (int i = 0; i < ndims; i++) { data_size *= dims[i]; }

    float rdata[data_size];
    status = H5Dread(id, datatype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);
    
    // Add information to the corres. layer
    std::stringstream full_name(ds_name);
    std::vector <std::string> tokens;
    std::string intermediate;

    while(getline(full_name, intermediate, '/'))
    {
        tokens.push_back(intermediate);
    }

    // The secondary last element indicates the layer name
    // TODO, I'm not sure if this is always true. Need to do more research
    Layer &layer = arch.getLayer(tokens[tokens.size() - 2]);
    std::vector<unsigned> dims_vec(dims, dims + ndims);
    std::vector<float> rdata_vec(rdata, rdata + data_size);

    if (tokens[tokens.size() - 1].find("kernel") != std::string::npos)
    {
        layer.setWeights(dims_vec, rdata_vec);
    }
    else if (tokens[tokens.size() - 1].find("bias") != std::string::npos)
    {
        layer.setBiases(dims_vec, rdata_vec);
    }
}
}
}
