#include <iostream>
#include <string>

#include "layouteval/stress.hpp"
#include "common/RPGraph.hpp"
#include "common/RPGraphLayout.hpp"


int main(int argc, const char** argv) {
	// Accept layout file?
	std::string edge_list = argv[1];
	std::string layout_path = argv[2];

	RPGraph::UGraph graph = new RPGraph::UGraph(edge_list);
	RPGraph::GraphLayout layout(graph);

	// TODO: Read layout file and update all coordinates in layout.
	std::fstream layout_stream(layout_path, std::ifstream::in);

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

	// TODO: Call stress algo.
	

	delete graph;

	return EXIT_SUCCESS;
}