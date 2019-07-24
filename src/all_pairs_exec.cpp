/**
 * File for testing implementation of allPairsShortestPath algorithm(s).
 */

#include <stdlib.h>
#include <iostream>
#include <string>

#include "layouteval/stress.hpp"
#include "common/RPGraph.hpp"

int main(int argc, const char** argv) {
	// Accept edge list file and verify name.
	std::string edge_list = argv[1];

	if (!is_file_exists(edge_list.c_str())) {
		std::cout << "ERROR: No file at " << edge_list << std::endl;
		exit(EXIT_FAILURE);
	}

	// Read file into graph.
	RPGraph::UGraph graph(edge_list);

	// Calculate all pair shortest path matrix.
	RPGraph::matrix dist = RPGraph::allPairsShortestPaths(graph);

	RPGraph::print_matrix(dist, "Output at end of all_pairs_exec.  Should be complete distance matrix.");

	return EXIT_SUCCESS;
}
