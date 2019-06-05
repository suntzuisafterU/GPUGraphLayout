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

#include "RPCommon.hpp"
#include "RPGraph.hpp"
#include "RPGraphLayout.hpp"
#include "RPCPUForceAtlas2.hpp"

#define __NVCC__ // TEMP FOR USE IN VS
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
    printf("Loading edgelist at '%s'...", edgelist_path);
    fflush(stdout);
    /**
     * UGraph object of adjaceny list format.
     */
	// TODO: Make UGraph from some other source, not just edgelist path.
    RPGraph::UGraph graph = RPGraph::UGraph(edgelist_path);
    printf("done.\n");
    printf("    fetched %d nodes and %d edges.\n", graph.num_nodes(), graph.num_edges());

    // Create the GraphLayout and ForceAtlas2 objects.
    RPGraph::GraphLayout layout(graph);
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
     * TODO: Wrap this in a function and use it twice, once for the comm layout and once for the full layout.
     */
    for (int iteration = 1; iteration <= max_iterations; ++iteration)
    {
		compositeStep(iteration);
    }
	// TODO: Expansion kernel is called here.

	// TODO: Remake Graph, from datastructure provided by SCoDA, not from edgelist path.
	// TODO: Remake Graph layout
	// TODO: Remake ForceAtlas2 object
	// TODO: Execute FA2 again.
	

    delete fa2;
    exit(EXIT_SUCCESS);
}


