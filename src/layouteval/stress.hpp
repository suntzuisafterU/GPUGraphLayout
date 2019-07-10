#ifndef stress_hpp
#define stress_hpp

#include "../common/RPCommon.hpp" // Coordinate
#include "../common/RPGraphLayout.hpp" // GraphLayout

namespace LayoutEval {

float stress(RPGraph::GraphLayout* layout, int L);

inline float distU(Coordinate c1, Coordinate c2);

inline float distG();

/**
 * O(N**2) memory complexity, O(N**3)?? time complexity. 
 * Maybe just do Dijkstras
 */
vector< vector< float > >* allPairsShortestPaths(RPGraph::Graph* graph) {
    // TODO: Basically Dijkstras algo on each node individually. OR: floyd-warshall algo. Probably better.
}

} // LayoutEval

#endif // stress_hpp