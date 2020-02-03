#ifndef DisjointPartitionAlgo_hpp
#define DisjointPartitionAlgo_hpp

#include <unordered_map>

#include "CommunityAlgo.hpp"
#include "../common/RPGraph.hpp"

namespace RPGraph {

    struct DisjointResults;

    template<class T>
    class DisjointPartitionAlgo : public RPGraph::CommunityAlgo {
        virtual T compute_partition(const UGraph& full_graph, UGraph& comm_graph, 
                nid_comm_map_t & nid_comm_map) = 0; 
        // TODO: Should this return a UGraph (the comm graph) or should it a struct with the results and accept an out param?
    };

    struct DisjointResults {
        
    };

}

#endif // DisjointPartitionAlgo_hpp