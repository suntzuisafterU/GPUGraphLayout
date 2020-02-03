#ifndef RPGraphLayoutUtils_hpp
#define RPGraphLayoutUtils_hpp

#include <fstream>
#include <string>

#include "../../../lib/pngwriter/src/pngwriter.h"

#include "../../common/RPGraphLayout.hpp"
#include "../../common/RPGraph.hpp"
#include "../../scoda/scoda.hpp"


namespace RPGraph {
    void writeToPNG(RPGraph::GraphLayout* layout, const int image_w, const int image_h, std::string path);
    void writeToCSV(RPGraph::GraphLayout* layout, std::string path);
    void writeCommunityGraphToEdgelist(RPGraph::UGraph& graph, SCoDA_Report report, std::string path);
    // void writeToBin(RPGraph::GraphLayout* layout, std::string path);
} // namespace RPGraph

#endif // RPGraphLayoutUtils_hpp