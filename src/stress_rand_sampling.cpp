
#include "layouteval/stress.hpp"
#include "utils/IO/RPGraphLayoutRead.hpp"

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/List.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/basic/graph_generators/deterministic.h>

#include <iostream>
#include <string>
#include <utility>

// include ogdf, where ever and how ever it is installed.

/**
 * IMPORTANT: The correcteness of this implementation is currently entirely dependent on the assumption that the 
 *            UGraphs used to initialize both of the original layouts used the exact same initialization method.
 *            This should be true IFF the function `read_edgelist_file` in the source file `RPGraph.cpp` was the
 *            not modified between invocations.  If there is any doubt just regenerate the data all in one pipeline (bash scripts).
 */
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

	std::cout << "Reading the edgelist file and initializing graph and layouts." << std::endl;
    // Initialize our UGraph
	RPGraph::UGraph graph(edge_list);
	RPGraph::GraphLayout layout1(graph);
	RPGraph::GraphLayout layout2(graph);

	RPGraph::readFromCSV(layout1, layout_path1); // Read the provided layout file.
	RPGraph::readFromCSV(layout2, layout_path2); // Read the provided layout file.

    // Initialize OGDF Graph of some kind
    ogdf::Graph G; // Shouldn't need GraphAttributes object, SPSS does not require it.
    int n = static_cast<int>(graph.num_nodes());
    ogdf::List< std::pair< int,int > > edges;

    // Use our UGraph to initialize the ogdf::Graph
    // IMPORTANT: This is the same for loop structure that has been used many times throughout this
    //            project.  A very handy and simple improvement would be to add an iterator to the 
    //            UGraph class.  I just ran out of time.
    for (RPGraph::contiguous_nid_t _src_id = 0; _src_id < graph.num_nodes(); _src_id++) { // Iterate over source nodes
        for (RPGraph::contiguous_nid_t _dst_id : graph.neighbors_with_geq_id(_src_id)) { // Iterate over adjacency list of each source node. Contains ids of target nodes that are larger.
            // Fill up a NodeArray, and create a List<std::pair< int,int >> edges at the same time.
            int src_id = static_cast<int>(_src_id);
            int dst_id = static_cast<int>(_dst_id);
            edges.emplaceBack(src_id, dst_id);
        }
    }

    // Populate the new ogdf::Graph G
    ogdf::customGraph(G, n, edges);

    // std::cout << "G: " << G << std::endl;

    // Determine number of samples to take.

    // In for loop:
    //   get single source all pairs distances from OGDF graph.
    //   ogdf::bfs_SPSS(node s, const Graph &G, NodeArray< TCost>& distanceArray, TCost edgeCosts)
    //   use this to calculate the stress of those specific nodes in each layout,
    //   and add this to the incremented layout values.
    
    // IMPORTANT: Will have to const_cast back and forth when indexing into layout??


    return EXIT_SUCCESS;
}