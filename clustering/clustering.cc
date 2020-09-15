#include <algorithm>
#include <vector>
#include "clustering.h"
#include "unroll.h"
#define CROSSBAR_SIZE 4

std::vector<int> argsort(std::vector<int> data)
{
    std::vector<int> index(data.size(), 0);
    for (int i = 0; i != index.size(); i++)
    {
        index[i] = i;
    }
    auto comparator = [&data](int a, int b) { return data[a] < data[b]; };
    std::sort(index.begin(), index.end(), comparator);
    return index;
}

std::vector<int> difference(std::vector<int> &v1, std::vector<int> &v2)
{
    std::vector<int> v3;

    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());

    std::set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(), back_inserter(v3));

    return v3;
}

std::vector<int> intersection(std::vector<int> &v1, std::vector<int> &v2)
{
    std::vector<int> v3;

    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());

    std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), back_inserter(v3));

    return v3;
}

std::vector<Cluster> fcfs_clustering(std::vector<Neuron> snn)
{
    std::vector<Cluster> clusters;
    int count = 0;

    Cluster new_cluster = Cluster(count);
    clusters.push_back(new_cluster);

    int i;
    for (i = 0; i < snn.size(); i++)
    {
        Neuron n = snn[i];
        if (n.get_input_list().size() > 0)
        {
            int parent_cluster = -1;
            int cluster_to_merge = -1;
            int l;
            for (l = 0; l < clusters.size(); l++)
            {
                if (isPresent(n.get_parent(), clusters[l].get_parent()) && n.get_parent() > 0)
                {
                    parent_cluster = l;
                    break;
                }
            }
            if (parent_cluster >= 0)
            {
                int comi = intersection(clusters[parent_cluster].get_input(), n.get_input_list()).size();
                int total_inputs_post_merging = clusters[parent_cluster].get_input().size() + n.get_input_list().size() - comi;

                if (total_inputs_post_merging <= CROSSBAR_SIZE)
                {
                    cluster_to_merge = parent_cluster;
                }
            }

            if (cluster_to_merge < 0)
            {
                std::vector<int> new_inputs;
                std::vector<int> common_inputs;
                std::vector<Neuron> mapped_neurons;

                int l;
                for (l = 0; l < clusters.size(); l++)
                {
                    std::vector<int> A = clusters[l].get_input();
                    std::vector<int> B = n.get_input_list();
                    std::vector<int> AB;
                    AB.reserve(A.size() + B.size()); // preallocate memory
                    AB.insert(AB.end(), A.begin(), A.end());
                    AB.insert(AB.end(), B.begin(), B.end());

                    std::unique(AB.begin(), AB.end());
                    std::vector<int> ci = intersection(clusters[l].get_input(), n.get_input_list());
                    common_inputs.push_back(ci.size());
                    new_inputs.push_back(AB.size());
                    mapped_neurons.push_back(clusters[l].get_neurons().size());
                }

                std::reverse(common_inputs.begin(), common_inputs.end());
                std::vector<int> sorted_idxes = argsort(common_inputs);
                int j;
                for (j = 0; j < sorted_idxes.size(); j++)
                {
                    int idx = sorted_idxes[j];
                    if (new_inputs[idx] <= CROSSBAR_SIZE && mapped_neurons[idx] < CROSSBAR_SIZE)
                    {
                        cluster_to_merge = idx;
                        break;
                    }
                }
            }

            if (cluster_to_merge < 0)
            {
                count++;
                Cluster newer_cluster = Cluster(count);
                newer_cluster.AddNeuron(n);
                newer_cluster.add_parent(n.get_id());
                clusters.push_back(newer_cluster);
            }
            else
            {
                std::vector<int> inputs_to_merge = n.get_input_list();
                if (parent_cluster < 0)
                {
                    clusters[cluster_to_merge].AddNeuron(n);
                    clusters[cluster_to_merge].AddInput(n.get_input_list());
                }
                else
                {
                    inputs_to_merge = difference(n.get_input_list(), n.get_parent());
                    clusters[cluster_to_merge].AddInput(inputs_to_merge);
                }
                clusters[cluster_to_merge].AddParent(n.get_id());
            }
        }
    }
    return clusters;
}

std::vector<int> sort_cluster_with_common_elements(std::vector<Cluster> clusters, std::vector<int> elements)
{
    std::vector<int> inter;
    int i;
    for (i = 0; i < clusters.size(); i++)
    {
        std::vector<int> temp = intersection(clusters[i].get_input(), elements);
        inter.insert(temp.size());
    }
    std::reverse(inter.begin(), inter.end());
    return argsort(inter);
}

