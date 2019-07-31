#ifndef RPGraphLayoutRead_hpp
#define RPGraphLayoutRead_hpp

#include "../../common/RPGraphLayout.hpp"

#include <string>

namespace RPGraph {
    void readFromCSV(RPGraph::GraphLayout& layout, std::string layout_path);
} // namespace RPGraph

#endif // RPGraphLayoutRead_hpp