#include "stress.hpp"
#include "../common/RPTypeDefs.hpp"
#include <vector>
#include <iostream>
#include <limits>

namespace RPGraph {

using RPGraph::Coordinate;
// using std::vector; // Doesn't seem to work...

RPGraph::StressReport stress(RPGraph::GraphLayout* layout, int L) {
    // Iterate over all nodes in layout.graph
    RPGraph::Graph& g = layout->graph; // If I use a reference here does this get copied?
    
    return StressReport{0.0F}; // TODO: Temp
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

/**
 * Stress formula;
 * Given layout, L
 * initialize float result
 * for all node pairs:
 *      compute distG and distU,
 *      compute k_ij = k/distG**2
 *      result += k_ij * [distU - (L * distG)]**2
 */
StressReport stress(RPGraph::GraphLayout* layout, int L) {
	// Calculate all pairs shortest paths.
	matrix all_pairs_shortests = allPairsShortestPaths(layout->graph); // TODO: Does this get passed correctly as a pointer?
	
	int k = 1; // TODO: TEMP: What is k supposed to be?
	int stress = 0;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (i != j) {
				float dist_g{ i < j ? all_pairs_shortests[i][j] : all_pairs_shortest[j][i] };
				float dist_u{ layout->getDistance(i, j) };
				float k_ij = k / dist_g * *2;
				stress += k_ij * (dist_u - (L * dist_g))**2;
			}
		}
	}

	StressReport results = {
		stress
	};

	return results;
}

} // RPGraph