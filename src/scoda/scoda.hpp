
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

#include "../common/RPGraph.hpp" // For UGraph and nid_t

namespace CommunityAlgos {
    /**
     *
     */
    int scoda_G(uint32_t degree_threshold, std::fstream& edgelist_file,
                RPGraph::UGraph& full_graph, RPGraph::UGraph& comm_graph, 
                std::unordered_map<RPGraph::nid_t, RPGraph::nid_t>& nid_comm_vec);

    int scoda_G(uint32_t degree_threshold,
                RPGraph::UGraph& full_graph, RPGraph::UGraph& comm_graph, 
                std::unordered_map<RPGraph::nid_t, RPGraph::nid_t>& nid_comm_vec);

    int scoda_partition(uint32_t degree_threshold, std::fstream &edgelist_file);
    
    void print_partition(std::unordered_map<RPGraph::nid_t, RPGraph::nid_t> &nid_comm_map);
}

#endif /* scoda_hpp */
