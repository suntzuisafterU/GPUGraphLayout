/**
 * File for testing implementation of allPairsShortestPath algorithm(s).
 */

#include <stdlib.h>
#include <iostream>

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

	std::cout << dist << std::endl; // TODO: Try this, see if it prints nice.

	return EXIT_SUCCESS;
}