#include <algorithm>
#include <boost/tokenizer.hpp>
#include <cmath>
#include "model.hh"
#include <sys/time.h>
#include <sys/resource.h>


namespace DI
{

Model::Model(const std::string& connection_file_name)
{
    UINT64 max_neuron_id = extractMaxNeuronId(connection_file_name);
    std::cout << "Max neuron ID: " << max_neuron_id << "\n";

    // Initialize neurons with IDs 
    for (auto i = 0; i <= max_neuron_id; i++)
    {
        snn.emplace_back(i);
        snn_neuron_ids.insert(i);
    }

    std::set<UINT64> tempNeuronId = {};
    readConnections(connection_file_name);
    labelNeuronWithDepth(0, tempNeuronId);

    typedef boost::tokenizer<boost::char_separator<char>> tok_t;
    boost::char_separator<char> sep(".", "", boost::keep_empty_tokens);
    tok_t tok(connection_file_name, sep);
    std::cout << *(tok.begin()) << ",";

    UINT64 total_neurons = 0;
    UINT64 total_in = 0;
    UINT64 total_out = 0;
    for (auto &neu : snn)
    {
        total_neurons += 1;
        total_in += neu.numInputNeurons();
        total_out += neu.numOutputNeurons();
    }

    std::pair<UINT64, UINT64> irr_metric = getIrregularMetric();
    UINT64 metric = std::get<0>(irr_metric);
    UINT64 num_connections = std::get<1>(irr_metric);
    std::cout << total_neurons << ","
              << total_in << ","
              << total_out << ","
              << metric << ", " << num_connections << ", " << (float)metric/(float)num_connections << std::endl;
}

Model::~Model()
{
}

UINT64 Model::extractMaxNeuronId(const std::string &file_name)
{
    std::fstream file;
    file.open(file_name, std::ios::in);

    UINT64 max_id = 0;
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
            UINT64 nid = std::stoull(*i);
            if (nid > max_id)
            {
                max_id = nid;
            }
        }
    }
    file.close();
    return max_id;
}

void Model::readConnections(const std::string &connection_file_name)
{
    std::fstream file;
    file.open(connection_file_name, std::ios::in);

    std::string line;
    typedef boost::tokenizer<boost::char_separator<char>> tok_t;
    boost::char_separator<char> sep(" ", "", boost::keep_empty_tokens);

    while (std::getline(file, line))
    {
        UINT64 source_neuron_id;
        tok_t tok(line, sep);
        std::set<UINT64> out_neuron_id_list;
        // std::set<Neuron> out_neuron_list;
        bool first = true;
        for (tok_t::iterator i = tok.begin(); i != tok.end(); ++i)
        {
            if (*i == "")
            {
                continue;
            }
            if (first)
            {
                source_neuron_id = std::stoull(*i);
                first = false;
                continue;
            }
            UINT64 out_neuron_id = std::stoull(*i);
            // Neuron out_neuron = Neuron(out_neuron_id);
            out_neuron_id_list.insert(out_neuron_id);
            // out_neuron_list.insert(out_neuron);
        }

	    for (auto out : out_neuron_id_list)
        {
            snn[source_neuron_id].addOutputNeuronID(out);
            snn[out].addInputNeuronID(source_neuron_id);
        }
    }
    file.close();
}

void Model::labelNeuronWithDepth(UINT64 starting_depth, std::set<UINT64>& indepth) {    
    std::set<UINT64> outdepth = {};
    if (starting_depth == 0) { //indepth should be snn 
        for (auto& neuron : snn) 
        {
            if (neuron.getInputNeuronIDList().size() == 0)
            {
                neuron.setDepth(1);
                outdepth.insert(neuron.getNeuronId());
            }   
        }
        labelNeuronWithDepth(1, outdepth);
    } else if (indepth.size()) { // If not last depth   
        for (auto& neuron_id : indepth)
        {
            for (auto& outneu_id: snn[neuron_id].getOutputNeuronIDList()) 
            {
                snn[outneu_id].setDepth(std::max((int)starting_depth + 1, snn[outneu_id].getDepth()));
                if (snn[outneu_id].getDepth() == starting_depth + 1) 
                    outdepth.insert(outneu_id);
            }
        }
        labelNeuronWithDepth(starting_depth + 1, outdepth);
    } else {
        return;
    }
}

// void Model::labelNeuronWithDepth(UINT64 starting_depth, std::vector<UINT64>& indepth) {    
//     if (starting_depth == 0) { //indepth should be snn 
//         indepth.clear();
//         for (auto& neuron : snn) 
//         {
//             if (neuron.getInputNeuronIDList().size() == 0)
//             {
//                 neuron.setDepth(1);
//                 indepth.push_back(neuron.getNeuronId());
//             }   
//         }
//         labelNeuronWithDepth(1, indepth);
//     } else if (indepth.size()) { // If not last depth 
//         std::vector<UINT64> tempVec;  
//         for (auto& neuron_id : indepth)
//         {
//             for (auto& outneu_id: snn[neuron_id].getOutputNeuronIDList()) 
//             {
//                 snn[outneu_id].setDepth(std::max((int)starting_depth + 1, snn[outneu_id].getDepth()));
//                 if (snn[outneu_id].getDepth() == starting_depth + 1) 
//                     tempVec.push_back(outneu_id);
//             }
//         }
//         indepth = tempVec;
//         labelNeuronWithDepth(starting_depth + 1, indepth);
//     } else {
//         return;
//     }
// }

void Model::outputNeuronDepthIR(const std::string& out_file) {
    if (snn.size() == 0) { return; }

    std::fstream file;
    file.open(out_file, std::fstream::out);

    for (auto &neuron : snn)
    {
        assert(neuron.getInputNeuronIDList().size() <= max_fanin);
        file << neuron.getNeuronId() << " ";
        file << neuron.getDepth();
        file << "\n";
    }

    file.close();
    return;
}

void Model::printNeuronConnDepth(const std::string& out_file) {
    if (snn.size() == 0) { return; }

    std::fstream file;
    file.open(out_file, std::fstream::out);

    for (auto &neuron : snn)
    {
        assert(neuron.getInputNeuronIDList().size() <= max_fanin);
        file << neuron.getNeuronId() << "\t";
        file << neuron.getDepth() << "\t" << "(";
        if (neuron.getInputNeuronIDList().size()) {
            for (auto &inneu : neuron.getInputNeuronIDList())
            {
                file << snn[inneu].getDepth() << ",";
            }
        } else {
            file << "None";
        }

        file << ")" << "\t" << "(";

        if (neuron.getOutputNeuronIDList().size()) {
            for (auto &outneu : neuron.getOutputNeuronIDList())
            {
                file << snn[outneu].getDepth() << ",";
            }
        } else {
            file << "None";            
        }

        file << ")" << "\n";
    }

    file.close();
    return;
}
std::pair<UINT64, UINT64> Model::getIrregularMetric() {
    //Define irregular metric as (\sum_{i=0} n {\sum_{j=0} k_i {d_i - d_j}}) / num_connection
    //for snn that has n neurons, neuron i has ki input connections
    UINT64 metric = 0;
    UINT64 num_connections = 0;
    for (auto& neuron : snn) {
        for (auto& inneu_id : neuron.getInputNeuronIDList()) {
            metric += (neuron.getDepth() - snn[inneu_id].getDepth() - 1);
            num_connections++;
        }
    }
    std::pair<UINT64, UINT64> returnVal = std::make_pair(metric, num_connections);
    return returnVal;
}

}

