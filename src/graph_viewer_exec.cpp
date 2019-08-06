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

#include "graph_viewer/RPGraphViewer.hpp"
#include <utility>
#include <memory>

int main(int argc, const char **argv) {
    // Seed random number generator, for reproducability.
    srandom(1234);

    // Parse commandline arguments
    if (argc < 10)
    {
        fprintf(stderr, "Usage: graph_viewer original|single|stacked gpu|cpu max_iterations num_snaps sg|wg scale gravity exact|approximate linlog|regular [percentage_iterations_on_comm_graph] edgelist_path out_path out_file_prefix [png image_w image_h|csv|bin]\n");
        exit(EXIT_FAILURE);
    }

	const std::string pipeline_to_use = std::string(argv[1]); /* See if statements at bottom of this file. */
    const bool cuda_requested = std::string(argv[2]) == "gpu" or std::string(argv[2]) == "cuda"; // using cuda
    const int max_iterations = std::stoi(argv[3]);
    const int num_screenshots = std::stoi(argv[4]); // aka: num_snaps
    const bool strong_gravity = std::string(argv[5]) == "sg"; // strong gravity?  sg is not proportional to distance from origin.
    const float scale = std::stof(argv[6]); // scaling for repulsion force.  paper used 80.
    const float gravity = std::stof(argv[7]); // scaling for gravity. paper used 1.
    const bool approximate = std::string(argv[8]) == "approximate"; // gpu only accepts BH approximation.
    const bool use_linlog = std::string(argv[9]) == "linlog"; // Enable linlog
    const float percentage_iterations_on_comm_graph = std::stof(argv[10])/100.0f;
    const char *edgelist_path = argv[11]; // infile
    const char *out_path = argv[12]; // output directory for images.
    const char *out_file_prefix = argv[13]; // annotated outfile names
    std::string out_format = "png"; // default make png that is 1250x1250
    int image_w = 1250;
    int image_h = 1250;

    int arg_no = 14;
    if(std::string(argv[arg_no]) == "png")
    {
        out_format = "png";
        image_w = std::stoi(argv[arg_no+1]);
        image_h = std::stoi(argv[arg_no+2]);
        std::cout << "Set image_w to " << image_w << " and image_h to " << image_h << std::endl;
        arg_no += 2;
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

    // return 
    // TODO: TEMP
    std::unique_ptr<RPGraph::GraphViewer> graph_viewer (new RPGraph::GraphViewer(
        cuda_requested,
        max_iterations,
        num_screenshots,
        strong_gravity,
        scale,
        gravity,
        approximate,
        use_linlog,
        percentage_iterations_on_comm_graph,
        edgelist_path, // infile
        out_path, // output directory for images.
        out_file_prefix, // annotated outfile names
        out_format, // default make png that is 1250x1250
        image_w,
        image_h));

    // For reproducibility.
    // srandom(1234);
    srandom( time(NULL) ); // TODO: Parameterize

    ////////////////TODO: JUST TESTING COMPILATION///////////////
    // std::unique_ptr<GraphViewer> graph_viewer( parseCommandLine() ); // TODO: Easiest way to initialize??
    ////////////////////////////////

    // TODO: Using lambdas temporarily to run different configurations.

    auto like_original = [&] () { /* Verified. */
        std::cout << "In lambda: like_original " << std::endl;
        graph_viewer->init(); // Required ?? since constructor would be to complex if it did all initialization.
        graph_viewer->iterate_and_periodically_show(max_iterations, /* randomize = */true, "OLD_STYLE_GV_LOOP"); /* TODO: reVerify. */
        std::cout << "Finished with simplified GV script.\n" << std::endl;
    };

    auto compress_and_show_comm_graph = [&] () { /* Verified. */
        std::cout << "In lambda: compress_and_show_comm_graph " << std::endl;
        graph_viewer->init();

        // call SCoDA, compress.
        graph_viewer->compress(); /* Verify compress works. */
        std::cout << "GV::compress() works, if everything is intact here." << std::endl;

        graph_viewer->iterate_and_periodically_show(max_iterations, /* randomize = */true, "BUGGED_COMPRESS_AND_SHOW_COMM_GRAPH");
        std::cout << "GV::iterate_and_periodically_show() works on internally compressed community graph, if everything is intact here." << std::endl;

        std::cout << "Finished compress_and_show_comm_graph.\n" << std::endl;
    };

    auto full_scoda_pipeline = [&] () { /* Verified. */
        std::cout << "In lambda: full_scoda_pipeline" << std::endl;
        graph_viewer->init();
        const int comm_iters = ceil((float)max_iterations * (percentage_iterations_on_comm_graph));
        const int full_iters = max_iterations - comm_iters;
        std::cout << "comm_iters: " << comm_iters << ", full_iters: " << full_iters << "\nFrom max_iterations: " << max_iterations << ", and percentage_iterations_on_comm_graph" << percentage_iterations_on_comm_graph << std::endl;

        // call SCoDA, compress.
        graph_viewer->compress(); /* Verify compress works. */
        std::cout << "GV::compress() works, if everything is intact here." << std::endl;

        graph_viewer->iterate_and_periodically_show(comm_iters, true, "FULL_PIPELINE_COMM_LAYOUT_AFTER_ITERATION_PRE_EXPANSION");
        std::cout << "GV::iterate_and_periodically_show() works on compressed layout, if everything is intact here." << std::endl;

        graph_viewer->expand(); // Back to original graph
        std::cout << "GV::expand() works, if everything is intact here." << std::endl;

        graph_viewer->iterate_and_periodically_show(full_iters, false, "FULL_PIPELINE_POST_EXPANSION_FULL_LAYOUT");
        std::cout << "GV::iterate_and_periodically_show works on expanded layout, if everything is intact here." << std::endl;

        std::cout << "Finished lambda: full_scoda_pipeline" << std::endl;
    };

    auto full_with_stacked_compression= [&] () { /* TODO: Verify. */
        std::cout << "In lambda: full_with_stacked_compression" << std::endl;
        graph_viewer->init();
        const int comm_iters = ceil((float)max_iterations * (percentage_iterations_on_comm_graph));
        const int full_iters = max_iterations - comm_iters;
        std::cout << "comm_iters: " << comm_iters << ", full_iters: " << full_iters << "\nFrom max_iterations: " << max_iterations << ", and percentage_iterations_on_comm_graph" << percentage_iterations_on_comm_graph << std::endl;

        // call SCoDA, compress.
        graph_viewer->compress();
        std::cout << "GV::compress() works, if everything is intact here." << std::endl;
        
        std::cout << "Testing stacked call to compress" << std::endl;
        graph_viewer->compress(); // TODO: BUG HERE, we ended up running SCoDA on the original_dg twice.  The stack is not working properly.
        std::cout << "Finished stacked compress call" << std::endl;

        graph_viewer->iterate_and_periodically_show(comm_iters, true, "FULL_WITH_STACKED_MOST_COMPRESSED_COMM");
        std::cout << "GV::iterate_and_periodically_show() works on most compressed layout, if everything is intact here." << std::endl;

        graph_viewer->expand(); // Back to original graph
        
        graph_viewer->iterate_and_periodically_show(comm_iters, false, "FULL_WITH_STACKED_INTERMEDIATE_COMM_AFTER_EXPANSION");
        std::cout << "GV::iterate_and_periodically_show() works on intermediate compressed layout, if everything is intact here." << std::endl;

        graph_viewer->expand();

        graph_viewer->iterate_and_periodically_show(full_iters, false, "FULL_WITH_STACKED_FINAL_LAYOUT");
        std::cout << "GV::iterate_and_periodically_show works on expanded layout, if everything is intact here." << std::endl;

        std::cout << "Finished lambda: full_with_stacked_compression" << std::endl;
    };

	if(pipeline_to_use == "original") like_original();
	else if(pipeline_to_use == "single") full_scoda_pipeline();
	else if (pipeline_to_use == "stacked") {
		std::cout << "WARNING: The commandline argument 'percentage_comm_iters is currently handled incorrectly for this pipeline.  You can still use it as is, just be aware of the way iterations are distributed." << std::endl;
		full_with_stacked_compression();
	}
	else { /* Unrecognized option. */
		std::cout << "ERROR: Unrecognized pipeline specifier: " << pipeline_to_use << std::endl;
		return EXIT_FAILURE;
	}

    return EXIT_SUCCESS;
}
