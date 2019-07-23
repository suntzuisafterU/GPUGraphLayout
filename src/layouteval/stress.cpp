#include "stress.hpp"
#include <vector>
#include <iostream>
#include <limits>

namespace RPGraph {

using RPGraph::Coordinate;
// using std::vector; // Doesn't seem to work...

/**
 * Stress formula;
 * Given layout, L
 * initialize float result
 * for all node pairs:
 *      compute distG and distU,
 *      compute k_ij = k/distG**2
 *      result += k_ij * [distU - (L * distG)]**2
 */
RPGraph::StressReport stress(RPGraph::GraphLayout* layout, int L) {
    // Iterate over all nodes in layout.graph
    RPGraph::Graph& g = layout->graph; // If I use a reference here does this get copied?
    
    return StressReport{0.0F}; // TODO: Temp
}

float distU(Coordinate c1, Coordinate c2) {
    // OR: Just use the provided getDistance(con_nidt, con_nidt) method.
    // TODO: Euclidean distance
}

float distG() {
    // TODO: shortest path distance.
}

/**
 * Floyd-Warshall algorithm.
 */
std::vector< std::vector< int > >* allPairsShortestPaths(RPGraph::Graph* graph) {
	typedef std::vector< std::vector< int > > matrix;
	const int n = graph->num_nodes(); // TODO: Is this type conversion safe?
	std::cout << "n := " << n << std::endl;
	// dist := n by n matrix, with all non-diagonal values infinity,
	// and diagonals initialized to zero.
	constexpr int infinity = std::numeric_limits<int>::max(); // constexpr means compile time evaluation.
	std::cout << "infinity := " << infinity << std::endl;
	matrix dist(n, std::vector<int>(n, infinity));

	// for each edge (u,v) in graph: 
	//                       set dist[u][v] = 1 // The weight of the edge.
	//                   and set dist[v][u] = 1 // Since this is an undirected graph.
	// TODO: Make more efficient after testing initial implementation.
	
	for (int i; i < n; i++) {
		dist[i][i] = 0; // Set all self distances to zero.
	}
}

} // RPGraph