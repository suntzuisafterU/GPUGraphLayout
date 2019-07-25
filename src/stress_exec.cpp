#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "layouteval/stress.hpp"
#include "common/RPGraph.hpp"
#include "common/RPGraphLayout.hpp"


int main(int argc, const char** argv) {

	if (argc != 3) {
		std::cout << "Usage: stress_exec [edge list path] [generated layout path]" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Accept layout file?
	std::string edge_list = argv[1];
	std::string layout_path = argv[2];

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

	std::cout << "Reading the layout file and setting all coordinates within the layout." << std::endl;
	// TODO: Read layout file and update all coordinates in layout.
	std::ifstream layout_stream(layout_path);

	// This is code that reads a custom layout format. Specifically:
	// node_id: uint32_t, x_coord: float, y_coord: float
	// TODO: This would be best put somewhere else, for example in IO utils, if it were being used anywhere else.
	uint32_t temp_counter = 0;
	std::string line;
	while(std::getline(layout_stream, line))
	{
		// Skip any comments
		if(line[0] == '#') continue;
		if(line[0] == '%') continue;

		// Read source and target from file
		RPGraph::contiguous_nid_t curr_node;
		float x, y;
		std::istringstream ss(line);
		ss >> curr_node;
		if (ss.peek() == ',') ss.ignore();
		ss >> x;
		if (ss.peek() == ',') ss.ignore();
		ss >> y;

		layout.setCoordinates(curr_node, RPGraph::Coordinate(x, y));

		temp_counter++;
	}
	layout_stream.close();

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
}