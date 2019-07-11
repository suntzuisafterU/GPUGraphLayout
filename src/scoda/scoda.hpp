
#ifndef scoda_hpp
#define scoda_hpp

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <unordered_map>
#include <fstream>
#include <sstream> // for istringstream() in scoda.cpp
#include <iostream>
#include <algorithm> // max_element()

#include "../common/RPTypeDefs.hpp"
#include "../common/RPGraph.hpp" // For UGraph
#include "CommunityAlgo.hpp"
#include "DisjointPartitionAlgo.hpp"

namespace RPGraph {
    struct SCoDA_Results;

    class SCoDA : public DisjointPartitionAlgo < SCoDA_Results > {
        // TODO: using namespace RPGraph::contiguous_nid_t etc to shorten everything.
        SCoDA_Results compute_partition(const RPGraph::UGraph& original_graph, RPGraph::UGraph& comm_graph, 
                    std::unordered_map<RPGraph::contiguous_nid_t, RPGraph::comm_id_t>& nid_comm_map);

        int compute_mode_of_degree(const RPGraph::UGraph& in_graph);

        void print_partition(std::unordered_map<RPGraph::contiguous_nid_t, RPGraph::comm_id_t> &nid_comm_map);
    };

    struct SCoDA_Results : DisjointResults {
        int num_null_e;
        int num_duplicate_comm_edges;
        int num_comm_nodes;
        int num_full_nodes;
        float node_comp_ratio;
        int num_comm_edges;
        int num_full_edges;
        float edge_comp_ratio;
    };
    
    /**
     * source: https://stackoverflow.com/a/55961383/11385910
     */
    template <class Container>
    auto findMaxKeyValuePair(Container const &x) 
        -> typename Container::value_type
    {
        using value_t = typename Container::value_type;
        const auto compare = [](value_t const &p1, value_t const &p2)
        {
            return p1.second < p2.second;
        };
        return *std::max_element(x.begin(), x.end(), compare);
    }

} // namespace RPGraph

#endif /* scoda_hpp */
