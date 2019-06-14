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

    std::fstream edgelist_file(edgelist_path, std::ifstream::in); // TODO: Does this return a reference? Yes. See: https://stackoverflow.com/questions/655065/when-should-i-use-the-new-keyword-in-c and http://www.gotw.ca/gotw/009.htm

    RPGraph::UGraph full_graph = RPGraph::UGraph(); // Changed back to stack allocated reference.
    RPGraph::UGraph comm_graph = RPGraph::UGraph();
    std::unordered_map<RPGraph::nid_t, RPGraph::nid_t> nid_comm_map; /**< Map is used since node_ids are not necessarily sequentially complete. Stack allocation. */
    // TEMP VALUE!!! TODO::::
    int degree_threshold = 2; // TODO: TEMP VALUE TO TEST COMPILING, should figure out some way to parameterize or detect this in the future.  Note that detection requires streaming the entire graph with the authors implementation.  We could try sampling from the first portion of the graph (say 10%) and using a default value up to that point.
    //////////////////////////////////////////////////////////////////////////////////////////////
    int status = CommunityAlgos::scoda(degree_threshold, edgelist_file, full_graph, comm_graph, nid_comm_map); /**< Currently the streaming algorithm is required to also initialize any UGraph datastructures that are required. */
    // TODO: We pass in regerences to the full_graph and comm_graph objects, but nothing comes back. How should these be passed?
    edgelist_file.close();
    if(status != 0){ // 0 is success
        exit(status); // propgate error code.
    }
    //////////////////////////////////////////////////////////////////////////////////////////////
    printf("Finished scoda\n");

	// TODO: Get rid of this.  It works but produces a very large, sparse array, and I am not sure if the data is safe.
    // RPGraph::nid_t* nid_comm_array = &nid_comm_map[0]; // TODO: Figure out most efficient way to turn this unordered_map into an array.
    printf("done.\n");
    printf("    fetched %d nodes and %d edges.\n", full_graph.num_nodes(), full_graph.num_edges());

    // Create the GraphLayout and ForceAtlas2 objects.
    // TODO: how is this graph allocated? When is it freed?
    RPGraph::GraphLayout comm_layout = RPGraph::GraphLayout(comm_graph); /* Produce initial layout from comm_graph. */
	RPGraph::GraphLayout* current_layout = &comm_layout; /* Use pointer in lambdas that can be modified. */
    RPGraph::ForceAtlas2* comm_fa2; // Could be CPU or GPU object.
	bool randomize = true;
    #ifdef __NVCC__
    if(cuda_requested)
        // GPU FA2
        // TODO: Is this the correct way to initialize the value at a pointer.
        comm_fa2 = new RPGraph::CUDAForceAtlas2(comm_layout, approximate,
                                           strong_gravity, gravity, scale, randomize);
    else
    #endif
        comm_fa2 = new RPGraph::CPUForceAtlas2(comm_layout, approximate,
                                          strong_gravity, gravity, scale, randomize);

	RPGraph::ForceAtlas2* fa2 = comm_fa2;
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
			// TODO: Getting error here, segmentation fault from libpng.  Not sure why.  Rerunning, might be heisenbug.
			current_layout->writeToPNG(image_w, image_h, op);
		else if (out_format == "csv")
			current_layout->writeToCSV(op);
		else if (out_format == "bin")
			current_layout->writeToBin(op);

		printf("done.\n");
	};

	auto compositeStep = [&](int iteration) {
        /**
         * Implementation in either RPGPUForceAtlas2.cu or RPCPUForceAtlas2.cpp
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
    for (int iteration = 1; iteration < ceil(max_iterations/2); ++iteration)
    {
		compositeStep(iteration); /* comm graph layout is produced. */
    }
	fa2 = nullptr;
    // TODO: We are calling delete on a reference, is this valid?
	delete comm_fa2; /* Free old comm_fa2 object when done.  This is required to deallocate GPU memory. */

    // TODO: ERROR: This is the start of our problems according to valgrind.
    RPGraph::GraphLayout full_layout = RPGraph::GraphLayout(full_graph);
    current_layout = &full_layout; /* Use pointer in lambdas that can be modified. */
	// TODO: Use comm_layout to initialize full_layout positions. Must be done before intializing fa2
	// TODO: THIS DIDN'T WORK. FREE MEMORY PROPERLY LATER. delete fa2; /* Free old fa2 object */
	randomize = true; /* TEMP: Random to test duplicated code correctness. TODO: Make not random. */
	RPGraph::ForceAtlas2* full_fa2;
    #ifdef __NVCC__
    if(cuda_requested)
        // GPU FA2
        full_fa2 = new RPGraph::CUDAForceAtlas2(full_layout, approximate,
                                           strong_gravity, gravity, scale, randomize);
    else
    #endif
        full_fa2 = new RPGraph::CPUForceAtlas2(full_layout, approximate,
                                          strong_gravity, gravity, scale, randomize);
	////////////////
	///////////////
	fa2 = full_fa2;
	///////////////
	////////////////
	// TODO: Expansion kernel is called here. NOTE: kernel will require some form of array datastructure to operate on, will also have to look up the layout coordinate associated with the community_node.
	// TODO: Sequential expansion function here.
	/**
	 * Expansion:
	 *   Use setCoordinates(node_id, coordinate(x,y)); to expand the community layout to a full graph layout.
	 */

	/**
	 * Second layout with full graph.
	 */
    for (int iteration = ceil(max_iterations/2); iteration <= max_iterations; ++iteration)
    {
		compositeStep(iteration); /* full graph layout is produced. */
    }
	fa2 = nullptr;
    // TODO: Create a struct for all these related things, then can have a destructor and just delete it.
	delete full_fa2; /* Free last ForceAtlas2 object. */

    exit(EXIT_SUCCESS);
}
