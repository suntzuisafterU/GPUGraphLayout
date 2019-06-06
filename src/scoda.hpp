
#ifndef scoda_hpp
#define scoda_hpp
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "RPGraph.hpp" // For UGraph and nid_t

namespace CommunityAlgos {
    /**
     *
     */
    void scoda(int32_t degree_threshold, RPGraph::UGraph& full_graph, RPGraph::UGraph& comm_graph, 
                std::vector<nid_t>& nid_comm_vec);

}

#endif /* scoda_hpp */