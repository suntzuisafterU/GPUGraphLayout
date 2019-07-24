/**
 * File for testing implementation of allPairsShortestPath algorithm(s).
 */

#include <stdlib.h>
#include <iostream>

#include "layouteval/stress.hpp"
#include "common/RPGraph.hpp"

void print_matrix(RPGraph::matrix& mat) {
	int n = mat.size(); /// Assumes square matrix.
	if(n < 30) {
		// Just print the entire thing.
		std::cout << "# Matrix values." << std::endl;
		for(int i = 0; i < n; ++i) {
			std::cout << std::endl; // Newline.
			std::cout << "[ ";
			for(int j = 0; j < n; ++j) {
				std::cout << " " << mat[i][j] << " " << std::endl;
			}
			std::cout << " ]";
		}
		std::cout << std::endl; // Final new line.

	} else {
		// Print the corners, 15 x 15 corners?
		throw "Not implemented.";
	}
};

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

	print_matrix(dist);

	return EXIT_SUCCESS;
}
