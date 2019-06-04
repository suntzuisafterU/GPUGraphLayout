#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

// Array of pairs, [(degree, community), (degree, community), ...]
#define DEGREE(id) (algo_state[2*id]) // Defines function for accessing the degree of the ith node.
#define COMMUNITY(id) (algo_state[2*id+1]) // Defines function for accessing the community id associated with the ith node.
#define COMM_EDGE_1(idx) (comm_edges[idx*2]) // Get first community edge from pair.
#define COMM_EDGE_2(idx) (comm_edges[idx*2+1]) // Get second community edge from pair.

int main( int argc, char *argv[] )
{
    // printf("argc: %d\n", argc);
    /* Argument handling */
    if( argc < 5 )
    {
        printf( "Usage: ./scoda MAX_NODE_ID DEGREE_THRESHOLD IGNORE_LINES < input_graph.txt > output_communities.txt\n");
        printf( "Parameters:\n" );
        printf( "\t-STDIN: tab-separated edge list (each line: SRC_NODE <tab> DST_NODE)\n");
        printf( "\t-STDOUT: community detection result (each line: NODE <tab> COMMUNITY)\n"),
        printf( "\t-MAX_NODE_ID: int32 larger than all nodes IDs in the graph\n" );
        printf( "\t-MAX_EDGE_ID: int32 larger than all possible edges in the graph\n" );
        printf( "\t-DEGREE_THRESHOLD: parameter of the algorithm\n" );
        printf( "\t-IGNORE_LINES: number of line to ignore at the beginning of the input file\n" );
        return EXIT_FAILURE;
    }

    /* Parse degree_threshold & max_node_id */
    int32_t degree_threshold, max_node_id, max_edge_id, ignore_lines;

    /* Read command line args */
    sscanf( argv[1], "%" SCNd32, &max_node_id ); // Required for static memory allocation.
    sscanf( argv[2], "%" SCNd32, &max_edge_id); // Required for static memory allocation.
    sscanf( argv[3], "%" SCNd32, &degree_threshold ); // Calculated as the mode of degree in the network.
    sscanf( argv[4], "%" SCNd32, &ignore_lines ); // Ignore this many lines as a header.

    /* Memory allocation & initialisation */
    char linebuf[BUFSIZ]; // Buffer set to 1024 by bufset?
    // int32_t is fixed width integer.  Does this match up with our desired GPU imp?
    int32_t *algo_state = (int32_t *) malloc( 2 * max_node_id * sizeof( int32_t ) ); // allocate the array of pairs
    memset( algo_state, 0, 2 * max_node_id * sizeof( int32_t ) ); // memset overwrites memory.  Write all zeroes to array
    for( int32_t i = 0 ; i < max_node_id ; i++ )
    {
        COMMUNITY( i ) = i; // Initialize every second element to community id associated with node of same id.
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////// New ////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Aarons custom fields (or equivalent c terminology)
     */
    int32_t num_null_e = 0; // Just for counting the number of FULLY ignored edges.
    int32_t next_comm_edge_idx = 0; // keep track of community edge index when inserting.
    // Allocate array for community edges
    // TODO: Replace with more memory efficient data structure of some kind.
    int32_t *comm_edges= (int32_t *) malloc( 2 * max_edge_id * sizeof( int32_t ) ); // allocate the array of pairs
    memset( comm_edges, 0, 2 * max_edge_id * sizeof( int32_t ) ); // memset overwrites memory.  Write all zeroes to array

    ///////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////// End new ////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////

    /* Waste ignore_lines lines from input stream */
    for( int32_t i = 0 ; i < ignore_lines ; i++ )
    {
        /* fgets: Read from stream and store as C-String,
        continues until BUFSIZ-1 or \n or EOF.
        Auto terminates the string with last byte. */
        fgets( linebuf, BUFSIZ, stdin );
    }

    /* Main SCoDA loop */
    int32_t src_id, dst_id, src_deg, dst_deg;
    while( fgets( linebuf, BUFSIZ, stdin ) != NULL ) { // fgets NULL on line that only contains EOF, or there could have been an error and ferror would be set.
        /*      source,  expands to format string, store source, store dest */
        sscanf( linebuf, "%" SCNd32 "\t%" SCNd32, &src_id, &dst_id );
        src_deg = DEGREE( src_id )++; // Index into array at 2-times id and update
        dst_deg = DEGREE( dst_id )++; // degree of source and destination.
        /* NOTE: In the future if we are experimenting with SCoDA we could 
        change the way we ignore edges.  We could just change the && to ||
        for example. Made a branch to try this on the benchmark code. 
        Produces much fewer communities but I do not have a way to validate 
        them at this time. TODO: Validate different versions of this with
        F1 score and NMI. */
        // This is the modification I am interested in testing:
        // if( src_deg <= degree_threshold || dst_deg <= degree_threshold ) {
        if( src_deg <= degree_threshold && dst_deg <= degree_threshold ) { // see mod one line up
            /* NOTE: I do not think SCoDA is a good candidate for pure GPU
            implementation since it has:
              a) conditional branching (see below)
              b) I can not think of a way to organize the memory access properly since this algorithm relies on a random stream. */
            if( src_deg > dst_deg ) {
                COMMUNITY( dst_id ) = COMMUNITY( src_id );
            } else { // If equal, src_id is moved
                COMMUNITY( src_id ) = COMMUNITY( dst_id );
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
        else if ( src_deg > degree_threshold && dst_deg > degree_threshold){
            // add community edge.  
            if ( src_id > dst_id ){
              COMM_EDGE_1(next_comm_edge_idx) = src_id; // Add community source edge.
              COMM_EDGE_2(next_comm_edge_idx) = dst_id; // Add community destination edge.
            } else if ( src_id < dst_id ){
              COMM_EDGE_1(next_comm_edge_idx) = dst_id; // Add community destination edge.
              COMM_EDGE_2(next_comm_edge_idx) = src_id; // Add community source edge.
            } else {
              // TODO: Remove after testing.
              printf("ERROR: Inter community edge was attempted\n");
            }
            // TODO: Record degree of edges here, at least for reference.
            next_comm_edge_idx++;                     // Increment community edge count.
        }

        // TODO: Remove after testing, this is just for counting null edges.
        else {
            num_null_e++;
        }
        /////////////////////////////// End new code /////////////////////////////////////////////////////////////////
    }
    // for( int32_t i = 0 ; i < max_node_id ; i++ ) {
    //     if( DEGREE( i ) > 0 ) { // How often do we get a degree of zero?
    //         printf( "%" PRId32 "\t%" PRId32 "\n", i, COMMUNITY( i ) );
    //     }
    // }
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////// New printing code ////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    for( int32_t i = 0; i < max_edge_id; i++ ){
        if( COMM_EDGE_1( i ) != 0 ) { // Happens for all empty spots, could bail out first time this happens.
            printf( "%" PRId32 " %" PRId32 "\n", COMM_EDGE_1( i ), COMM_EDGE_2( i ));
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////// End new printing ////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////

    return EXIT_SUCCESS;
}
