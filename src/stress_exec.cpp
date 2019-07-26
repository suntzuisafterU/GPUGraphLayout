#include <iostream>
#include <string>

#include "layouteval/stress.hpp"
#include "utils/IO/RPGraphLayoutUtils.hpp" // readFromCSV
#include "common/RPGraph.hpp"
#include "common/RPGraphLayout.hpp"
#include "common/RPCommon.hpp" // contiguous_nid_t


int main(int argc, const char** argv) {

	if (argc < 3) {
		std::cout << "Usage: stress_exec [edge list path] [generated layout path]" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Accept layout file?
	std::string edge_list = argv[1];
	std::string layout_path = argv[2]; // TODO: while still command line args put into vector of strings, calculate stress on each, saves the allPairsShortestPaths step.

	if (!is_file_exists(edge_list.c_str())) {
		std::cout << "ERROR: No file at " << edge_list << std::endl;
		exit(EXIT_FAILURE);
	}

	if (!is_file_exists(layout_path.c_str())) {
	 	std::cout << "ERROR: No file at " << layout_path << std::endl;
	 	exit(EXIT_FAILURE);
	}

	std::cout << "Reading the edgelist file and initializing graph and layout." << std::endl;
	RPGraph::UGraph graph(edge_list);
	RPGraph::GraphLayout layout(graph);

	RPGraph::readFromCSV(layout, layout_path); // Read the provided layout file.

	std::cout << "Calculating all pairs shortest paths." << std::endl;
	// Calculate all pairs shortest paths.  O(|V|**3)
	RPGraph::matrix all_pairs = RPGraph::allPairsShortestPaths(graph);
	int L = 1; // TODO: TEMP, we will likely want to implement optimizing over L as in the
			   //       provided msc-graphstudy code.

	std::cout << "Calculating the stress, and generating report." << std::endl;
	RPGraph::StressReport report = RPGraph::stress(layout, all_pairs, L);

	std::cout << report;

	// When the layout goes out of scope, the destructor is trying to free the coordinates array.
	return EXIT_SUCCESS;
};
