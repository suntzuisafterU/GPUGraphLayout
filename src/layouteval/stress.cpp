#include "stress.hpp"
#include "../common/RPTypeDefs.hpp"
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
matrix allPairsShortestPaths(RPGraph::Graph* graph) {
	const uint32_t n{ graph->num_nodes() }; // Uniform initialization, should prevent narrowing or unsafe conversions.
	std::cout << "n := " << n << std::endl;

	// dist := n by n matrix, with all non-diagonal values infinity,
	// and diagonals initialized to zero.
	constexpr uint32_t infinity = std::numeric_limits<uint32_t>::max(); // constexpr means compile time evaluation.
	std::cout << "infinity := " << infinity << std::endl;
	matrix dist(n, std::vector<uint32_t>(n, infinity));
	for (int i = 0; i < n; i++) {
		dist[i][i] = 0; // Set all self distances to zero.
	}

	// for each edge (u,v) in graph: 
	//                       set dist[u][v] = 1 // The weight of the edge.
	//                   and set dist[v][u] = 1 // Since this is an undirected graph.
	// TODO: Make more efficient after testing initial implementation.
	for (contiguous_nid_t src_id = 0; src_id < graph->num_nodes(); src_id++) { // Iterate over source nodes
		for (contiguous_nid_t dst_id : graph->neighbors_with_geq_id(src_id)) { // Iterate over adjacency list of each source node. Contains ids of target nodes that are larger. 
			dist[src_id][dst_id] = 1;
			dist[dst_id][src_id] = 1;
		}
	}
	
	// O(|V|**3), core of Floyd-Warshall.
	for (int k = 0; k < n; k++) {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				if (dist[i][j] > dist[i][k] + dist[k][j]) {
					dist[i][j] = dist[i][k] + dist[k][j];
				}
			}
		}
	}

	return dist; /// Compiler should optimize to move assignment.
}

StressReport stress(RPGraph::GraphLayout* layout, int L) {

}

} // RPGraph