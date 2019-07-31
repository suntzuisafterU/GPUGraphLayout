
#include "layouteval/stress.hpp"
#include "utils/IO/RPGraphLayoutRead.hpp"

#include <iostream>
#include <string>

// include ogdf, where ever and how ever it is installed.

int main(int argc, const char** argv) {
    // Read from the command line:
    //   edgelist file name,
    //   layout1 file name,
    //   layout2 file name.
	if (argc < 4) {
		std::cout << "Usage: stress_exec [edge list path] [first generated layout path] [second generated layout path]" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Accept layout file?
	std::string edge_list = argv[1];
	std::string layout_path1 = argv[2]; // TODO: while still command line args put into vector of strings, calculate stress on each, saves the allPairsShortestPaths step.
    std::string layout_path2 = argv[3];

	if (!is_file_exists(edge_list.c_str())) {
		std::cout << "ERROR: No file at " << edge_list << std::endl;
		exit(EXIT_FAILURE);
	}

	if (!is_file_exists(layout_path1.c_str())) {
	 	std::cout << "ERROR: No file at " << layout_path1 << std::endl;
	 	exit(EXIT_FAILURE);
	}

	if (!is_file_exists(layout_path2.c_str())) {
	 	std::cout << "ERROR: No file at " << layout_path2 << std::endl;
	 	exit(EXIT_FAILURE);
	}

	std::cout << "Reading the edgelist file and initializing graph and layout." << std::endl;
	RPGraph::UGraph graph(edge_list);
	RPGraph::GraphLayout layout1(graph);
	RPGraph::GraphLayout layout2(graph);

	RPGraph::readFromCSV(layout1, layout_path1); // Read the provided layout file.
	RPGraph::readFromCSV(layout2, layout_path2); // Read the provided layout file.

    // Initialize our UGraph
    // Initialize OGDF Graph of some kind

    // Determine number of samples to take.

    // In for loop:
    //   get single source all pairs distances from OGDF graph.
    //   use this to calculate the stress of those specific nodes in each layout,
    //   and add this to the incremented layout values.


    return EXIT_SUCCESS;
}