std::vector<Cluster> bind(std::vector<Node> graph)
{
    std::vector<Cluster> clusters;
    int cluster_count = 0;
    int i;
    for (i = 0; i < graph.size(); i++)
    {
        bool merge = false;
        int merge_idx = -1;

        int node_id = graph[i].get_node();
        std::vector<int> node_edges = graph[i].get_edges();
        std::vector<int> sorted_cluster_list = sort_cluster_with_common_elements(clusters, node_edges);
        int j;
        for (j = 0; j < clusters.size(); j++)
        {
            int cluster_idx = sorted_cluster_list[j];
            std::vector<int> cluster_fanin_after_merge;
            cluster_fanin_after_merge.insert(cluster_fanin_after_merge.end(), clusters[cluster_idx].get_input().begin(), clusters[cluster_idx].get_input().end());
            cluster_fanin_after_merge.insert(cluster_fanin_after_merge.end(), node_edges.begin(), node_edges.end());
            std::unique(cluster_fanin_after_merge.begin(), cluster_fanin_after_merge.end());

            std::vector<int> cluster_fanout_after_merge;
            cluster_fanout_after_merge.insert(cluster_fanout_after_merge.end(), clusters[cluster_idx].get_output().begin(), clusters[cluster_idx].get_output().end());
            cluster_fanout_after_merge.push_back(node_id);

            if (cluster_fanin_after_merge.size() <= CROSSBAR_SIZE && cluster_fanout_after_merge.size() <= CROSSBAR_SIZE)
            {
                merge = true;
                merge_idx = cluster_idx;
            }
        }

        if (merge)
        {
            clusters[merge_idx].AddInput(node_edges);
            std::vector<int> node_id_list;
            node_id_list.push_back(node_id);
            clusters[merge_idx].AddOutput(node_id_list);
        }
        else
        {
            Cluster new_cluster = Cluster(cluster_count);
            new_cluster.AddInput(node_edges);
            std::vector<int> node_id_list;
            node_id_list.push_back(node_id);
            new_cluster.AddOutput(node_id_list);
            clusters.push_back(new_cluster);

            cluster_count++;
        }
    }
    return clusters;
}
// Function to return the next random number
int getNum(vector<int> &v)
{

    // Size of the vector
    int n = v.size();

    // Generate a random number
    srand(time(NULL));

    // Make sure the number is within
    // the index range
    int index = rand() % n;

    // Get random number from the vector
    int num = v[index];

    // Remove the number from the vector
    swap(v[index], v[n - 1]);
    v.pop_back();

    // Return the removed number
    return num;
}

// Function to generate n non-repeating random numbers
std::vector<int> generateRandom(int n)
{
    std::vector<int> v(n);
    std::vector<int> out;

    // Fill the vector with the values
    // 1, 2, 3, ..., n
    for (int i = 0; i < n; i++)
        v[i] = i + 1;

    // While vector has elements
    // get a random number from the vector and print it
    while (v.size())
    {
        out.push_back(getNum(v));
    }
    return out;
}

void fcfs_mapping(std::vector<Cluster> clusters, int node_id, std::vector<int> fanins, int graph_node, &int cluster_idx, &bool merge)
{
    cluster_idx = -1;
    merge = false;
}

{
    int cluster_count = 0;
    std::vector<Cluster> clusters;

    if (cluster_mapping_policy == "RANDOM" || cluster_mapping_policy == "FCFS" || cluster_mapping_policy == "PRIOR")
    {
        std::vector<int> coord = generateRandom(graph.size());
        int i;
        std::vector<int> new_graph(coord.size());
        for (i = 0; i < coord.size(); i++)
        {
            new_graph[i] = graph[i];
        }
        graph.clear();
        graph = new_graph;
        new_graph.clear();
    }

    int i;
    for (i = 0; i < graph.size(); i++)
    {
        int node_id = graph[i].get_node();
        std::vector<int> fanins = graph[i].get_edges();

        bool merge = false;
        int cluster_idx = -1;

        if (clusters.size() > 0)
        {
            if (cluster_mapping_policy == "FCFS")
            {
                fcfs_mapping(clusters, node_id, fanins, graph[i], &cluster_idx, &merge);
            }
        }

        if (merge)
        {
            clusters[cluster_idx].AddNode(graph[i]);
            std::vector<int> node_id_list;
            node_id_list.push_back(node_id);
            clusters[cluster_idx].AddOutput(node_id_list);
            clusters[cluster_idx].AddInput(fanins);
        }
        else
        {
            Cluster new_cluster = Cluster(cluster_count);
            new_cluster.AddNode(graph[i]);
            std::vector<int> node_id_list;
            node_id_list.push_back(node_id);
            new_cluster.AddOutput(node_id_list);
            new_cluster.AddInput(fanins);
            clusters.push_back(new_cluster);
            cluster_count++;
        }
    }

    return clusters;
}