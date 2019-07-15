#ifndef RPGraphViewer_hpp
#define RPGraphViewer_hpp

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <fstream>

#include "../common/RPCommon.hpp"
#include "../common/RPGraph.hpp"
#include "../common/RPGraphLayout.hpp"
#include "../common/RPTypeDefs.hpp"
#include "../utils/IO/RPGraphLayoutUtils.hpp"
// #include "../utils/DatasetAdapter.hpp"
#include "../FA2/RPCPUForceAtlas2.hpp"
#include "../scoda/CommunityAlgo.hpp"
#include "../scoda/DisjointPartitionAlgo.hpp"
#include "../scoda/scoda.hpp"
#include "../layouteval/PairwiseLayoutAnalysis.hpp"
#include "../layouteval/stress.hpp"

#ifdef __NVCC__
#include <cuda_runtime_api.h>
#include "../FA2/RPGPUForceAtlas2.hpp"
#endif // __NVCC__


namespace RPGraph {

    /// Store graph together with associated layout.  
    struct DerivedGraph {
        // DerivedGraph(): graph{}, layout{} { };

        DerivedGraph(RPGraph::UGraph& ug) : graph{ std::move(ug) }, layout{ ug } { // How do we take this and move the stuff in?
            if (ug.num_nodes() == 0) throw "Error, UGraph not iniatialized.";  // Die, TODO: This may be bad practice, and late.  But we are crashing the application so should be fine.
            // IF we survive this constructor, the ug must be full, and the layout must have nodes in it.
            // TODO: Need move semantics for UGraph for this to work.
        };

		// TODO: TEMP: DerivedGraph(const DerivedGraph& other) = delete;
		// TODO: TEMP: DerivedGraph operator= (const DerivedGraph& other) = delete;

        // TODO: Get rid of this.
        DerivedGraph(const DerivedGraph& other): 
                graph{ other.graph }, 
                layout{ other.layout } { };

        DerivedGraph(DerivedGraph&& other): 
                graph{ other.graph },
                layout{ other.layout } {
            // The destructor of `other` should be called when it goes out of scope.
        };

        DerivedGraph& operator= (DerivedGraph&& other) {
            // Self-assignment detection.
            if (&other == this)
                return *this;
            
            // Transfer ownership.
            this->graph = other.graph;
            this->layout = std::move(other.layout);

            return *this;
        };

        RPGraph::UGraph graph; // TODO: Was const
		RPGraph::GraphLayout layout;
        // std::unique_ptr<RPGraph::GraphLayout> layout_ptr; // Unique ptr, then is mutable.
    };

    struct HyperEdgeReports {
        RPGraph::SCoDA_Report scoda_report;
        RPGraph::StressReport stress_report; // Can be null, will be null for large graphs. For small graphs compares source_dg to result_dg.
        RPGraph::PairwiseLayoutReport pairwise_layout_report; // Where the rubber meets the road.
    };
    
    /// Compression via community algo associates 2 graphs with each other. Expansion uses this association as well.
    struct DerivedGraphHyperEdge { // TODO: naming?

        // TODO: first 2 args were const
        DerivedGraphHyperEdge(RPGraph::DerivedGraph& sg, const RPGraph::nid_comm_map_t nid_comm_map, RPGraph::UGraph& rg, HyperEdgeReports& reports) :
                source_dg { std::move(sg) }, 
                nid_comm_map { nid_comm_map },
                result_dg { rg }, // TODO: This
                reports { reports }
                { };

        DerivedGraphHyperEdge(RPGraph::DerivedGraphHyperEdge&& other):
                source_dg{ std::move(other.source_dg) },
                nid_comm_map{ other.nid_comm_map },
                result_dg{ other.result_dg },
                reports{ other.reports } { 
                    // Other DGHE Destructor should be called I believe.
                };

        // TODO: Get rid of this.  Temp.
        DerivedGraphHyperEdge(const RPGraph::DerivedGraphHyperEdge& other):
                source_dg{ other.source_dg },
                nid_comm_map{ other.nid_comm_map },
                result_dg{ other.result_dg },
                reports{ other.reports } { };

        const DerivedGraph& source_dg; // was const, working out bugs
        const RPGraph::nid_comm_map_t nid_comm_map; // was const
        DerivedGraph result_dg; // Not const, we can access the Layout

        HyperEdgeReports& reports; // All reports associated with this step, in either direction. (compression or expansion)
        // TODO: Have a destructor or something that prints the reports? Lol
    };

    class GraphViewer {
        public:
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
                        int image_h = 1250 ) :
                        cuda_requested{cuda_requested},
                        max_iterations{max_iterations},
                        num_screenshots{num_screenshots},
                        strong_gravity{strong_gravity},
                        scale{scale},
                        gravity{gravity},
                        approximate{approximate},
                        use_linlog{use_linlog},
                        percentage_iterations_on_comm_graph{percentage_iterations_on_comm_graph},
                        edgelist_path{edgelist_path},
                        out_path{out_path},
                        out_file_prefix{out_file_prefix},
                        out_format{out_format},
                        image_w{image_w},
                        image_h{image_h} {

                         };

            GraphViewer() = delete;

            GraphViewer(const GraphViewer& other) = delete;
            GraphViewer & operator=(const GraphViewer& other) = delete;
            void init();
            void show(int iter); /**< Display or print data, depends on output method. */
            void iterate_on_layout(int num_iters);

            void compress();
            void expand();
            // TODO: void set_comm_algo(RPGraph::CommAlgo);
            // TODO: void set_layout_method(/* CPU or GPU FA2 */);
            // TODO: void set_display_method(/* png writer */);


        private:
			// TODO: stack could be stack of references to dghes.  Then hold actual dghes in a vector or something.
            std::vector < DerivedGraphHyperEdge > hyper_edges; // TODO: Analysis this datastructure.  Nameing?
			std::vector < DerivedGraphHyperEdge > __old_hyper_edges; // TODO: Temporary until a better solution is discovered.
            RPGraph::SCoDA comm_algo;
            RPGraph::ForceAtlas2* fa2; // TODO: Make some kind of safe pointer or something.

			inline RPGraph::DerivedGraphHyperEdge& get_current_hyper_edge() {
				return hyper_edges.back();
			}

			inline void _discard_hyper_edge() {
				__old_hyper_edges.push_back(hyper_edges.back()); // TODO: Testing
				hyper_edges.pop_back(); // Erases, no return.
			}

			RPGraph::contiguous_nid_t cast_comm_to_contig(RPGraph::comm_id_t comm_id) {
				contiguous_nid_t val = comm_id; // TODO: Get rid of this function.
				return val;
			}

            RPGraph::GraphLayout& get_current_layout(); /// Gets the layout from the HyperEdge on the top of the stack.
			const RPGraph::nid_comm_map_t& get_current_comm_map();
			const RPGraph::UGraph& get_current_source_graph();
			RPGraph::UGraph& get_current_result_graph(); // Could be null?
            RPGraph::DerivedGraph& get_current_source_derived_graph();

            /// Parameters to layout algorithms. TODO: Turn this into a struct or something that lives in one place.
            const bool cuda_requested;
            const int max_iterations;
            const int num_screenshots;
            const bool strong_gravity;
            const float scale;
            const float gravity;
            const bool approximate;
            const bool use_linlog;
            const float percentage_iterations_on_comm_graph; // TODO: Will need different parameters than this.
            const char *edgelist_path;
            const char *out_path;
            const char *out_file_prefix;
            std::string out_format;
            int image_w;
            int image_h;
            RPGraph::DerivedGraph original_graph;
    };
} // namespace RPGraph

#endif // RPGraphViewer_hpp
