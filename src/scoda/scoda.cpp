#include "scoda.hpp"

namespace RPGraph
{

// #define DEGREE(id) (original_graph.degree(id))              /// Defines function for accessing the degree of the ith node.
#define INSERT_COMMUNITY(nid, comm_id) (nid_comm_map.insert({nid, comm_id})) /// Defines function to update community association of node nid.
#define COMMUNITY_OF(nid) (nid_comm_map.at(nid))                             /// Defines function for accessing the community id associated with the ith node.

SCoDA_Results compute_partition(const RPGraph::UGraph& original_graph, RPGraph::UGraph& comm_graph, 
            std::unordered_map<RPGraph::contiguous_nid_t, RPGraph::comm_id_t>& nid_comm_map) {
    // TODO: Compute mode of degree??
    compute_mode_of_degree(original_graph);
    /* Memory allocation & initialisation */
    nid_comm_map.reserve(original_graph.num_nodes()); // TODO: Testing

    int num_null_e = 0; // Just for counting the number of FULLY ignored edges.
    int num_duplicate_comm_edges = 0;

    printf("\nStarting scoda.\n");
    /* Main SCoDA loop */

    RPGraph::nid_t src_id, dst_id;
    uint32_t src_deg, dst_deg;
    std::vector<uint32_t> degrees; /// Required since SCoDA must track the degree of edges as they are streamed.

    if (original_graph.num_nodes() == 0 || original_graph.num_edges() == 0) return -1; // ERROR, empty UGraph.
    // TODO: Move explicit manipulation of UGraph to internal function or iterator class.
    for (RPGraph::contiguous_nid_t src_id = 0; src_id < original_graph.num_nodes(); ++src_id) // Iterate over source nodes
    {
        for (RPGraph::contiguous_nid_t dst_id : original_graph.neighbors_with_geq_id(src_id)) // Iterate over adjacency list of each source node. Contains ids of target nodes that are larger.
        {
            /// If this is the first time we have seen these nodes, add them to the nid_comm_map.
            // .count() is used for membership test...
            if (nid_comm_map.count(src_id) == 0)
                INSERT_COMMUNITY(src_id, src_id); // Default community for node has same id as node
            if (nid_comm_map.count(dst_id) == 0)
                INSERT_COMMUNITY(dst_id, dst_id); // Default community for node has same id as node

            degrees[src_id] += 1;
            degrees[dst_id] += 1;
            
            src_deg = degrees[src_id];
            dst_deg = degrees[dst_id];

            // This is the modification I am interested in testing:
            // if( src_deg <= degree_threshold || dst_deg <= degree_threshold )
            if (src_deg <= degree_threshold && dst_deg <= degree_threshold)
            {
                /* NOTE: I do not think SCoDA is a good candidate for pure GPU
				implementation since it has:
				  a) conditional branching (see below)
				  b) I can not think of a way to organize the memory access properly since this algorithm relies on a random stream. */

                if (src_deg > dst_deg)
                {
                    INSERT_COMMUNITY(dst_id, COMMUNITY_OF(src_id));
                }
                else
                { // If equal, src_id is moved
                    INSERT_COMMUNITY(src_id, COMMUNITY_OF(dst_id));
                }
            }
            /////////////////////////////////// Add community edges for community graph here ////////////////////////////
            /**
			 * Approach: Since we ignore edges that connect 2 nodes that have a degree above the threshold,
			 * we know that these edges will define the connections between the communities that we are detecting.
			 * 2 possibilities:
			 *   a) Only use edges that connect 2 nodes that have BOTH exceeded the threshold
			 *      (gives us three classes of edge.  transfer, null, and comm<-comm edges are
			 *                                                            static since both nodes
			 *                                                            are locked into a community)
			 *   b) Use ALL edges that are ignored when making communities period (ALL null edges become comm edges)
			 *      NOTE: this may lead to connecting communities that have dynamic definitions.
			 *
			 * I will implement both a) and b) and see how it goes.
			 */

            // a) detect static community edges.

            // TODO: Try > and >=, (>= will mean that if both nodes have degree=degree_threshold then we will move
            //       communities AND add a community connecting edge at the same time.  Probably undesireable.)
            else if (src_deg > degree_threshold && dst_deg > degree_threshold)
            {
                if (comm_graph.has_edge(COMMUNITY_OF(src_id), COMMUNITY_OF(dst_id)))
                {
                    num_duplicate_comm_edges++;
                }
                // add community edge. Includes duplicates.
                comm_graph.add_edge(COMMUNITY_OF(src_id), COMMUNITY_OF(dst_id));
            }

            // TODO: Remove after testing, this is just for counting null edges.
            else
            {
                num_null_e++;
            }
        }
    }
    float node_comp_ratio = (float) original_graph.num_nodes() / (float) comm_graph.num_nodes();
    float edge_comp_ratio = (float) original_graph.num_edges() / (float) comm_graph.num_edges();
    return SCoDA_Results(num_null_e, num_duplicate_comm_edges, 
                         comm_graph.num_nodes(), original_graph.num_nodes, node_comp_ratio,
                         comm_graph.num_edges(), original_graph.num_edges, edge_comp_ratio);
}

}
// TODO: Write one that takes a degree threshold parameter.

