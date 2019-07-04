#include "scoda.hpp"

namespace CommunityAlgos
{

#define DEGREE(id) (full_graph.degree(full_graph.node_map[id]))              /// Defines function for accessing the degree of the ith node.
#define INSERT_COMMUNITY(nid, comm_id) (nid_comm_map.insert({nid, comm_id})) /// Defines function to update community association of node nid.
#define COMMUNITY_OF(nid) (nid_comm_map.at(nid))                             /// Defines function for accessing the community id associated with the ith node.

/**
 * Produce community graph and node_id -> community mapping. Assumes full_graph is pre-initialized.
 * 
 * returns: 
 *   UGraph reference community graph AND unordered_map reference node_id -> community mapping.
 */
int scoda_G(uint32_t degree_threshold,
            RPGraph::UGraph &full_graph, RPGraph::UGraph &comm_graph,
            std::unordered_map<RPGraph::nid_t, RPGraph::nid_t> &nid_comm_map)
{
    /* Memory allocation & initialisation */
    nid_comm_map.reserve(full_graph.num_nodes()); // TODO: Testing

    int num_null_e = 0; // Just for counting the number of FULLY ignored edges.
    int num_duplicate_edges = 0;

    printf("\nStarting scoda.\n");
    /* Main SCoDA loop */

    RPGraph::nid_t src_id, dst_id, src_deg, dst_deg;
    if (full_graph.num_nodes() == 0 || full_graph.num_edges() == 0)
        return -1; // ERROR, empty UGraph.
    // TODO: Move explicit manipulation of UGraph to internal function or iterator class.
    for (RPGraph::nid_t source_id = 0; source_id < full_graph.num_nodes(); ++source_id) // Iterate over source nodes
    {
        for (RPGraph::nid_t target_id : full_graph.neighbors_with_geq_id(source_id)) // Iterate over adjacency list of each source node. Contains ids of target nodes that are larger.
        {
            src_id = full_graph.node_map_r[source_id]; // TODO: This should go.  Reverse mapping to maintain macros and other operations across polymorphic SCoDA calls.
            dst_id = full_graph.node_map_r[target_id];

            /// If this is the first time we have seen these nodes, add them to the nid_comm_map.
            // .count() is used for membership test...
            if (nid_comm_map.count(src_id) == 0)
                INSERT_COMMUNITY(src_id, src_id); // Default community for node has same id as node
            if (nid_comm_map.count(dst_id) == 0)
                INSERT_COMMUNITY(dst_id, dst_id); // Default community for node has same id as node

            // degrees are >= 1;
            src_deg = DEGREE(src_id);
            dst_deg = DEGREE(dst_id);

            // This is the modification I am interested in testing:
            // if( src_deg <= degree_threshold || dst_deg <= degree_threshold )
            if (src_deg <= degree_threshold && dst_deg <= degree_threshold)
            {
                /* NOTE: I do not think SCoDA is a good candidate for pure GPU
				implementation since it has:
				  a) conditional branching (see below)
				  b) I can not think of a way to organize the memory access properly since this algorithm relies on a random stream. */

                // TODO: Initialize the community ids of the unordered_map

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
                    num_duplicate_edges++;
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
    printf("num_null_e: %d\n", num_null_e);
    printf("num_duplicate_edges: %d\n", num_duplicate_edges);
    printf("num_comm_e: %d\n", comm_graph.num_edges());
    return EXIT_SUCCESS;
}

/**
 * Produce community graph and node_id -> community mapping. Assumes `edgelist_file` is a list of pairs "src target" representing undirected edges.
 * 
 * returns: 
 *   UGraph reference community graph AND unordered_map reference node_id -> community mapping.
 */
int scoda_G(uint32_t degree_threshold, std::fstream &edgelist_file,
            RPGraph::UGraph &full_graph, RPGraph::UGraph &comm_graph,
            std::unordered_map<RPGraph::nid_t, RPGraph::nid_t> &nid_comm_map)
{
    /* Memory allocation & initialisation */

    int num_null_e = 0; // Just for counting the number of FULLY ignored edges.
    int num_duplicate_edges = 0;

    printf("\nStarting scoda.\n");
    /* Main SCoDA loop */

    std::string line;
    RPGraph::nid_t src_id, dst_id, src_deg, dst_deg;
    while (std::getline(edgelist_file, line))
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
        full_graph.add_edge(src_id, dst_id);

        /// If this is the first time we have seen these nodes, add them to the nid_comm_map.
        // .count() is used for membership test...
        if (nid_comm_map.count(src_id) == 0)
            INSERT_COMMUNITY(src_id, src_id); // Default community for node has same id as node
        if (nid_comm_map.count(dst_id) == 0)
            INSERT_COMMUNITY(dst_id, dst_id); // Default community for node has same id as node

        // degrees are >= 1;
        src_deg = DEGREE(src_id);
        dst_deg = DEGREE(dst_id);

        // This is the modification I am interested in testing:
        // if( src_deg <= degree_threshold || dst_deg <= degree_threshold )
        if (src_deg <= degree_threshold && dst_deg <= degree_threshold)
        {
            /* NOTE: I do not think SCoDA is a good candidate for pure GPU
            implementation since it has:
              a) conditional branching (see below)
              b) I can not think of a way to organize the memory access properly since this algorithm relies on a random stream. */

            // TODO: Initialize the community ids of the unordered_map

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
                num_duplicate_edges++;
            }
            // add community edge.
            comm_graph.add_edge(COMMUNITY_OF(src_id), COMMUNITY_OF(dst_id));
            // TODO: Could have duplicate edges, consider making edges weighted.
        }

        // TODO: Remove after testing, this is just for counting null edges.
        else
        {
            num_null_e++;
        }
    }
    printf("num_null_e: %d\n", num_null_e);
    printf("num_duplicate_edges: %d\n", num_duplicate_edges);
    printf("num_comm_e: %d\n", comm_graph.num_edges());
    return EXIT_SUCCESS;
}

int scoda_partition(uint32_t degree_threshold, std::fstream &edgelist_file)
{
    std::unordered_map<RPGraph::nid_t, RPGraph::nid_t> nid_comm_map;
    std::unordered_map<RPGraph::nid_t, uint32_t> degrees; // Make map to track degrees. Use map since node ids are not guaranteed to be contiguous.

    std::string line;
    RPGraph::nid_t src_id, dst_id;
    uint32_t src_deg, dst_deg;
    while (std::getline(edgelist_file, line)) // TODO: Genericize stream handling. 
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
        degrees[src_id]++; // TODO: IMPORTANT: Is this incrementing the degrees of the associated id?
        degrees[dst_id]++;

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

            // TODO: Initialize the community ids of the unordered_map

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

} // namespace CommunityAlgos