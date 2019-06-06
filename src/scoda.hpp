
#ifndef scoda_hpp
#define scoda_hpp
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <unordered_map>

#include "RPGraph.hpp" // For UGraph and nid_t

namespace CommunityAlgos {
    /**
     *
     */
    int scoda(uint32_t degree_threshold, RPGraph::UGraph& full_graph, RPGraph::UGraph& comm_graph, 
                std::unordered_map<RPGraph::nid_t, RPGraph::nid_t>& nid_comm_vec);

}

#endif /* scoda_hpp */