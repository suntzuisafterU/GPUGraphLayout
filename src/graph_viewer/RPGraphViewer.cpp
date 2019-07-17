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

#include "RPGraphViewer.hpp"

namespace RPGraph {

            // TODO: void GraphViewer::set_comm_algo(RPGraph::CommAlgo comm_algo_enum) {
            //     switch(comm_algo_enum) {
            //         case SCoDA_ENUM:
            //             this->comm_algo = RPGraph::SCoDA();
            //             return; // Or break??
            //     }
            // }

            // TODO: void GraphViewer::init(std::string edgelist_path) {
            //     // TODO: Create a DatasetAdapter to the desired path (once DA is ready)
            //     RPGraph::UGraph full_graph(edgelist_path); // Initialize full_graph from provided path.
            // }

            GraphViewer::~GraphViewer() { }

            void GraphViewer::init() {
                // Read source file and create UGraph.
				RPGraph::UGraph graph1(this->edgelist_path);
				// Push new unique pointer to derived graph onto vetor.
				this->derived_graphs.push_back(std::unique_ptr<RPGraph::DerivedGraph> (new RPGraph::DerivedGraph(graph1)));
            }

            void GraphViewer::show(int iteration) {
                // RPGraph::UGraph& original_graph = this->derived_graphs_and_maps.size() == 0 ? this->very_first_graph : this->derived_graphs_and_maps.last_item_or_whatever().first;
                // TODO: Keep a curent layout reference/pointer. RPGraph::GraphLayout& current_layout = this->derived_graphs_and_maps.size() == 0 ? this
                std::string op(this->out_path);
                // op.append("/").append(this->out_file_prefix).append(std::to_string(iteration)).append(".").append(this->out_format);
                op.append(std::to_string(iteration)).append(".").append(this->out_format);
                printf("Starting iteration %d (%.2f%%), writing %s...", iteration, 100 * (float)iteration / this->max_iterations, out_format.c_str());

                fflush(stdout); // TODO: Why is this necessary?


                // if (out_format == "png")
                    writeToPNG(&this->get_current_layout(), this->image_w, this->image_h, op);

                printf("done.\n"); // TODO: Remove after refactoring.
            }

            void GraphViewer::iterate_on_layout(int num_iters) {
                // Create the GraphLayout and ForceAtlas2 objects.
                RPGraph::GraphLayout& current_layout = get_current_layout(); // TODO: IS THIS HOW WE WANT TO DO THIS??
                RPGraph::ForceAtlas2* fa2;
                bool randomize = true; // TODO: Needs to be parameterized.
                #ifdef __NVCC__
                if(cuda_requested)
                    // GPU FA2
                    fa2 = new RPGraph::CUDAForceAtlas2(current_layout, approximate,
                                                    strong_gravity, gravity, scale, randomize, use_linlog);
                else
                #endif
                    fa2 = new RPGraph::CPUForceAtlas2(current_layout, approximate,
                                                    strong_gravity, gravity, scale, randomize, use_linlog);

                const int print_period = ceil((float)num_iters*0.05);
                /**
                * Initial layout will be produced from community graph.
                */
                for (int iteration = 1; iteration < num_iters; ++iteration) // TODO: Should GraphViewer keep track of overall iterations??
                {
                    fa2->doStep();
                    if (iteration % print_period == 0) {
                        printf("Starting iteration %d (%.2f%%).\n", iteration, 100*(float)iteration/num_iters);
                    }
                }

                fa2->sync_layout();

                delete fa2; // Cleanup.
                };

			RPGraph::GraphLayout& GraphViewer::get_current_layout() {
                // If no hyper edges have been made, then the original graph is the current graph.
                if(this->hyper_edges.size() == 0) {
					return this->derived_graphs.back()->layout;
                } else {
                    // If a hyper edge has been made, then the current source is a comm graph.
                    DerivedGraphHyperEdge& dghe = get_current_hyper_edge();
					return dghe.result_dg->layout;
                }
			}

			RPGraph::GraphLayout& GraphViewer::get_previous_layout() {
				if (this->hyper_edges.size() == 0) {
					throw "ERROR: Trying to expand without a previous layout!";
				}
				else {
					DerivedGraphHyperEdge& dghe = get_current_hyper_edge();
					return dghe.source_dg->layout;
				}
			}

