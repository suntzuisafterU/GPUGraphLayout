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
        fprintf(stderr, "Usage: graph_viewer gpu|cpu max_iterations num_snaps sg|wg scale gravity exact|approximate linlog|regular [percentage_iterations_on_comm_graph] edgelist_path out_path out_file_prefix [png image_w image_h|csv|bin]\n");
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

    int arg_no = 13;
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

    auto compress_and_show_comm_graph = [&] () { /* TODO: Verify. */
        std::cout << "In lambda: compress_and_show_comm_graph " << std::endl;
        graph_viewer->init();

        // call SCoDA, compress.
        graph_viewer->compress(); /* Verify compress works. */
        std::cout << "GV::compress() works, if everything is intact here." << std::endl;

        graph_viewer->iterate_and_periodically_show(max_iterations, /* randomize = */true, "BUGGED_COMPRESS_AND_SHOW_COMM_GRAPH");
        std::cout << "GV::iterate_and_periodically_show() works on internally compressed community graph, if everything is intact here." << std::endl;

        std::cout << "Finished compress_and_show_comm_graph.\n" << std::endl;
    };

    auto full_scoda_pipeline = [&] () { /* TODO: Verify. */
        std::cout << "In lambda: full_scoda_pipeline" << std::endl;
        graph_viewer->init();
        const int comm_iters = ceil((float)max_iterations * (percentage_iterations_on_comm_graph/100));
        const int full_iters = max_iterations - comm_iters;

        // call SCoDA, compress.
        graph_viewer->compress(); /* Verify compress works. */
        std::cout << "GV::compress() works, if everything is intact here." << std::endl;

        graph_viewer->iterate_on_layout(comm_iters, true);
        std::cout << "GV::iterate_on_layout() works on community graph, if everything is intact here." << std::endl;

        graph_viewer->show(comm_iters, "FULL_PIPELINE_COMM_LAYOUT_AFTER_ITERATION_PRE_EXPANSION"); /* TODO: Verify that community graph is laid out correctly. */
        std::cout << "GV::show() works on community graph, if everything is intact here." << std::endl;

        graph_viewer->expand(); // Back to original graph
        std::cout << "GV::expand() works, if everything is intact here." << std::endl;

        graph_viewer->show(comm_iters+1, "FULL_PIPELIN_POST_EXPANSION_FULL_LAYOUT"); // TODO: Verify that this is a valid state to show the layout in.
        std::cout << "GV::show() of initial layout after expansion. " << std::endl;

        graph_viewer->iterate_on_layout(full_iters, false); // false for randomization.
        std::cout << "GV::iterate_on_layout() works after expansion, if everything is intact here." << std::endl;

        graph_viewer->show(full_iters, "FULL_PIPELINE_FINAL_LAYOUT");
        std::cout << "GV::show() works after expansion, if everything is intact here." << std::endl;
    };

    // like_original();

    compress_and_show_comm_graph();

    // TODO: Implement this type of showing only with more explanitory file names, for example initial, and half way, and final.
    // if (num_screenshots > 0 && (iteration % snap_period == 0 || iteration == max_iterations))
    // {
    //     graph_viewer->show(iteration); // TODO: Refactor
    // }

    return EXIT_SUCCESS;
}
