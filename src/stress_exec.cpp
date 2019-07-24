#include <iostream>
#include <string>

#include "layouteval/stress.hpp"
#include "common/RPGraph.hpp"
#include "common/RPGraphLayout.hpp"


int main(int argc, const char** argv) {
	// Accept layout file?
	std::string edge_list = argv[1];
	std::string layout_file = argv[2];

	RPGraph::UGraph graph(edge_list);
	RPGraph::GraphLayout layout(graph);

	// TODO: Read layout file and update all coordinates in layout.
	std::fstream layout_stream(layout_file, std::ifstream::in);

	uint32_t curr_node;
	std::string line;
	while(std::getline(layout_file, line))
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
	layout_file.close();

	return EXIT_SUCCESS;
}