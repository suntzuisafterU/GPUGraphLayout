
#include "layouteval/stress.hpp"
#include "utils/IO/RPGraphLayoutRead.hpp"

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/basic/List.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/basic/graph_generators/deterministic.h>
#include <ogdf/graphalg/ShortestPathAlgorithms.h> // bfs_SPSS();

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <unordered_set>

/**
 * IMPORTANT: The correcteness of this implementation is currently entirely dependent on the assumption that the 
 *            UGraphs used to initialize both of the original layouts used the exact same initialization method.
 *            This should be true IFF the function `read_edgelist_file` in the source file `RPGraph.cpp` was the
 *            not modified between invocations.  If there is any doubt just regenerate the data all in one pipeline (bash scripts).
 */
int main(int argc, const char** argv) {

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////// Commandline args and validation /////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
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

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////// Initialize our UGraph and GraphLayout structures /////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // Initialize our UGraph
	RPGraph::UGraph graph(edge_list);
	RPGraph::GraphLayout layout1(graph);
	RPGraph::GraphLayout layout2(graph);

	RPGraph::readFromCSV(layout1, layout_path1); // Read the provided layout file.
	RPGraph::readFromCSV(layout2, layout_path2); // Read the provided layout file.
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////// Initialize ogdf::Graph using our UGraph /////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    ogdf::Graph G;

    // Use our UGraph to initialize the ogdf::Graph
    // IMPORTANT: This is the same for loop structure that has been used many times throughout this
    //            project.  A very handy and simple improvement would be to add an iterator to the 
    //            UGraph class.  I just ran out of time.
    std::unordered_map<  int, ogdf::node > membership_map;
    membership_map.reserve(graph.num_nodes());
    std::cout << "membership_map.size() : " << membership_map.size() << std::endl;
    for (RPGraph::contiguous_nid_t _src_id = 0; _src_id < graph.num_nodes(); _src_id++) { // Iterate over source nodes
        for (RPGraph::contiguous_nid_t _dst_id : graph.neighbors_with_geq_id(_src_id)) { // Iterate over adjacency list of each source node. Contains ids of target nodes that are larger.
            /* Example of initializing a graph: http://www.ogdf.net/doku.php/tech:howto:manual */
            int src_id = static_cast<int>(_src_id);
            int dst_id = static_cast<int>(_dst_id);
            ogdf::node n1 = membership_map.count(src_id) > 0 ? membership_map.at(src_id) : G.newNode(src_id);
            ogdf::node n2 = membership_map.count(dst_id) > 0 ? membership_map.at(dst_id) : G.newNode(dst_id);
            membership_map.insert({src_id, n1}); // insert will not reassign, will silently do nothing if index is already occupied.  Should always be trying to assign the same value to the specific index.
            membership_map.insert({dst_id, n2});
            // edges.emplaceBack(src_id, dst_id);
            auto e = G.newEdge(n1, n2);
            std::cout << "Added edge: " << e << std::endl;
        }
    }
    std::cout << "membership_map.size() : " << membership_map.size() << std::endl;
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////// Random sampling, SPSS, and stress ////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // TODO: Determine number of samples to take.

    // SPSS source code: https://ogdf.github.io/doc/ogdf-snapshot/_shortest_path_algorithms_8h_source.html#l00065
    // NodeArray docs: https://ogdf.github.io/doc/ogdf-snapshot/classogdf_1_1_node_array.html

    // In for loop:
    //   randomly sample nodes (can use ogdf::Graph::chooseNode())
    //   get single source all pairs distances from OGDF graph.
    //   ogdf::bfs_SPSS(node s, const Graph &G, NodeArray< TCost>& distanceArray, TCost edgeCosts)
    //   use this to calculate the stress of those specific nodes in each layout,
    //   and add this to the incremented layout values.

    std::unordered_set<ogdf::node> sampled_nodes;
    int num_samples = 6; // TODO: Parameterize or infer from data.

    while(sampled_nodes.size() < num_samples) {
        ogdf::node s = G.chooseNode();
        std::cout << "Chose node: " << s << std::endl;
        if(sampled_nodes.count(s) > 0) {
            std::cout << "Resampled node!!" << std::endl;
            continue;
        }
        sampled_nodes.insert(s);

        ogdf::NodeArray<int> distanceArray(G, 0); /* Array of all nodes in G, each associated with initial value 0. */
        int edgeCosts = 1;

        ogdf::bfs_SPSS<int>(s, G, distanceArray, edgeCosts);

        // std::cout << "Size of distance Array: " << distanceArray.<< std::endl;

        int temp = 0;
        // Try iterating over the NodeArray
        for (auto &dist_g : distanceArray)
        {
            // TODO: IMPORTANT: Verify the identities of all the nodes in the NodeArray. Do they come in the order we expect? What about infinities?
            std::cout << "Index: " << temp++ << " node is distance " << dist_g << " from source node " << s << std::endl;
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////

    return EXIT_SUCCESS;
}