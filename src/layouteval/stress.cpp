#include "stress.hpp"

namespace RPGraph {

using RPGraph::Coordinate;
using std::vector;

/**
 * Stress formula;
 * Given layout, L
 * initialize float result
 * for all node pairs:
 *      compute distG and distU,
 *      compute k_ij = k/distG**2
 *      result += k_ij * [distU - (L * distG)]**2
 */
float stress(RPGraph::GraphLayout* layout, int L) {
    // Iterate over all nodes in layout.graph
    RPGraph::Graph& g = layout->graph; // If I use a reference here does this get copied?
    
    return 0.0F; // TODO: Temp
}

float distU(Coordinate c1, Coordinate c2) {
    // OR: Just use the provided getDistance(con_nidt, con_nidt) method.
    // TODO: Euclidean distance
}

float distG() {
    // TODO: shortest path distance.
}

/**
 * O(N**2) memory complexity, O(N**3)?? time complexity. 
 * Maybe just do Dijkstras
 */
vector< vector< float > >* allPairsShortestPaths(RPGraph::Graph* graph) {
    // TODO: Basically Dijkstras algo on each node individually. OR: floyd-warshall algo. Probably better.
}

} // RPGraph