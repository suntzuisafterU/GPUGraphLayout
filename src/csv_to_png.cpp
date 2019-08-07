#include "common/RPGraphLayout.hpp"
#include "common/RPGraph.hpp"
#include "utils/IO/RPGraphLayoutRead.hpp"
#include "utils/IO/RPGraphLayoutUtils.hpp"

#include <iostream>
#include <string>

int main(int argc, const char** argv) {
	// Get path from command line.
	if (argc < 6) {
		std::cout << "Usage: edgelist_file csv_input_path png_output_path image_width image_height" << std::endl;
		return EXIT_FAILURE;
	}
	std::string edgelist_file{ argv[1] };
	std::string csv_path{ argv[2] };
	std::string out_path{ argv[3] };
	const int image_w{ *argv[4] };
	const int image_h{ *argv[5] };

	// Read layout csv file.
	RPGraph::UGraph graph(edgelist_file);
	RPGraph::GraphLayout layout(graph);
	RPGraph::readFromCSV(layout, csv_path);

	// Produce png with pngwriter.
	RPGraph::writeToPNG(&layout, image_w, image_h, out_path);

	return EXIT_SUCCESS;
}