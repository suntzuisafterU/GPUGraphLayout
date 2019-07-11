




// TODO: Move much of lambdas etc from graph viewer here.



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

namespace RPGraph {

    using namespace RPGraph::GraphViewer;
    
            GraphViewer(const bool cuda_requested,
                        const int max_iterations,
                        const int num_screenshots,
                        const bool strong_gravity,
                        const float scale,
                        const float gravity,
                        const bool approximate,
                        const bool use_linlog,
                        const float percentage_iterations_on_comm_graph,
                        const char *edgelist_path,
                        const char *out_path,
                        const char *out_file_prefix,
                        std::string out_format = "png",
                        int image_w = 1250,
                        int image_h = 1250 ){

                        }


            void GraphViewer::init(std::string edgelist_path) {
                // TODO: Create a DatasetAdapter to the desired path (once DA is ready)
                RPGraph::UGraph full_graph(edgelist_path); // Initialize full_graph from provided path.
            }

            void GraphViewer::show() {
                    // If we need to, write the result to a png
                    if (num_screenshots > 0 && (iteration % snap_period == 0 || iteration == max_iterations))
                    {
                        produceOutput(iteration); // TODO: Refactor
                    }
                // Display according to selected method.
                auto produceOutput = [&](int iteration) {
                    /**
                    * Reverted to older version after multiple issues with the line intended to extract the basename of the network
                    */
                    std::string op(out_path);
                    op.append("/").append(out_file_prefix).append(std::to_string(iteration)).append(".").append(out_format);
                    printf("Starting iteration %d (%.2f%%), writing %s...", iteration, 100 * (float)iteration / max_iterations, out_format.c_str());
                    fflush(stdout);
                    fa2->sync_layout();

                    // if (out_format == "png")
                        writeToPNG(current_layout, image_w, image_h, op);

                    printf("done.\n");
                };
            }

            void GraphViewer::layout(int num_iters) {
                // Create the GraphLayout and ForceAtlas2 objects.
                RPGraph::GraphLayout current_layout(comm_graph); /* Produce initial layout from comm_graph. */
                RPGraph::ForceAtlas2* fa2; // Could be CPU or GPU object.
                bool randomize = true;
                #ifdef __NVCC__
                if(cuda_requested)
                    // GPU FA2
                    fa2 = new RPGraph::CUDAForceAtlas2(comm_layout, approximate,
                                                    strong_gravity, gravity, scale, randomize, use_linlog);
                else
                #endif
                    fa2 = new RPGraph::CPUForceAtlas2(comm_layout, approximate,
                                                    strong_gravity, gravity, scale, randomize, use_linlog);

                const int snap_period = ceil((float)max_iterations/num_screenshots);
                const int print_period = ceil((float)max_iterations*0.05);

                /**
                * Initial layout will be produced from community graph.
                */
                for (int iteration = 1; iteration < num_iters; ++iteration) // TODO: Should GraphViewer keep track of overall iterations??
                {
                    fa2->doStep();
                    if (iteration % print_period == 0) {
                        printf("Starting iteration %d (%.2f%%).\n", iteration, 100*(float)iteration/max_iterations);
                    }
                }

                delete fa2;
                };

            }

            void GraphViewer::compress() {
                // Create new comm_map and graph, add each to container.
                RPGraph::UGraph& original_graph = this->derived_graphs_and_maps.size() == 0 ? this->very_first_graph : this->derived_graphs_and_maps.last_item_or_whatever().first;
                std::unordered_map<RPGraph::nid_t, RPGraph::nid_t> nid_comm_map; /**< Map is used since node_ids are not necessarily sequentially complete. Stack allocation. */
                RPGraph::UGraph comm_graph;                // Initialize empty comm_graph for scoda to fill.
                // run CommunityAlgo
                // TODO: Implement move assignment for SCoDA?? Or does this get derived automatically?
                RPGraph::SCoDA_Results = this->comm_algo.compute_partition(original_graph, comm_graph, nid_comm_map); /**< Currently the streaming algorithm is required to also initialize any UGraph datastructures that are required. */
                // Add results to containers.
            }

            /**
            * Expansion:
            *   Use setCoordinates(node_id, coordinate(x,y)); to expand the community layout to a full graph layout.
            */
            void GraphViewer::expand() {
            nid_comm_map_t nid_comm_map = this->derived_graphs_and_maps.last_item_or_whatever().nid_comm_map; // TODO: Maybe need arrow
            // TODO: How should we set layout to expand into?
                for (const auto& nid_commid_pair : nid_comm_map) {
                    RPGraph::contiguous_nid_t node = nid_commid_pair.first;
                    RPGraph::contiguous_nid_t comm = nid_commid_pair.second;
                    RPGraph::Coordinate comm_coordinate = comm_layout.getCoordinate(comm_graph.node_map[comm]);
                    // TODO: Is it possible for a node to not have a community in the graph??? Probably yes. Does not seem to be an issue.
                    full_layout.setCoordinates(node, comm_coordinate); /**< Set the nodes id to be that of it's community. */
                }
            }

} // namespace RPGraph