#ifndef stress_hpp
#define stress_hpp

#include "StressReport.hpp"
#include "../common/RPCommon.hpp" // Coordinate
#include "../common/RPGraphLayout.hpp" // GraphLayout

#include <ogdf/basic/NodeArray.h> // node and NodeArray?

#include <vector>
#include <iostream>

namespace RPGraph {

typedef std::vector< std::vector< uint32_t > > matrix;

StressReport stress(RPGraph::GraphLayout& layout, matrix& all_pairs_shortest, int L);

StressReport stress_single_source(GraphLayout& layout, ogdf::node s, ogdf::NodeArray<int>& spss, int L);

/**
 * O(N**2) memory complexity, O(N**3)?? time complexity. 
 */
matrix allPairsShortestPaths(RPGraph::UGraph& graph);

void print_matrix(RPGraph::matrix& mat, const char* reason);

} // RPGraph

#endif // stress_hpp