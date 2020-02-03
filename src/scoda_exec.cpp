#include <string>
#include <iostream>

#include "common/RPGraph.hpp" // For UGraph
#include "common/RPCommon.hpp" // For is_file_exists()
#include "scoda/scoda.hpp"
#include "common/RPTypeDefs.hpp"
#include "utils/IO/RPGraphLayoutUtils.hpp"

int main(int argc, const char** argv) {

    if(argc != 3) {
        std::cout << "Usage: scoda_exec <edgelist_file> <outpath> " << std::endl;
        exit(EXIT_FAILURE);
    }

    // Accept edge stream from stdin
    const char* inpath = argv[1];
    const char* outpath = argv[2];

    // Check in_path and out_path
    if (!is_file_exists(inpath))
    {
        fprintf(stderr, "error: No edgelist at %s\n", inpath);
        exit(EXIT_FAILURE);
    }
    // TODO: This currently looks for a valid output folder, we are going to output a file so we don't need an entire folder.
    // if (!is_file_exists(outpath))
    // {
    //     fprintf(stderr, "error: No output folder at %s\n", outpath);
    //     exit(EXIT_FAILURE);
    // }

    std::string inpath_s(inpath);
    std::string outpath_s(outpath);

    // Initialize graph from inpath.
    RPGraph::UGraph original_graph(inpath_s);
    RPGraph::UGraph comm_graph;
    RPGraph::nid_comm_map_t nid_comm_map;

    // Invoke SCoDA.
    RPGraph::SCoDA scoda;
    RPGraph::SCoDA_Report report = scoda.compute_partition(original_graph, comm_graph, nid_comm_map);

    RPGraph::writeCommunityGraphToEdgelist(comm_graph, report, outpath);

    std::cout << "Finished writing community graph edge list file." << std::endl;

    return EXIT_SUCCESS;
}
