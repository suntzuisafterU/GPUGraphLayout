#include "RPGraphLayoutRead.hpp"
#include <sstream>
#include <iostream>
#include <fstream>

namespace RPGraph {
     /**
      * Preconditions: Layout MUST be initialized with a graph that was read from the same edgelist file that was initially used to create the layout file.
      */
     void readFromCSV(RPGraph::GraphLayout& layout, std::string layout_path) {
		std::cout << "Reading the layout file and setting all coordinates within the layout." << std::endl;
		// TODO: Read layout file and update all coordinates in layout.
		std::ifstream layout_stream;
		layout_stream.open(layout_path);
		uint32_t N{ layout.graph.num_nodes() };

		// This is code that reads a custom layout format. Specifically:
		// node_id: uint32_t, x_coord: float, y_coord: float
		// TODO: This would be best put somewhere else, for example in IO utils, if it were being used anywhere else.
		uint32_t temp_counter = 0;
		std::string line;
		while (std::getline(layout_stream, line))
		{
			// Skip any comments
			if (line[0] == '#') continue;
			if (line[0] == '%') continue;

			// Read source and target from file
			RPGraph::dangerous_nid_t external_nid;
            RPGraph::contiguous_nid_t safe_nid;
			float x, y;
			std::istringstream ss(line);
			ss >> external_nid;
            safe_nid = layout.graph.getContigFromExternal(external_nid); // IMPORTANT: Maps through associated graph.
            if(safe_nid >= N) { // Safety check just in case.
                std::cout << "ERROR: safe_nid == " << safe_nid << " and is larger than N = " << N << std::endl;
                exit(EXIT_FAILURE);
            }

			if (ss.peek() == ',') ss.ignore();
			ss >> x;
			if (ss.peek() == ',') ss.ignore();
			ss >> y;

			layout.setCoordinates(safe_nid, RPGraph::Coordinate(x, y));

			temp_counter++;
		}
	}
}