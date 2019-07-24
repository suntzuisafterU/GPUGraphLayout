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
	}

	if (!is_file_exists(layout_path.c_str())) {
		std::cout << "ERROR: No file at " << layout_path << std::endl;
	}

	RPGraph::UGraph graph(edge_list);
	RPGraph::GraphLayout layout(graph);

	// TODO: Read layout file and update all coordinates in layout.
	std::ifstream layout_stream(layout_path);

	RPGraph::contiguous_nid_t curr_node;
	std::string line;
	while(std::getline(layout_stream, line))
	{
		// Skip any comments
		if(line[0] == '#') continue;
		if(line[0] == '%') continue;

		// Read source and target from file
		float x, y;
		std::istringstream(line) >> x >> y;

		layout.setCoordinates(curr_node, RPGraph::Coordinate(x, y));

		curr_node++;
	}
	layout_stream.close();

	int L = 1; // TODO: TEMP, we will likely want to implement optimizing over L as in the
	           //       provided msc-graphstudy code.
	// TODO: Call stress algo.
	RPGraph::StressReport report = RPGraph::stress(layout, L);

	std::cout << report;

	return EXIT_SUCCESS;
}