/*
 ==============================================================================

 graph_viewer.cpp
 Copyright © 2016, 2017, 2018  G. Brinkmann

 This file is part of graph_viewer.

 graph_viewer is free software: you can redistribute it and/or modify
 it under the terms of version 3 of the GNU Affero General Public License as
 published by the Free Software Foundation.

 graph_viewer is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Affero General Public License for more details.

 You should have received a copy of the GNU Affero General Public License
 along with graph_viewer.  If not, see <https://www.gnu.org/licenses/>.

-------------------------------------------------------------------------------

 This code was written as part of a research project at the Leiden Institute of
 Advanced Computer Science (www.liacs.nl). For other resources related to this
 project, see https://liacs.leidenuniv.nl/~takesfw/GPUNetworkVis/.

 ==============================================================================
*/

// Reading May 21th

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
// #include <unique_ptr> // TODO: Use after testing.

#include "common/RPCommon.hpp"
#include "common/RPGraph.hpp"
#include "common/RPGraphLayout.hpp"
#include "utils/IO/RPGraphLayoutUtils.hpp"
#include "FA2/RPCPUForceAtlas2.hpp"
#include "scoda/scoda.hpp"
#include "graph_viewer/RPGraphViewer.hpp"

#ifdef __NVCC__
#include <cuda_runtime_api.h>
#include "FA2/RPGPUForceAtlas2.hpp"
#endif

