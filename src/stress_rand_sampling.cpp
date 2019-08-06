
#include "layouteval/stress.hpp"
#include "common/RPTypeDefs.hpp"
#include "utils/IO/RPGraphLayoutRead.hpp"

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/Graph_d.h>
#include <ogdf/graphalg/ShortestPathAlgorithms.h> // bfs_SPSS();

#include <chrono>
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

    // TODO: Move to something like `utils/ogdf_conversions` as a new file and a procedure. Would we need to keep the membership_map as a global variable?
    // Use our UGraph to initialize the ogdf::Graph
    // IMPORTANT: This is the same for loop structure that has been used many times throughout this
    //            project.  A very handy and simple improvement would be to add an iterator to the 
    //            UGraph class.  I just ran out of time.
    std::cout << "Populating ogdf::Graph." << std::endl;
    std::unordered_map<  int, ogdf::node > membership_map;
    membership_map.reserve(graph.num_nodes());
    for (RPGraph::contiguous_nid_t _src_id = 0; _src_id < graph.num_nodes(); _src_id++) { // Iterate over source nodes
        for (RPGraph::contiguous_nid_t _dst_id : graph.neighbors_with_geq_id(_src_id)) { // Iterate over adjacency list of each source node. Contains ids of target nodes that are larger.
            /* Example of initializing a graph: http://www.ogdf.net/doku.php/tech:howto:manual */
            int src_id = static_cast<int>(_src_id);
            int dst_id = static_cast<int>(_dst_id);
            ogdf::node n1 = membership_map.count(src_id) > 0 ? membership_map.at(src_id) : G.newNode(src_id);
            ogdf::node n2 = membership_map.count(dst_id) > 0 ? membership_map.at(dst_id) : G.newNode(dst_id);
            membership_map.insert({src_id, n1}); // insert will not reassign, will silently do nothing if index is already occupied.  Should always be trying to assign the same value to the specific index.
            membership_map.insert({dst_id, n2});
            auto e = G.newEdge(n1, n2); // Adds edge even if we don't use the reference.
            // std::cout << "Added edge: " << e << std::endl;
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////// Random sampling, SPSS, and stress ////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // TODO: Determine number of samples to take.

    // SPSS source code: https://ogdf.github.io/doc/ogdf-snapshot/_shortest_path_algorithms_8h_source.html#l00065
    // NodeArray docs: https://ogdf.github.io/doc/ogdf-snapshot/classogdf_1_1_node_array.html
    // node docs: http://www.ogdf.net/doc-ogdf/classogdf_1_1_node_element.html

    // In for loop:
    //   randomly sample nodes (can use ogdf::Graph::chooseNode())
    //   get single source all pairs distances from OGDF graph.
    //   ogdf::bfs_SPSS(node s, const Graph &G, NodeArray< TCost>& distanceArray, TCost edgeCosts)
    //   use this to calculate the stress of those specific nodes in each layout,
    //   and add this to the incremented layout values.

    std::unordered_set<ogdf::node> sampled_nodes;
    int num_samples = 400; // TODO: Parameterize or infer from data.
    int progress_print_period = 10;
    RPGraph::StressReport sr_layout1{0, layout1.graph.num_nodes(), 0, layout1.graph.num_edges(), 0, num_samples};
    RPGraph::StressReport sr_layout2{0, layout2.graph.num_nodes(), 0, layout2.graph.num_edges(), 0, num_samples};
    std::cout << "Starting stress calculation." << std::endl;
    while(sampled_nodes.size() < static_cast<uint>(num_samples)) {
        //////////////////////
        // TEMP: TIMING
        auto start = std::chrono::steady_clock::now();
        //////////////////////
        int num_sampled = sampled_nodes.size();
        if(num_sampled % progress_print_period == 0) std::cout << 100.0F * num_sampled/num_samples << "%" << std::endl;
        // TODO: Should we seed the RNG?  From the documentation we don't even know if they use rand()...
        ogdf::node s = G.chooseNode();
        // std::cout << "Chose node: " << s << std::endl;
        if(sampled_nodes.count(s) > 0) {
            // std::cout << "Resampled node!!" << std::endl;
            continue;
        }
        sampled_nodes.insert(s);

        ogdf::NodeArray<int> distanceArray(G, 0); /* Array of all nodes in G, each associated with initial value 0. */
        int edgeCosts = 1;

        // std::cout << "Starting shortest paths single source." << std::endl;
        ogdf::bfs_SPSS<int>(s, G, distanceArray, edgeCosts); 
        /* 
         * Fills the distanceArray.  We can index this with nodes I think.  
         */

        std::vector< std::pair< RPGraph::contiguous_nid_t, int > > distance_vec;

        #define forall_nodes(v,G) for((v)=(G).firstNode(); (v); (v)=(v)->succ()) // defined in the documentation but not in the latest version on master. Source: http://www.ogdf.net/doc-ogdf/_graph__d_8h_source.html#l00335
        ogdf::node n;
        forall_nodes(n, G) { /* MACRO, defined in Graph_d.h */
            int index = n->index();
            distance_vec.emplace_back(static_cast<RPGraph::contiguous_nid_t>(index), distanceArray[n]);
        }

        // TEMP: Display the values saved in distance_vec.
        // for (auto& pair : distance_vec) { std::cout << "Node id: " << pair.first << ", is distance: " << pair.second << " from source node: " << s->index() << std::endl; }


        // std::cout << "Calculating stress on first layout." << std::endl;
        int L = 1; /* TODO: Parameter tweaking.  See msc-graphstudy. For example a value of 100 will have a huge effect. */
        RPGraph::StressReport sr_1_temp = RPGraph::stress_single_source(
                                    layout1, 
                                    static_cast<RPGraph::contiguous_nid_t>(s->index()), 
                                    distance_vec, 
                                    L);

        // std::cout << "Calculation stress on second layout." << std::endl;
        RPGraph::StressReport sr_2_temp = RPGraph::stress_single_source(
                                    layout2, 
                                    static_cast<RPGraph::contiguous_nid_t>(s->index()), 
                                    distance_vec, 
                                    L);

        sr_layout1.stress += sr_1_temp.stress;
        sr_layout2.stress += sr_2_temp.stress;

        // TEMP: TIMING
        if(sampled_nodes.size() <= 1) {
            auto end = std::chrono::steady_clock::now();
            auto duration = end - start; // std::chrono::duration_cast<std::chrono::seconds>(end - start);
            auto dur_seconds = duration.count() / 10e9;
            std::cout << "durtion of one sample: " << dur_seconds << std::endl;
            std::cout << "It will take approximately " << dur_seconds * num_samples << " seconds to finish this computation." << std::endl;
        }
    }

	/* Calculate stress_per_node as an estimate based on the number of samples taken. */
	sr_layout1.stress_per_node = sr_layout1.stress / num_samples;
	sr_layout2.stress_per_node = sr_layout2.stress / num_samples;

    std::cout << "##########################################################################" << std::endl;

    // Compare the layouts:
    std::cout << layout1 << std::endl;
    std::cout << layout2 << std::endl;

    std::cout << "Stress of layout1: \n" << sr_layout1 << std::endl;
    std::cout << "Stress of layout2: \n" << sr_layout2 << std::endl;

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    std::cout << "Finished" << std::endl;

    return EXIT_SUCCESS;
}