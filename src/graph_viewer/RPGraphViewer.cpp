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

            GraphViewer::~GraphViewer() {
				delete original_dg;
				for (auto& ptr : hyper_edges) {
					delete ptr;
					ptr = nullptr;
				}
				hyper_edges.clear();

				for (auto& ptr : __old_hyper_edges) {
					delete ptr;
					ptr = nullptr;
				}
				__old_hyper_edges.clear();
			}

            void GraphViewer::init() {
                // Read source file and create UGraph.
				UGraph* graph_ptr = new UGraph(this->edgelist_path);
				// Store pointer in DerivedGraph that will manage it.
				this-> original_dg = new DerivedGraph(graph_ptr);
            }

            void GraphViewer::show(int iteration, const char* explain) {
                std::cout << "Showing... ";
                std::string png_path(this->out_path);
                std::string csv_path(this->out_path);
                png_path.append("/").append(explain).append(this->out_file_prefix).append(std::to_string(iteration)).append(".").append(this->out_format);
                csv_path.append("/").append(explain).append(this->out_file_prefix).append(std::to_string(iteration)).append(".").append("csv");
                // png_path.append(std::to_string(iteration)).append(".").append(this->out_format); /* Use this if you have errors with the line above for some reason. */

                if (this->out_format == "png" ) writeToPNG(this->get_current_layout(), this->image_w, this->image_h, png_path);
                writeToCSV(this->get_current_layout(), csv_path); /* Always write a CSV, sometimes don't write a png. */
                std::cout << "done." << std::endl;
            }

            void GraphViewer::iterate_on_layout(int num_iters, bool randomize) {
                // Create the GraphLayout and ForceAtlas2 objects.
                GraphLayout* current_layout = get_current_layout(); // TODO: IS THIS HOW WE WANT TO DO THIS?? I don't know, whats wrong with it?
                RPGraph::ForceAtlas2* fa2;
                #ifdef __NVCC__
                if(cuda_requested)
                    // GPU FA2
                    fa2 = new RPGraph::CUDAForceAtlas2(*current_layout, approximate,
                                                    strong_gravity, gravity, scale, randomize, use_linlog);
                else
                #endif
                    fa2 = new RPGraph::CPUForceAtlas2(*current_layout, approximate,
                                                    strong_gravity, gravity, scale, randomize, use_linlog);

                const int print_period = ceil((float)num_iters*0.05);
                /**
                * Initial layout will be produced from community graph.
                */
                for (int iteration = 1; iteration < num_iters; ++iteration)
                {
                    fa2->doStep();
                    if (iteration % print_period == 0) {
                        printf("Starting iteration %d (%.2f%%).\n", iteration, 100*(float)iteration/num_iters);
                    }
                }

                fa2->sync_layout(); /* Copies memory from GPU back to CPU, updates the coordinates in GraphLayout. */

                delete fa2; // Cleanup.
                };

            /**
             *  Mimics behaviour of original graph_viewer, but handles files a bit differently.
             */
            void GraphViewer::iterate_and_periodically_show() {
                // Create the GraphLayout and ForceAtlas2 objects.
                GraphLayout* current_layout = get_current_layout(); // TODO: IS THIS HOW WE WANT TO DO THIS??
                RPGraph::ForceAtlas2* fa2;

                bool randomize = true;

                #ifdef __NVCC__
                if(cuda_requested)
                    // GPU FA2
                    fa2 = new RPGraph::CUDAForceAtlas2(*current_layout, approximate,
                                                    strong_gravity, gravity, scale, randomize, use_linlog);
                else
                #endif
                    fa2 = new RPGraph::CPUForceAtlas2(*current_layout, approximate, /// `*current_layout` simply passes by reference.
                                                    strong_gravity, gravity, scale, randomize, use_linlog);

                const int snap_period = ceil((float)max_iterations/num_screenshots);
                const int print_period = ceil((float)max_iterations*0.05);

                for (int iteration = 1; iteration <= max_iterations; ++iteration)
                {
                    // Show starting layout.
                    fa2->sync_layout();
                    show(iteration, "OLDSTYLE_LAYOUT_LOOP_INITIAL_LAYOUT");
                    fa2->doStep();
                    // If we need to, write the result to a png
                    if (num_screenshots > 0 && (iteration % snap_period == 0 || iteration == max_iterations))
                    {
                        fa2->sync_layout();
                        show(iteration, "OLDSTYLE_LAYOUT_LOOP");

                        printf("done.\n");
                    }

                    // Else we print (if we need to)
                    else if (iteration % print_period == 0)
                    {
                        printf("Starting iteration %d (%.2f%%).\n", iteration, 100*(float)iteration/max_iterations);
                    }
                }

                delete fa2;
            }


			GraphLayout* GraphViewer::get_current_layout() {
                // If no hyper edges have been made, then the original graph is the current graph.
                if(this->hyper_edges.size() == 0) {
					return this->original_dg->get_layout();
                } else {
                    // If a hyper edge has been made, then the current source is a comm graph.
                    DerivedGraphHyperEdge* dghe = get_current_hyper_edge();
					return dghe->result_dg->get_layout();
                }
			}

			GraphLayout* GraphViewer::get_previous_layout() {
				if (this->hyper_edges.size() == 0) {
					throw "ERROR: Trying to expand without a previous layout!";
				}
				else {
					DerivedGraphHyperEdge* dghe = get_current_hyper_edge();
					return dghe->source_dg->get_layout();
				}
			}

			const nid_comm_map_t& GraphViewer::get_current_comm_map() {
				if (hyper_edges.size() == 0) {
					throw "ERROR: Trying to access a community map before any have been created. (or when the last valid one was discarded)";
				}
				DerivedGraphHyperEdge* dghe = get_current_hyper_edge();
				return dghe->nid_comm_map;
			}

			UGraph* GraphViewer::get_current_source_graph() {
                // If no hyper edges have been made, then the original graph is the current graph.
                if(this->hyper_edges.size() == 0) {
					return this->original_dg->get_graph();
                } else {
                    // If a hyper edge has been made, then the current source is a comm graph.
                    DerivedGraphHyperEdge* dghe = get_current_hyper_edge();
                    return dghe->source_dg->get_graph();
                }
			}

			UGraph* GraphViewer::get_current_result_graph() {
				DerivedGraphHyperEdge* dghe = get_current_hyper_edge();
				return dghe->result_dg->get_graph();
			}

            DerivedGraph* GraphViewer::get_current_source_derived_graph() {
                // If no hyper edges have been made, then the original graph is the current graph.
                if(this->hyper_edges.size() == 0) {
					return this->original_dg;
                } else {
                    // If a hyper edge has been made, then the current source is a comm graph.
                    DerivedGraphHyperEdge* dghe = get_current_hyper_edge();
                    return dghe->source_dg;
                }
            }

            void GraphViewer::compress() {
                std::cout << "Compressing with SCoDA..." << std::endl;
                // Create new comm_map and graph, add each to container.
				UGraph* source_graph = get_current_source_graph(); // TODO: BUG: This does not return the correct value. It returns an invalid UG object.
				// TODO: Will have to create a container for all the maps, reports, etc.
				nid_comm_map_t nid_comm_map; /**< Map is used since node_ids are not necessarily sequentially complete. Stack allocation. */
				// Can we use move semantics to deal with this?
				UGraph* comm_graph = new UGraph(); // Initialize empty comm_graph for scoda to fill. This probably has to be a pointer.  Will probably just have to use a bunch of poiters. We can free one graph and one map every time we delete a hyper edge....
                // run CommunityAlgo
                // TODO: Implement move assignment for SCoDA?? Or does this get derived automatically?
                SCoDA_Report scoda_report = this->comm_algo.compute_partition(*source_graph, *comm_graph, nid_comm_map); /**< Currently the streaming algorithm is required to also initialize any UGraph datastructures that are required. */

				// TEMP: Print scoda report.
				std::cout << scoda_report;

                // Add results to containers.
				HyperEdgeReports hyper_edge_reports{ scoda_report }; // TODO: DANGER::: This is dependent on ordering!

				// Reference to original source DG.
                DerivedGraph* source_derived_graph = get_current_source_derived_graph();
				// Create new DG, DGHE will manage this as the result_dg.
				DerivedGraph* result_derived_graph = new RPGraph::DerivedGraph(comm_graph);
				hyper_edges.push_back(new DerivedGraphHyperEdge(
					source_derived_graph, 
					nid_comm_map, // Map passed by value.
					result_derived_graph,
					hyper_edge_reports)); // reports passed by value.
                std::cout << "Finished compressing." << std::endl;
            }

            /**
            * Expansion:
            *   Use setCoordinates(node_id, coordinate(x,y)); to expand the community layout to a full graph layout.
            */
            void GraphViewer::expand() {
                std::cout << "Expanding community layout back into source layout." << std::endl;
				// Check for expandability.
				if (hyper_edges.size() == 0) throw "Error: No hyper edges to expand.";

				const nid_comm_map_t& nid_comm_map = get_current_comm_map();
				const GraphLayout* comm_layout = get_current_layout();
				// TODO: This portion is easy to screw up.
				// TODO: Technically NOT SAFE since we are storing the actual nid_comm_map inside of the hyper edge.
				GraphLayout* full_layout = get_previous_layout();

                for (const auto& nid_commid_pair : nid_comm_map) {
					// The community graph is not complete.  We must check for community membership first.
                    contiguous_nid_t node = nid_commid_pair.first; // node is contiguous to the full_layouts associated graph.
                    comm_id_t comm = nid_commid_pair.second;
					Coordinate comm_coordinate = comm_layout->getCoordinateFromCommNode(comm);
                    full_layout->setCoordinates(node, comm_coordinate); /**< Set the nodes id to be that of it's community. */
                }
				_discard_hyper_edge();
                std::cout << "Finished expanding." << std::endl;
            }

} // namespace RPGraph