namespace { // anonymous

RPGraph::GraphViewer parseCommandLine() {
    // Parse commandline arguments
    if (argc < 10)
    {
        fprintf(stderr, "Usage: graph_viewer gpu|cpu max_iterations num_snaps sg|wg scale gravity exact|approximate linlog|regular [percentage_iterations_on_comm_graph] edgelist_path out_path [png image_w image_h|csv|bin]\n");
        exit(EXIT_FAILURE);
    }

    const bool cuda_requested = std::string(argv[1]) == "gpu" or std::string(argv[1]) == "cuda"; // using cuda
    const int max_iterations = std::stoi(argv[2]);
    const int num_screenshots = std::stoi(argv[3]); // aka: num_snaps
    const bool strong_gravity = std::string(argv[4]) == "sg"; // strong gravity?  sg is not proportional to distance from origin.
    const float scale = std::stof(argv[5]); // scaling for repulsion force.  paper used 80.
    const float gravity = std::stof(argv[6]); // scaling for gravity. paper used 1.
    const bool approximate = std::string(argv[7]) == "approximate"; // gpu only accepts BH approximation.
    const bool use_linlog = std::string(argv[8]) == "linlog"; // Enable linlog
    const float percentage_iterations_on_comm_graph = std::stof(argv[9])/100.0f;
    const char *edgelist_path = argv[10]; // infile
    const char *out_path = argv[11]; // output directory for images.
    const char *out_file_prefix = argv[12]; // annotated outfile names
    std::string out_format = "png"; // default make png that is 1250x1250
    int image_w = 1250;
    int image_h = 1250;

    // Minor, accept multiple png vs csv args.
    for (int arg_no = 13; arg_no < argc; arg_no++)
    {
        if(std::string(argv[arg_no]) == "png")
        {
            out_format = "png";
            image_w = std::stoi(argv[arg_no+1]);
            image_h = std::stoi(argv[arg_no+2]);
            arg_no += 2;
        }

        else if(std::string(argv[arg_no]) == "csv")
        {
            out_format = "csv";
        }

        else if(std::string(argv[arg_no]) == "bin")
        {
            out_format = "bin";
        }
    }


    if(cuda_requested and not approximate)
    {
        fprintf(stderr, "error: The CUDA implementation (currently) requires Barnes-Hut approximation.\n");
        exit(EXIT_FAILURE);
    }

    /**
     * This currently fails if the inpath exists but is not an actual file.
     * It loads 0 edges and 0 nodes and then CUDA has a memory access failure.
     */
    // Check in_path and out_path
    if (!is_file_exists(edgelist_path))
    {
        fprintf(stderr, "error: No edgelist at %s\n", edgelist_path);
        exit(EXIT_FAILURE);
    }
    if (!is_file_exists(out_path))
    {
        fprintf(stderr, "error: No output folder at %s\n", out_path);
        exit(EXIT_FAILURE);
    }

    // If not compiled with cuda support, check if cuda is requested.
    #ifndef __NVCC__
    if(cuda_requested)
    {
        fprintf(stderr, "error: CUDA was requested, but not compiled for.\n");
        exit(EXIT_FAILURE);
    }
    #endif

    fflush(stdout); // TODO: Why do this?

    // return unique_ptr<RPGraph::GraphViewer> // TODO: Use after testing
    return GraphViewer()
}

int main(int argc, const char **argv)
{
    // For reproducibility.
    // srandom(1234);
    srandom( time(NULL) ); // TODO: Parameterize

    GraphViewer parseCommandLine();

	// TODO: Would loading the file into a database in memory make our subsequent loads faster, while also allowing scoda to use a randomly generated index to satisfy it's probability constraints?

    RPGraph::UGraph full_graph(edgelist_path); // Initialize full_graph from provided path.
    RPGraph::UGraph comm_graph;                // Initialize empty comm_graph for scoda to fill.
    std::unordered_map<RPGraph::nid_t, RPGraph::nid_t> nid_comm_map; /**< Map is used since node_ids are not necessarily sequentially complete. Stack allocation. */
	//////////////////////////////////////////////////////////////////////////////////////////////
    uint32_t degree_threshold = 2; // TODO: TEMP VALUE TO TEST COMPILING, should figure out some way to parameterize or detect this in the future.  Note that detection requires streaming the entire graph with the authors implementation.  We could try sampling from the first portion of the graph (say 10%) and using a default value up to that point.
    //////////////////////////////////////////////////////////////////////////////////////////////
    int status = CommunityAlgos::scoda_G(degree_threshold, full_graph, comm_graph, nid_comm_map); /**< Currently the streaming algorithm is required to also initialize any UGraph datastructures that are required. */
	
    if(status != 0){ // 0 is success
        exit(status); // propgate error code.
    }
    //////////////////////////////////////////////////////////////////////////////////////////////
    printf("Finished scoda\n");

    printf("done.\n");
    printf("    fetched %d nodes and %d edges.\n", full_graph.num_nodes(), full_graph.num_edges());

    // Create the GraphLayout and ForceAtlas2 objects.
    RPGraph::GraphLayout comm_layout(comm_graph); /* Produce initial layout from comm_graph. */
	RPGraph::GraphLayout* current_layout = &comm_layout; /* Use pointer in lambdas that can be modified. */
    RPGraph::ForceAtlas2* comm_fa2; // Could be CPU or GPU object.
	bool randomize = true;
    #ifdef __NVCC__
    if(cuda_requested)
        // GPU FA2
        comm_fa2 = new RPGraph::CUDAForceAtlas2(comm_layout, approximate,
                                           strong_gravity, gravity, scale, randomize, use_linlog);
    else
    #endif
        comm_fa2 = new RPGraph::CPUForceAtlas2(comm_layout, approximate,
                                          strong_gravity, gravity, scale, randomize, use_linlog);

	RPGraph::ForceAtlas2* fa2 = comm_fa2;
    printf("Started Layout algorithm...\n");
    const int snap_period = ceil((float)max_iterations/num_screenshots);
    const int print_period = ceil((float)max_iterations*0.05);

	/**
	 * Initial layout will be produced from community graph.
	 */
    for (int iteration = 1; iteration < ceil(max_iterations * percentage_iterations_on_comm_graph); ++iteration)
    {
		compositeStep(iteration); /* comm graph layout is produced. */
    }
	fa2 = nullptr;

    RPGraph::GraphLayout full_layout(full_graph);
    current_layout = &full_layout; /* Use pointer in lambdas that can be modified. */

	// TODO: Expansion kernel instead of sequential code called here. NOTE: Low priority.

	/**
	 * Expansion:
	 *   Use setCoordinates(node_id, coordinate(x,y)); to expand the community layout to a full graph layout.
	 */
	for (const auto& nid_commid_pair : nid_comm_map) {
		RPGraph::nid_t node = nid_commid_pair.first;
		RPGraph::nid_t comm = nid_commid_pair.second;
		RPGraph::Coordinate comm_coordinate = comm_layout.getCoordinate(comm_graph.node_map[comm]);
		// TODO: Is it possible for a node to not have a community in the graph??? Probably yes. Does not seem to be an issue.
		full_layout.setCoordinates(full_graph.node_map[node], comm_coordinate); /**< Set the nodes id to be that of it's community. */
	}

	// TODO: Was moved here for safety.  May be able to move it above the full_layout initialization.
	delete comm_fa2; /* Free old comm_fa2 object when done.  This is required to deallocate GPU memory. */

	randomize = false;
	RPGraph::ForceAtlas2* full_fa2;
    #ifdef __NVCC__
    if(cuda_requested)
        // GPU FA2
        full_fa2 = new RPGraph::CUDAForceAtlas2(full_layout, approximate,
                                           strong_gravity, gravity, scale, randomize, use_linlog);
    else
    #endif // __NVCC__
        full_fa2 = new RPGraph::CPUForceAtlas2(full_layout, approximate,
                                          strong_gravity, gravity, scale, randomize, use_linlog);
	fa2 = full_fa2;

	/**
	 * Second layout with full graph.
	 */
    for (int iteration = ceil(max_iterations * percentage_iterations_on_comm_graph); iteration <= max_iterations; ++iteration)
    {
		compositeStep(iteration); /* full graph layout is produced. */
    }
	fa2 = nullptr;
	delete full_fa2; /* Free last ForceAtlas2 object. */

    exit(EXIT_SUCCESS);
}

} // namespace anonymous