			const RPGraph::nid_comm_map_t& GraphViewer::get_current_comm_map() {
				DerivedGraphHyperEdge& dghe = get_current_hyper_edge();
				return dghe.nid_comm_map;
			}

			RPGraph::UGraph& GraphViewer::get_current_source_graph() {
                // If no hyper edges have been made, then the original graph is the current graph.
                if(this->hyper_edges.size() == 0) {
                    return this->derived_graphs.back()->get_graph();
                } else {
                    // If a hyper edge has been made, then the current source is a comm graph.
                    DerivedGraphHyperEdge& dghe = get_current_hyper_edge();
                    return dghe.source_dg->get_graph();
                }
			}

			RPGraph::UGraph& GraphViewer::get_current_result_graph() {
				DerivedGraphHyperEdge& dghe = get_current_hyper_edge();
				return dghe.result_dg->get_graph();
			}

            RPGraph::DerivedGraph& GraphViewer::get_current_source_derived_graph() {
                // If no hyper edges have been made, then the original graph is the current graph.
                if(this->hyper_edges.size() == 0) {
					return *this->derived_graphs[0];
                } else {
                    // If a hyper edge has been made, then the current source is a comm graph.
                    DerivedGraphHyperEdge& dghe = get_current_hyper_edge();
                    return *dghe.source_dg;
                }
            }

            void GraphViewer::compress() {
                // Create new comm_map and graph, add each to container.
				RPGraph::UGraph& original_graph = get_current_source_graph(); // TODO: BUG: This does not return the correct value. It returns an invalid UG object.
				// TODO: Will have to create a container for all the maps, reports, etc.
                std::unordered_map<RPGraph::contiguous_nid_t, RPGraph::contiguous_nid_t> nid_comm_map; /**< Map is used since node_ids are not necessarily sequentially complete. Stack allocation. */
				// Can we use move semantics to deal with this?
                RPGraph::UGraph comm_graph; // Initialize empty comm_graph for scoda to fill. This probably has to be a pointer.  Will probably just have to use a bunch of poiters. We can free one graph and one map every time we delete a hyper edge....
                // run CommunityAlgo
                // TODO: Implement move assignment for SCoDA?? Or does this get derived automatically?
                RPGraph::SCoDA_Report scoda_report = this->comm_algo.compute_partition(original_graph, comm_graph, nid_comm_map); /**< Currently the streaming algorithm is required to also initialize any UGraph datastructures that are required. */
                // Add results to containers.
				RPGraph::HyperEdgeReports hyper_edge_reports{ scoda_report }; // TODO: DANGER::: This is dependent on ordering!
				// TODO: Will need move semantics.
                RPGraph::DerivedGraph& original_derived_graph = get_current_source_derived_graph();
                hyper_edges.emplace_back( // TODO: Is this a nameless dghe?
					original_derived_graph, 
					nid_comm_map, 
					comm_graph,  // Constructs a DerivedGraph struct.
					hyper_edge_reports); // TODO: This should not have to provide and initialized layout object.
            }

            /**
            * Expansion:
            *   Use setCoordinates(node_id, coordinate(x,y)); to expand the community layout to a full graph layout.
            */
            void GraphViewer::expand() {
				// Check for expandability.
				if (hyper_edges.size() == 0) throw "Error: No hyper edges to expand.";

				const RPGraph::nid_comm_map_t& nid_comm_map = get_current_comm_map();
				const RPGraph::GraphLayout& comm_layout = get_current_layout();
				// TODO: This portion is easy to screw up.
				// TODO: Technically NOT SAFE since we are storing the actual nid_comm_map inside of the hyper edge.
				RPGraph::GraphLayout& full_layout = get_previous_layout();

                for (const auto& nid_commid_pair : nid_comm_map) {
                    RPGraph::contiguous_nid_t node = nid_commid_pair.first;
                    RPGraph::comm_id_t comm = nid_commid_pair.second;
                    RPGraph::Coordinate comm_coordinate = comm_layout.getCoordinate(cast_comm_to_contig(comm));
                    // TODO: Is it possible for a node to not have a community in the graph??? Probably yes. Does not seem to be an issue.
                    full_layout.setCoordinates(node, comm_coordinate); /**< Set the nodes id to be that of it's community. */
                }
				_discard_hyper_edge(); // Will have to pop off the top here to get access to the underlying full_layout.
            }

} // namespace RPGraph
