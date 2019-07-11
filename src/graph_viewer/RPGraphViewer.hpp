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
#include "../FA2/RPCPUForceAtlas2.hpp"
#include "../scoda/DisjointPartitionAlgo.hpp"
#include "../scoda/scoda.hpp"

#ifdef __NVCC__
#include <cuda_runtime_api.h>
#include "../FA2/RPGPUForceAtlas2.hpp"
#endif // __NVCC__


namespace RPGraph {

    /// Store graph together with associated map.  
    struct DerivedGraph {
        RPGraph::UGraph& comm_graph;
        RPGraph::nid_comm_map_t& nid_comm_map;
        RPGraph::GraphLayout& layout;
        RPGraph::SCoDA_Results& results;
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
            void show(int); /**< Display or print data, depends on output method. */
            void set_comm_algo(RPGraph::CommAlgo);
            // TODO: void set_layout_method(/* CPU or GPU FA2 */);
            // TODO: void set_display_method(/* png writer */);


        private:
            RPGraph::UGraph very_first_graph;
            std::vector< DerivedGraph > derived_graphs_and_maps;
            
            template<class T>
            RPGraph::DisjointPartitionAlgo<T> comm_algo;

            /// Parameters to layout algorithms. TODO: Turn this into a struct or something that lives in one place.
            const bool cuda_requested;
            const int max_iterations;
            const int num_screenshots;
            const bool strong_gravity;
            const float scale;
            const float gravity;
            const bool approximate;
            const bool use_linlog;
            const float percentage_iterations_on_comm_graph;
            const char *edgelist_path;
            const char *out_path;
            const char *out_file_prefix;
            std::string out_format;
            int image_w;
            int image_h;
    };
} // namespace RPGraph

#endif // RPGraphViewer_hpp
