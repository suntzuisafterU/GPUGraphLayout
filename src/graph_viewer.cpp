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
#include <fstream>

#include "RPCommon.hpp"
#include "RPGraph.hpp"
#include "RPGraphLayout.hpp"
#include "RPCPUForceAtlas2.hpp"
#include "scoda.hpp"

#define __NVCC__ //TODO:  TEMP FOR USE IN VS
#ifdef __NVCC__
#include <cuda_runtime_api.h>
#include "RPGPUForceAtlas2.hpp"
#endif

/**
 * Entry point for executable.  CPU or GPU.
 */
int main(int argc, const char **argv)
{
    // For reproducibility.
    // srandom(1234);
    srandom( time(NULL) );

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////// Parse command line    //////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////

    // Parse commandline arguments
    if (argc < 10)
    {
        fprintf(stderr, "Usage: graph_viewer gpu|cpu max_iterations num_snaps sg|wg scale gravity exact|approximate edgelist_path out_path [png image_w image_h|csv|bin]\n");
        exit(EXIT_FAILURE);
    }

    const bool cuda_requested = std::string(argv[1]) == "gpu" or std::string(argv[1]) == "cuda"; // using cuda
    const int max_iterations = std::stoi(argv[2]);
    const int num_screenshots = std::stoi(argv[3]); // aka: num_snaps
    const bool strong_gravity = std::string(argv[4]) == "sg"; // strong gravity?  sg is not proportional to distance from origin.
    const float scale = std::stof(argv[5]); // scaling for repulsion force.  paper used 80.
    const float gravity = std::stof(argv[6]); // scaling for gravity. paper used 1.
    const bool approximate = std::string(argv[7]) == "approximate"; // gpu only accepts BH approximation.
    const char *edgelist_path = argv[8]; // infile
    const char *out_path = argv[9]; // output directory for images.
    std::string out_format = "png"; // default make png that is 1250x1250
    int image_w = 1250;
    int image_h = 1250;

    // Minor, accept multiple png vs csv args.
    for (int arg_no = 10; arg_no < argc; arg_no++)
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

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////// End commandline args  //////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////

    // Load graph.
    printf("Opening edgelist file at '%s'...", edgelist_path);
    fflush(stdout);
	// TODO: Would loading the file into a database in memory make our subsequent loads faster, while also allowing scoda to use a randomly generated index to satisfy it's probability constraints?

    std::fstream edgelist_file(edgelist_path, std::ifstream::in);

    RPGraph::UGraph full_graph = RPGraph::UGraph();
    RPGraph::UGraph comm_graph = RPGraph::UGraph();
    std::unordered_map<RPGraph::nid_t, RPGraph::nid_t> nid_comm_map; /**< Map is used since node_ids are not necessarily sequentially complete. */
    // TEMP VALUE!!! TODO::::
    int degree_threshold = 2; // TODO: TEMP VALUE TO TEST COMPILING
    //////////////////////////////////////////////////////////////////////////////////////////////
    int status = CommunityAlgos::scoda(degree_threshold, edgelist_file, full_graph, comm_graph, nid_comm_map);
    edgelist_file.close();
    if(status != 0){ // 0 is success
        exit(status); // propgate error code.
    }
    //////////////////////////////////////////////////////////////////////////////////////////////
    printf("Finished scoda\n");

    RPGraph::nid_t* nid_comm_array = &nid_comm_map[0]; // TODO: Figure out most efficient way to turn this unordered_map into an array.
    printf("done.\n");
    printf("    fetched %d nodes and %d edges.\n", full_graph.num_nodes(), full_graph.num_edges());

    // Create the GraphLayout and ForceAtlas2 objects.
    // TODO: In Visual Studio: Refactor layout to comm_layout and produce another layout further down for the full graph.
    RPGraph::GraphLayout layout(comm_graph); // TODO: Call scoda and get a new community UGraph first, then make a layout from that.
    RPGraph::ForceAtlas2 *fa2; // Could be CPU or GPU object.
    #ifdef __NVCC__
    if(cuda_requested)
        // GPU FA2
        fa2 = new RPGraph::CUDAForceAtlas2(layout, approximate,
                                           strong_gravity, gravity, scale);
    else
    #endif
        fa2 = new RPGraph::CPUForceAtlas2(layout, approximate,
                                          strong_gravity, gravity, scale);

    printf("Started Layout algorithm...\n");
    const int snap_period = ceil((float)max_iterations/num_screenshots);
    const int print_period = ceil((float)max_iterations*0.05);

	/**
	 * Lambda function using capture-by-reference to access all outer scope 
	 * variables with the same symbol.
	 * https://stackoverflow.com/questions/4324763/can-we-have-functions-inside-functions-in-c
	 */
	auto produceOutput = [&](int iteration) {
		/**
		 * Reverted to older version after multiple issues with the line intended to extract the basename of the network
		 */
		std::string op(out_path);
		op.append("/").append(std::to_string(iteration)).append(".").append(out_format);
		printf("Starting iteration %d (%.2f%%), writing %s...", iteration, 100 * (float)iteration / max_iterations, out_format.c_str());
		fflush(stdout);
		fa2->sync_layout(); /* The same symbol is used regardless of which stage we are at. */

		if (out_format == "png")
			layout.writeToPNG(image_w, image_h, op);
		else if (out_format == "csv")
			layout.writeToCSV(op);
		else if (out_format == "bin")
			layout.writeToBin(op);

		printf("done.\n");
	};

	auto compositeStep = [&](int iteration) {
        /**
         * Implementation in either RPGPUForceAtlas2.cpp or RPCPUForceAtlas2.cpp
         */
        fa2->doStep();
        // If we need to, write the result to a png
        if (num_screenshots > 0 && (iteration % snap_period == 0 || iteration == max_iterations))
        {
			produceOutput(iteration);
        }

        // Else we print (if we need to)
        else if (iteration % print_period == 0)
        {
          printf("Starting iteration %d (%.2f%%).\n", iteration, 100*(float)iteration/max_iterations);
        }
	};

	/**
	 * Initial layout will be produced from community graph.
	 */
    for (int iteration = 1; iteration <= max_iterations; ++iteration)
    {
		compositeStep(iteration);
    }
	// TODO: Expansion kernel is called here. NOTE: kernel will require some form of array datastructure to operate on, will also have to look up the layout coordinate associated with the community_node.
	// TODO: Sequential expansion function here.
	/**
	 * Expansion:
	 *   Use setCoordinates(node_id, coordinate(x,y)); to expand the community layout to a full graph layout.
	 */

	// TODO: Remake Graph layout
	// TODO: Remake ForceAtlas2 object
	// TODO: Execute FA2 again.

    delete fa2;
    exit(EXIT_SUCCESS);
}
