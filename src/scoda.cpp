#include "scoda.hpp"

namespace CommunityAlgos {

// NOTE: We don't have to update degree anymore.  Just add the nodes to the UGraph.
#define DEGREE(id) (full_graph.degree(id))         // Defines function for accessing the degree of the ith node.
#define COMMUNITY(id) (nid_comm_vec[id])           // Defines function for accessing the community id associated with the ith node.
#define COMM_EDGE_1(idx) (comm_edges[idx * 2])     // Get first community edge from pair.
#define COMM_EDGE_2(idx) (comm_edges[idx * 2 + 1]) // Get second community edge from pair.

/**
 * Produce community graph and node_id -> community mapping.
 * 
 * returns: 
 *   UGraph pointer community graph AND unorderedmap pointer node_id -> community mapping.
 */

void scoda(uint32_t degree_threshold, RPGraph::UGraph &full_graph, RPGraph::UGraph &comm_graph,
           std::vector<nid_t> &nid_comm_vec)
{
    /* Memory allocation & initialisation */

    uint32_t num_null_e = 0;         // Just for counting the number of FULLY ignored edges.


    /* Main SCoDA loop */
    char linebuf[BUFSIZ];
    nid_t src_id, dst_id, src_deg, dst_deg;
    while (fgets(linebuf, BUFSIZ, stdin) != NULL)
    { // fgets NULL on line that only contains EOF, or there could have been an error and ferror would be set.
        /*      source,  expands to format string, store source, store dest */
        sscanf(linebuf, "%" SCNu32 "\t%" SCNu32, &src_id, &dst_id); // TODO: Does this agree with uint32_t?
        /* NOTE: In the future if we are experimenting with SCoDA we could 
        change the way we ignore edges.  We could just change the && to ||
        for example. Made a branch to try this on the benchmark code. 
        Produces much fewer communities but I do not have a way to validate 
        them at this time. TODO: Validate different versions of this with
        F1 score and NMI. */
        // This is the modification I am interested in testing:
        // if( src_deg <= degree_threshold || dst_deg <= degree_threshold ) {
        if (src_deg <= degree_threshold && dst_deg <= degree_threshold)
        { // see mod one line up
            /* NOTE: I do not think SCoDA is a good candidate for pure GPU
            implementation since it has:
              a) conditional branching (see below)
              b) I can not think of a way to organize the memory access properly since this algorithm relies on a random stream. */
            if (src_deg > dst_deg)
            {
                COMMUNITY(dst_id) = COMMUNITY(src_id);
            }
            else
            { // If equal, src_id is moved
                COMMUNITY(src_id) = COMMUNITY(dst_id);
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
            // add community edge.
            comm_graph.add_edge(src_id, dst_id);
            // TODO: Could have duplicate edges, conside making edges weighted.
        }

        // TODO: Remove after testing, this is just for counting null edges.
        else
        {
            num_null_e++;
        }
    }
    return EXIT_SUCCESS;
    }
} // namespace CommunityAlgos