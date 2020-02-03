#ifndef stress_hpp
#define stress_hpp

#include "StressReport.hpp"
#include "../common/RPCommon.hpp" // Coordinate
#include "../common/RPTypeDefs.hpp" // contiguous_nid_t
#include "../common/RPGraphLayout.hpp" // GraphLayout

// #include <ogdf/basic/NodeArray.h> // node and NodeArray?

#include <vector>
#include <utility>
#include <iostream>

namespace RPGraph {

typedef std::vector< std::vector< uint32_t > > matrix;

StressReport stress(RPGraph::GraphLayout& layout, matrix& all_pairs_shortest, int L);

StressReport stress_single_source(GraphLayout& layout, contiguous_nid_t s, std::vector< std::pair<contiguous_nid_t, int> >& spss_vec, int L);

/**
 * O(N**2) memory complexity, O(N**3)?? time complexity. 
 */
matrix allPairsShortestPaths(RPGraph::UGraph& graph);

void print_matrix(RPGraph::matrix& mat, const char* reason);

} // RPGraph

#endif // stress_hpp