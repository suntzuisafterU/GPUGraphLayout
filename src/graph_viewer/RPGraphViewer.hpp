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
		DerivedGraph::DerivedGraph(RPGraph::UGraph& ug) : graph{ ug } { // How do we take this and move the stuff in?
			if (ug.num_nodes() == 0) throw "Error, UGraph not iniatialized.";  // Die
			// IF we initialize a layout here, the ug must be full.
			// Need move semantics
			this->layout = RPGraph::GraphLayout(ug);
		}

		DerivedGraph(const DerivedGraph& other) = delete;
		DerivedGraph operator= (const DerivedGraph& other) = delete;

        const RPGraph::UGraph graph;
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
        const DerivedGraph& source_dg;
        const RPGraph::nid_comm_map_t nid_comm_map; // Map associates these 2 graphs.  This is the only place that the map lives? May have to implement move semantics.
        const DerivedGraph& result_dg;

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
                        image_h{image_h} { };

            GraphViewer() = delete;

            GraphViewer(const GraphViewer& other) = delete;
            GraphViewer & operator=(const GraphViewer& other) = delete;
            // TODO: void init(std::string file_path);
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

            RPGraph::GraphLayout& get_current_layout(); /// Gets the layout from the HyperEdge on the top of the stack.
			RPGraph::nid_comm_map_t& get_current_comm_map();
			RPGraph::UGraph& get_current_source_graph();
			RPGraph::UGraph& get_current_result_graph(); // Could be null?
			void add_hyper_edge(DerivedGraphHyperEdge dghe);

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
    };
} // namespace RPGraph

#endif // RPGraphViewer_hpp
