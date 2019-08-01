
#include "layouteval/stress.hpp"
#include "utils/IO/RPGraphLayoutRead.hpp"

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/basic/List.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/basic/graph_generators/deterministic.h>
#include <ogdf/graphalg/ShortestPathAlgorithms.h> // bfs_SPSS();
#include <ogdf/basic/SList.h> // TEMP, DEBUGGING

#include <iostream>
#include <string>
#include <utility>
#include <vector>

typedef ogdf::List< std::pair< int,int > > Edgelist;

// include ogdf, where ever and how ever it is installed.
// void print_ogdf_graph(ogdf::Graph& G) {
//     std::vector< std::pair< int,int > > edges;
//     G.allEdges(edges);
//     for(auto& e : edges) {
//         std::cout << "Edge: " << e.first << ", " << e.second << std::endl;
//     }
// };

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
    ogdf::GraphAttributes GA; // Do we need this???
    // int n = static_cast<int>(graph.num_nodes());
    // Edgelist edges;
    // ogdf::Array< ogdf::node > nodes;

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

    // If we end up having to use file formats, the most promising is the UCINET DL format so far:
    // https://gephi.org/users/supported-graph-formats/ucinet-dl-format/
    // (But what format is the original data in??)

    // Populate the new ogdf::Graph G
    // ogdf::customGraph(G, n, edges, nodes); // Getting incomplete type error.

    // std::cout << "G: " << G << std::endl;
    std::cout << "Finished creating ogdf::Graph??" << std::endl;

    // Determine number of samples to take.

    // In for loop:
    //   randomly sample nodes (can use ogdf::Graph::chooseNode())
    //   get single source all pairs distances from OGDF graph.
    //   ogdf::bfs_SPSS(node s, const Graph &G, NodeArray< TCost>& distanceArray, TCost edgeCosts)
    //   use this to calculate the stress of those specific nodes in each layout,
    //   and add this to the incremented layout values.

    // Try to do the SSPS here...
    // signature: 
    // 
    // template<typename TCost> /* For us TCost is int, and always is edgeCosts=1 */
    // void bfs_SPSS(node s, const Graph& G, NodeArray<TCost> & distanceArray, TCost edgeCosts);
    //
    // documentation: https://ogdf.github.io/doc/ogdf-snapshot/group__ga-sp.html#ga900d113a4194675ba88c6aec52df0c27
    // source code: https://ogdf.github.io/doc/ogdf-snapshot/_shortest_path_algorithms_8h_source.html#l00065

    // NodeArray docs: https://ogdf.github.io/doc/ogdf-snapshot/classogdf_1_1_node_array.html
    ogdf::NodeArray<int> distanceArray(G, 0);
    int edgeCosts = 1;
    ogdf::node s = G.chooseNode();
    // Lets see where the function fails...
    ogdf::NodeArray<bool> mark(G, false);
    ogdf::SListPure<ogdf::node> bfs;
    bfs.pushBack(s);
    // mark s and set distance to itself 0
    mark[s] = true;
    distanceArray[s] = int(0); // seg fault here.  We need to initialize the node array.
    while (!bfs.empty())
    {
        ogdf::node w = bfs.popFrontRet();
        int d = distanceArray[w] + edgeCosts;
        for (ogdf::adjEntry adj : w->adjEntries)
        {
            ogdf::node v = adj->twinNode();
            if (!mark[v])
            {
                mark[v] = true;
                bfs.pushBack(v);
                distanceArray[v] = d;
            }
        }
    }

    ogdf::bfs_SPSS<int> (s, G, distanceArray, edgeCosts);

    std::cout << "Finished, look at the distanceArray: " << std::endl;
    
    // IMPORTANT: Will have to const_cast back and forth when indexing into layout??


    return EXIT_SUCCESS;
}