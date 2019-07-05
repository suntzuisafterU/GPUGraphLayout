#ifndef stress_hpp
#define stress_hpp

#include "../common/RPCommon.hpp" // Coordinate
#include "../common/RPGraphLayout.hpp" // GraphLayout

namespace LayoutEval {

float stress(RPGraph::GraphLayout* layout, int L);

} // LayoutEval

#endif // stress_hpp