
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

#include "RPGraph.hpp" // For UGraph and nid_t

namespace CommunityAlgos {
    /**
     *
     */
    int scoda(uint32_t degree_threshold, std::fstream& edgelist_file,
                RPGraph::UGraph& full_graph, RPGraph::UGraph& comm_graph, 
                std::unordered_map<RPGraph::nid_t, RPGraph::nid_t>& nid_comm_vec);

    int scoda(uint32_t degree_threshold,
                RPGraph::UGraph& full_graph, RPGraph::UGraph& comm_graph, 
                std::unordered_map<RPGraph::nid_t, RPGraph::nid_t>& nid_comm_vec);
    
    std::vector< std::vector<RPGraph::nid_t> >& 
    get_partition(std::unordered_map<RPGraph::nid_t, RPGraph::nid_t> &nid_comm_map);

    void print_partition(std::vector< std::vector<RPGraph::nid_t> >& partition);
}

#endif /* scoda_hpp */