int SCoDA::compute_mode_of_degree(const RPGraph::UGraph& in_graph) {
    std::unordered_map<int, uint32_t> degree_frequencies; // NOTE: uint32_t used in case of dataset with very large number of nodes with the same degree.  Keeps bounds in line with everything else.
    for(auto deg : in_graph.degrees ) {
        degree_frequencies[deg] += 1;
    }
    std::pair<int, uint32_t> maxPair = findMaxKeyValuePair(degree_frequencies); // TODO: Testing.
    return maxPair.first;
}

/**
 * Produce community graph and node_id -> community mapping. Assumes original_graph is pre-initialized.
 * 
 * returns: 
 *   UGraph reference community graph AND unordered_map reference node_id -> community mapping.
 */
int scoda_G(uint32_t degree_threshold,
            RPGraph::UGraph &original_graph, RPGraph::UGraph &comm_graph,
            std::unordered_map<RPGraph::nid_t, RPGraph::nid_t> &nid_comm_map)

int scoda_partition(uint32_t degree_threshold, std::fstream &edgelist_file)
{
    std::unordered_map<RPGraph::contiguous_nid_t, RPGraph::comm_id_t> nid_comm_map;
    std::unordered_map<RPGraph::contiguous_nid_t, uint32_t> degrees; // Make map to track degrees. Use map since node ids are not guaranteed to be contiguous.

    std::string line;
    RPGraph::nid_t src_id, dst_id;
    uint32_t src_deg, dst_deg;
    while (std::getline(edgelist_file, line)) // TODO: Genericize stream handling. Can this be passed STDIN?
    /* Good resource: https://www.ntu.edu.sg/home/ehchua/programming/cpp/cp10_IO.html */
    {
        // TODO: Have a more robust comment filtering procedure here.
        // Skip any comments
        if (line[0] == '#')
            continue;
        if (line[0] == '%')
            continue;

        // Read source and target from file
        std::istringstream(line) >> src_id >> dst_id;

        // Must add edge before retrieving degrees of nodes.
        degrees[src_id] += 1;
        degrees[dst_id] += 1;

        /// If this is the first time we have seen these nodes, add them to the nid_comm_map.
        // .count() is used for membership test...
        if (nid_comm_map.count(src_id) == 0)
            INSERT_COMMUNITY(src_id, src_id); // Default community for node has same id as node
        if (nid_comm_map.count(dst_id) == 0)
            INSERT_COMMUNITY(dst_id, dst_id); // Default community for node has same id as node

        // degrees are >= 1;
        src_deg = degrees[src_id];
        dst_deg = degrees[dst_id];

        // This is the modification I am interested in testing:
        // if( src_deg <= degree_threshold || dst_deg <= degree_threshold )
        if (src_deg <= degree_threshold && dst_deg <= degree_threshold)
        {
            /* NOTE: I do not think SCoDA is a good candidate for pure GPU
            implementation since it has:
              a) conditional branching (see below)
              b) I can not think of a way to organize the memory access properly since this algorithm relies on a random stream. */

            if (src_deg > dst_deg)
            {
                INSERT_COMMUNITY(dst_id, COMMUNITY_OF(src_id));
            }
            else
            { // If equal, src_id is moved
                INSERT_COMMUNITY(src_id, COMMUNITY_OF(dst_id));
            }
        }
        /////////////////////////////////// Add community edges for community graph here ////////////////////////////
        /**
         * Approach: Since we ignore edges that connect 2 nodes that have a degree above the threshold, 
         * we know that these edges will define the connections between the communities that we are detecting.
         * 2 possibilities:
         *   a) Only use edges that connect 2 nodes that have BOTH exceeded the threshold 
         *      (gives us three classes of edge.  transfer, null, and comm<-comm edges are 
         *                                                            static since both nodes
         *                                                            are locked into a community)
         *   b) Use ALL edges that are ignored when making communities period (ALL null edges become comm edges)
         *      NOTE: this may lead to connecting communities that have dynamic definitions.
         * 
         * I will implement both a) and b) and see how it goes.
         */

        // a) detect static community edges.
    }

    // Print partition to stdout.
    print_partition(nid_comm_map);

    return EXIT_SUCCESS;
}

// using std::vector;
// using std::cout;
// using RPGraph::nid_t;
// using std::unordered_map;
// using std::endl;

// produce partition from nid_commid_map
void print_partition(std::unordered_map<RPGraph::nid_t, RPGraph::nid_t> &nid_comm_map) {
    // TODO: Testing, can we initialize vectors of size zero?
    std::vector< std::vector<RPGraph::nid_t> > partition;
    partition.resize(nid_comm_map.size(), std::vector<RPGraph::nid_t>(0));
    
    for(auto const & pair : nid_comm_map) {
        auto node = pair.first;
        auto comm = pair.second;
        partition.at(comm).push_back(node);
    }

    for(auto const & vec : partition) {
        if(vec.size() != 0){
            // First is community id...
            for(auto const & node : vec) {
                // TODO: Does the ordering impact the way F1 or NMI reads our partitions?
                std::cout << std::to_string(node) << " ";
            }
            std::cout << std::endl;
        } // else empty community.
    }
}

} // namespace RPGraph
