#include <vector>
#include <utility>
#include <iostream>
#include <limits>
#include <cmath>
#include <string>
#include <iomanip>

#include "stress.hpp"
#include "../common/RPTypeDefs.hpp"

namespace RPGraph {

void print_matrix(RPGraph::matrix& mat, const char* reason) {
	int n = mat.size(); /// Assumes square matrix.
	constexpr uint32_t infinity = std::numeric_limits<uint32_t>::max(); // constexpr means compile time evaluation.

	if(n < 30) {
		// Just print the entire thing.
		std::cout << "\n#############################################################################" << std::endl;
		std::cout << "# Matrix values. Reason for printing: " << reason << std::endl;
		for(int i = 0; i < n; ++i) {
			std::cout << std::endl; // Newline.
			std::cout << "[ ";
			for(int j = 0; j < n; ++j) {
				if(mat[i][j] == infinity) {
					std::cout << " " << "+inf" << " ";
				} else {
					std::cout << " " << std::setw(4) << mat[i][j] << " ";
				}
			}
			std::cout << " ]";
		}
		std::cout << "\n#############################################################################" << std::endl;
		std::cout << std::endl; // Final new line.

	} else {
		// Print the corners, 15 x 15 corners?
		throw "Not implemented.";
	}
};


/**
 * Floyd-Warshall algorithm.
 */
matrix allPairsShortestPaths(RPGraph::UGraph& graph) {
	const uint32_t n{ graph.num_nodes() }; // Uniform initialization, should prevent narrowing or unsafe conversions.
	std::cout << "n := " << n << std::endl;

	// dist := n by n matrix, with all non-diagonal values infinity,
	// and diagonals initialized to zero.
	constexpr uint32_t infinity = std::numeric_limits<uint32_t>::max(); // constexpr means compile time evaluation.
	std::cout << "infinity := " << infinity << std::endl;
	matrix dist(n, std::vector<uint32_t>(n, infinity));
	for (uint32_t i = 0; i < n; i++) {
		dist[i][i] = 0; // Set all self distances to zero.
	}
	
	// print_matrix(dist, "Initial matrix setup.  Should be max values everywhere, and zeroes on the diagonal."); // Good here.

	// for each edge (u,v) in graph: 
	//                       set dist[u][v] = 1 // The weight of the edge.
	//                   and set dist[v][u] = 1 // Since this is an undirected graph.
	// TODO: Make more efficient after testing initial implementation.
	for (contiguous_nid_t src_id = 0; src_id < graph.num_nodes(); src_id++) { // Iterate over source nodes
		for (contiguous_nid_t dst_id : graph.neighbors_with_geq_id(src_id)) { // Iterate over adjacency list of each source node. Contains ids of target nodes that are larger. 
			dist[src_id][dst_id] = 1; /// NOTE: IF edge weights were to be interpreted here, they would actually mean that 2 nodes were MORE tightly connected.  As such some form of inverse relation would have to be defined.  The simplest would be setting edge weights for the purpose of allPairsShortestPaths calculation to be 1/(weight that is used for force layout).  I am not sure if this is an actual legitimate inverse calculation.  Realistically any inverse should be based on the layout algorithm used to have any real meaning.
			dist[dst_id][src_id] = 1;
		}
	}

	// print_matrix(dist, "Matrix of initial graph.  Sould be all max values, and a few 1's. Matrix should be semetric.");
	
	// O(|V|**3), core of Floyd-Warshall.
	for (uint32_t k = 0; k < n; k++) {
		for (uint32_t i = 0; i < n; i++) {
			for (uint32_t j = 0; j < n; j++) {
				if (dist[i][k] != infinity && dist[k][j] != infinity && /// IMPORTANT: Required since 'infinity' as an unsigned int is a value that is +1 from wrapping back around to 0...
					dist[i][j] > dist[i][k] + dist[k][j]) {
					dist[i][j] = dist[i][k] + dist[k][j];
				}
			}
		}
		// print_matrix(dist, "Matrix after one of the intermediate nodes has been considered.");
	}

	// print_matrix(dist, "Final all pairs shortest paths matrix.");

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
StressReport stress(GraphLayout& layout, matrix& all_pairs_shortest, int L) {

	uint32_t n{ layout.graph.num_nodes() };
	// Calculate all pairs shortest paths.
	
	int k = 1; // TODO: TEMP: What is k supposed to be?
	double stress = 0;
	for (uint32_t i = 0; i < n; i++) {
		for (uint32_t j = 0; j < n; j++) {
			if (i != j) {
				/// Warning: narrowing conversion from unsigned int to double here.  Why would this be a narrowing conversion? Answer: https://stackoverflow.com/a/11521166/11385910
				/// Even with a double it would technically be narrowing.  We should never get close to values that would be narrowing though.
				uint32_t dist_g{ i < j ? all_pairs_shortest[i][j] : all_pairs_shortest[j][i] }; /// Result from Floyd-Warshal algorithm.
				double dist_u{ layout.getDistance(i, j) }; /// Euclidean distance in the layout.
				double k_ij{ k / std::pow(dist_g, 2) }; /// Value defined in original paper.
				double current_stress{ k_ij * std::pow(dist_u - (L * dist_g), 2) };
				stress += current_stress;
			}
		}
	}

	uint32_t num_nodes{ layout.graph.num_nodes() };
	double stress_per_node{ stress / num_nodes };
	uint32_t num_edges{ layout.graph.num_edges() };
	double stress_per_edge{ stress / num_edges };

	StressReport results = {
		stress,
		num_nodes,
		stress_per_node,
		num_edges,
		stress_per_edge,
		0
	};

	return results;
}

/**
 * OGDF github installation instructions: https://github.com/ogdf/ogdf/blob/master/doc/build.md
 */
StressReport stress_single_source(GraphLayout& layout, 
								  contiguous_nid_t src_id, 
								  std::vector< std::pair<contiguous_nid_t, int> >& spss_vec, 
								  int L) {

	int k = 1; // TODO: TEMP: What is k supposed to be?
	double stress = 0;

	for(auto& node_dg_pair : spss_vec) {
		// Vector is easier, must check for infinity distance or whatever OGDF returns here, in case of multiple components.
		contiguous_nid_t dst_id{ node_dg_pair.first };
		int dist_g{ node_dg_pair.second };
		///////////////////////////////////////
		if (dist_g == 0) { continue; } // IMPORTANT: 0 means no path.  Don't divide by zero or you'll get a -nan.
		///////////////////////////////////////
		double dist_u{ layout.getDistance(src_id, dst_id) }; /// Euclidean distance in the layout. Should we normalize?
		double k_ij{ k / std::pow(dist_g, 2) }; /// Value defined in original paper.
		double current_stress{ k_ij * std::pow(dist_u - (L * dist_g), 2) };
		stress += current_stress;
		// std::cout << "current stress: " << current_stress << ", dist_g: " << dist_g << ", dist_u: " << dist_u << ", k_ij: " << k_ij << ", TOTAL stress: " << stress << std::endl;
	}

	uint32_t num_nodes{ layout.graph.num_nodes() }; /* TODO: Should be changed to spss_vec.size() if we do some form of duplicate node check. */
	double stress_per_node{ stress/num_nodes };
	uint32_t num_edges{ 0 }; /* Not sure if we can figure out how many edges were actually involved at this point, or now that I think of it, if that is even a good way to summarize stress. */
	double stress_per_edge{ 0/* stress/num_edges */ }; /* Meaningless right now. */

 	StressReport results = {
 		stress,
 		num_nodes,
 		stress_per_node,
 		num_edges,
 		stress_per_edge,
		1 // num_samples
 	};

	return results;
}

} // RPGraph