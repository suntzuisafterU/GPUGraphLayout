#ifndef RPGraphLayoutUtils_hpp
#define RPGraphLayoutUtils_hpp

#include <fstream>

#include "../../common/RPGraphLayout.hpp"
#include "../../../lib/pngwriter/src/pngwriter.h"


namespace RPGraph {
    void writeToPNG(RPGraph::GraphLayout* layout, const int image_w, const int image_h, std::string path);
    void writeToCSV(RPGraph::GraphLayout* layout, std::string path);
    // void writeToBin(RPGraph::GraphLayout* layout, std::string path);
} // namespace RPGraph

#endif // RPGraphLayoutUtils_hpp