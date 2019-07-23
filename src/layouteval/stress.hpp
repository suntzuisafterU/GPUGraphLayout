#ifndef stress_hpp
#define stress_hpp

#include <vector>
#include "../common/RPCommon.hpp" // Coordinate
#include "../common/RPGraphLayout.hpp" // GraphLayout

namespace RPGraph {

struct StressReport {
    float stress;
    // TODO: Should this have some way to reference the graph? Parameters that were used, etc.
};

StressReport stress(RPGraph::GraphLayout* layout, int L);

inline float distU(Coordinate c1, Coordinate c2);

inline float distG();

/**
 * O(N**2) memory complexity, O(N**3)?? time complexity. 
 */
std::vector< std::vector< int > >* allPairsShortestPaths(RPGraph::Graph* graph);

} // RPGraph

#endif // stress_hpp