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

RPGraph::GraphViewer* parseCommandLine() { // TODO: Belongs in IO utils
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

    int arg_no = 13;
    if(std::string(argv[arg_no]) == "png")
    {
        out_format = "png";
        image_w = std::stoi(argv[arg_no+1]);
        image_h = std::stoi(argv[arg_no+2]);
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

    return new GraphViewer(
        cuda_requested,
        max_iterations,
        num_screenshots,
        strong_gravity,
        scale,
        gravity,
        approximate,
        use_linlog,
        percentage_iterations_on_comm_graph,
        *edgelist_path, // infile
        *out_path, // output directory for images.
        *out_file_prefix, // annotated outfile names
        out_format, // default make png that is 1250x1250
        image_w,
        image_h);
}

int main(int argc, const char **argv)
{
    // For reproducibility.
    // srandom(1234);
    srandom( time(NULL) ); // TODO: Parameterize

    std::unique_ptr<GraphViewer> graph_viewer( parseCommandLine() ); // TODO: Easiest way to initialize??
    // TODO: graph_viewer.init(file_path);
    // TODO: graph_viewer.set_comm_algo(/* scoda */);
    // TODO: graph_viewer.set_layout_method(/* CPU or GPU FA2 */);
    // TODO: graph_viewer.set_display_method(/* png writer */);

    graph_viewer.compress(); // Could be done multiple times.

    // TODO: Replace functionality somehow.
    // if (num_screenshots > 0 && (iteration % snap_period == 0 || iteration == max_iterations))
    // {
    //     produceOutput(iteration); // TODO: Refactor
    // }

    int change_me = 999;
    graph_viewer.iterate_on_layout(change_me);
    int need_to_track_iterations_I_guess = 123987;
    graph_viewer.show(need_to_track_iterations_I_guess);
    // TODO: show_swing(); Or jitter?
    graph_viewer.expand(); // Back to original graph
    graph_viewer.iterate_on_layout(change_me);
    graph_viewer.show(need_to_track_iterations_I_guess);
}
