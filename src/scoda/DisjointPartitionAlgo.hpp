#ifndef DisjointPartitionAlgo_hpp
#define DisjointPartitionAlgo_hpp

#include <unordered_map>

#include "CommunityAlgo.hpp"
#include "../common/RPGraph.hpp"

namespace RPGraph {

    struct DisjointResults;

    class DisjointPartitionAlgo : public RPGraph::CommunityAlgo {
        // TODO: Do we need the scope specifier RPGraph:: ???
        virtual DisjointResults compute_partition(UGraph& full_graph, UGraph& comm_graph, 
                std::unordered_map<contiguous_nid_t, comm_id_t> & nid_comm_map) = 0; 
        // TODO: Should this return a UGraph (the comm graph) or should it a struct with the results and accept an out param?
    };

    struct DisjointResults {
        
    };

}

#endif // DisjointPartitionAlgo_hpp