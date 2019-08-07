#include "common/RPGraphLayout.hpp"
#include "utils/IO/RPGraphLayoutRead.hpp"
#include "utils/IO/RPGraphLayoutUtils.hpp"

#include <string>

int main(int argc, const char** argv) {
	// Get path from command line.
	std::string csv_path{ argv[1] };
	std::string out_path{ argv[2] };
	const int image_w{ argv[3] };
	const int image_h{ argv[4] };

	// Read layout csv file.
	RPGraph::GraphLayout layout;
	RPGraph::readFromCSV(layout, csv_path);

	// Produce png with pngwriter.
	RPGraph::writeToPNG(&layout, image_w, image_h, out_path);

	return EXIT_SUCCESS;
}