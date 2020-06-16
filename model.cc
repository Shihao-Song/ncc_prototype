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
void Model::loadArch(std::string &arch_file)
{
    try
    {
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(arch_file, pt);

        // Iterate through the layers
        BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("config.layers"))
        {
            std::string class_name = v.second.get<std::string>("class_name");
            std::string name = v.second.get<std::string>("config.name");

            Layer::Layer_Type layer_type = Layer::Layer_Type::MAX;
            if (class_name == "Conv2D") { layer_type = Layer::Layer_Type::Conv2D; }
            else if (class_name == "Flatten") { layer_type = Layer::Layer_Type::Flatten; }
            else if (class_name == "Dense") { layer_type = Layer::Layer_Type::Dense; }
            else { std::cerr << "Error: Unsupported layer type.\n"; exit(0); }

            arch.addLayer(name, layer_type);
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
