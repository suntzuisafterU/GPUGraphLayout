#include "scoda.hpp"
#include <iostream> // TODO: REMOVE, TEMP, DEBUGGING

namespace RPGraph {

// TODO: Replace macros with inlined functions.

SCoDA_Report SCoDA::compute_partition(RPGraph::UGraph& original_graph, RPGraph::UGraph& comm_graph, 
            nid_comm_map_t& nid_comm_map) { // TODO: Use the typedefs

// IMPORTANT: Bug in INSERT_COMMUNITY macro fixed.
#define INSERT_COMMUNITY(nid, comm_id) (nid_comm_map[nid] = comm_id)         /// Defines function to update community association of node nid. 
#define COMMUNITY_OF(nid) (nid_comm_map.at(nid))                             /// Defines function for accessing the community id associated with the ith node.

    uint32_t degree_threshold = compute_mode_of_degree(original_graph);
    /* Memory allocation & initialisation */
    nid_comm_map.reserve(original_graph.num_nodes()); // TODO: Testing

    uint32_t num_null_e = 0; // Just for counting the number of FULLY ignored edges.
    uint32_t num_duplicate_comm_edges = 0;

    printf("\nStarting scoda.\n");
    /* Main SCoDA loop */

    uint32_t src_deg, dst_deg;
    std::vector<uint32_t> degrees; /// Required since SCoDA must track the degree of edges as they are streamed.
    degrees.resize(original_graph.num_nodes()); // IMPORTANT: if resize is not used the incrementation of value of degree inside the loop will not work at all.

    // if (original_graph.num_nodes() == 0 || original_graph.num_edges() == 0) exit(EXIT_FAILURE); // ERROR, empty UGraph.
    // TODO: Move explicit manipulation of UGraph to internal function or iterator class.
    for (RPGraph::contiguous_nid_t src_id = 0; src_id < original_graph.num_nodes(); src_id++) // Iterate over source nodes
    {
        for (RPGraph::contiguous_nid_t dst_id : original_graph.neighbors_with_geq_id(src_id)) // Iterate over adjacency list of each source node. Contains ids of target nodes that are larger.
        {
			// IMPORTANT: Found definite bug in the way nid_comm_map is built.  The nodes are ONLY being mapped to a community of the same id.

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
            else if (src_deg > degree_threshold && dst_deg > degree_threshold && 
					COMMUNITY_OF(src_id) != COMMUNITY_OF(dst_id)) // IMPORTANT: Avoid adding self edges to comm_graph.  TODO: Testing.
            {
				// TODO: Fix the insertion mapping of community nodes...
                if (comm_graph.has_edge_public(COMMUNITY_OF(src_id), COMMUNITY_OF(dst_id)))
                {
                    num_duplicate_comm_edges++;
                }
                // add community edge. Includes duplicates.
                comm_graph.add_edge_public(COMMUNITY_OF(src_id), COMMUNITY_OF(dst_id));
            }

            // TODO: Remove after testing, this is just for counting null edges.
            else
            {
                num_null_e++;
            }
        }
    }

    uint32_t num_original_nodes = original_graph.num_nodes();
    uint32_t num_comm_nodes = comm_graph.num_nodes();
    float node_comp_ratio = (float) original_graph.num_nodes() / (float) comm_graph.num_nodes();
    uint32_t num_original_e = original_graph.num_edges();
    uint32_t num_comm_e = comm_graph.num_edges();
    float edge_comp_ratio = (float) original_graph.num_edges() / (float) comm_graph.num_edges();

    SCoDA_Report results = { 
		degree_threshold,
        num_null_e, 
        num_duplicate_comm_edges, 
        num_original_nodes,
        num_comm_nodes,
        node_comp_ratio,
        num_original_e,
        num_comm_e,
        edge_comp_ratio 
    };

    return results;
}

// TODO: Write one that takes a degree threshold parameter.

uint32_t SCoDA::compute_mode_of_degree(RPGraph::UGraph& in_graph) {
    std::unordered_map<uint32_t, uint32_t> degree_frequencies; // NOTE: uint32_t used in case of dataset with very large number of nodes with the same degree.  Keeps bounds in line with everything else.
    for(auto& deg : in_graph.get_degrees() ) {
        degree_frequencies[deg.second] += 1; // TODO: BUG HERE, is not counting the frequencies properly. Was working before I changed the DerivedGraph struct.  in_graph is a valid graph from the looks of it in gdb.
    }

    // When inspecting the in_graph, it appears to have numbers of elements for size of degrees and adjacency_list that are way out to lunch, must be a conversion between unsigned and signed somewhere.  Going to check with VisualStudio.

    std::pair<uint32_t, uint32_t> maxPair = findMaxKeyValuePair(degree_frequencies); // TODO: Testing.
	return std::max(maxPair.first, 2U);
}

// TODO: Accept a file and send this through a DatasetAdapter...
// produce partition from nid_commid_map
void SCoDA::print_partition(nid_comm_map_t &nid_comm_map/* TODO: , UGraph must be used to unmap the node ids in current configuration. */) {
	throw "DANGER: Will print a partition that does not correlate with external data!";
    // TODO: Testing, can we initialize vectors of size zero?
    std::vector< std::vector<RPGraph::contiguous_nid_t> > partition;
    partition.resize(nid_comm_map.size(), std::vector<RPGraph::contiguous_nid_t>(0));
    
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
                std::cout << std::to_string(node) << " "; // TODO: IMPORTANT! Must send this through a DatasetAdapter
            }
            std::cout << std::endl;
        } // else empty community.
    }
}

} // namespace RPGraph
