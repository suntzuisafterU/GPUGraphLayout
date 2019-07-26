#include "RPGraphLayoutUtils.hpp"

#include <sstream>

namespace RPGraph {
    /**
     * pngwriter entry point.
     */
    void writeToPNG(RPGraph::GraphLayout* layout, const int image_w, const int image_h,
                                      std::string path)
    {
        const float xRange = layout->getXRange();
        const float yRange = layout->getYRange();
        const RPGraph::Coordinate center = layout->getCenter(); // Why do we need the center?
        const float xCenter = center.x; // Probably to set the origin for layout.
        const float yCenter = center.y;
        const float minX = xCenter - xRange/2.0;
        const float minY = yCenter - yRange/2.0;
        const float xScale = image_w/xRange;
        const float yScale = image_h/yRange;

        // Here we need to do some guessing as to what the optimal
        // opacity of nodes and edges might be, given network size.
        /**
         * TODO: Adjust node opacity for best results.
         */
        const float node_opacity = 10000.0  / layout->graph.num_nodes();
        const float edge_opacity = 100000.0 / layout->graph.num_edges();

        // Write to file.
        pngwriter layout_png(image_w, image_h, 0, path.c_str());
        layout_png.invert(); // set bg. to white

        for (contiguous_nid_t n1 = 0; n1 < layout->graph.num_nodes(); ++n1)
        {
            // Plot node,
            layout_png.filledcircle_blend((layout->getX(n1) - minX)*xScale,
                                          (layout->getY(n1) - minY)*yScale,
                                          3, node_opacity, 0, 0, 0);
            for (contiguous_nid_t n2 : layout->graph.neighbors_with_geq_id(n1)) {
                // ... and edge.
                layout_png.line_blend((layout->getX(n1) - minX)*xScale, (layout->getY(n1) - minY)*yScale,
                                      (layout->getX(n2) - minX)*xScale, (layout->getY(n2) - minY)*yScale,
                                      edge_opacity, 0, 0, 0);
            }
        }
        // Write it to disk.
        layout_png.write_png();
    }

     /**
      * Writing to csv may be a good way to streamline testing the decompressions effectiveness.
      */
     void writeToCSV(RPGraph::GraphLayout* layout, std::string path)
     {
         std::ofstream out_file(path);
 
         for (contiguous_nid_t n = 0; n < layout->graph.num_nodes(); ++n)
         {
             dangerous_nid_t id = layout->graph.getExternalFromContig(n); // id as found in edgelist
             out_file << id << "," << layout->getX(n) << "," << layout->getY(n) << "\n";
         }
 
         out_file.close();
     }
 

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

    void writeCommunityGraphToEdgelist(RPGraph::UGraph& graph, SCoDA_Report report, std::string outpath) {
        std::ofstream outfile(outpath);
        outfile << report;
        // Iterate over edges.  Write to edgelist file.
        for (RPGraph::contiguous_nid_t src_id = 0; src_id < graph.num_nodes(); src_id++) // Iterate over source nodes
        {
            for (RPGraph::contiguous_nid_t dst_id : graph.neighbors_with_geq_id(src_id)) // Iterate over adjacency list of each source node. Contains ids of target nodes that are larger.
            {
                outfile << src_id << "\t" << dst_id << std::endl;
            }
        }
    }

//     /**
//      * Do we have any use for writing to bin?
//      */
//     void writeToBin(RPGraph::GraphLayout* layout, std::string path)
//     {
//         if (is_file_exists(path.c_str()))
//         {
//             printf("Error: File exists at %s\n", path.c_str());
//             exit(EXIT_FAILURE);
//         }
// 
//         std::ofstream out_file(path, std::ofstream::binary);
// 
//         for (contiguous_nid_t n = 0; n < layout->graph.num_nodes(); ++n)
//         {
//             contiguous_nid_t id = layout->graph.node_map_r[n]; // id as found in edgelist
//             float x = layout->getX(n);
//             float y = layout->getY(n);
// 
//             out_file.write(reinterpret_cast<const char*>(&id), sizeof(id));
//             out_file.write(reinterpret_cast<const char*>(&x), sizeof(x));
//             out_file.write(reinterpret_cast<const char*>(&y), sizeof(y));
//         }
// 
//         out_file.close();
//     }
} // namespace RPGraph
