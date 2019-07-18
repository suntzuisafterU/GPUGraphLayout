#ifndef RPDangerousTypeDefs_hpp
#define RPDangerousTypeDefs_hpp

#include "RPTypeDefs.hpp"

namespace RPGraph {

	SAFE_TYPEDEF(uint32_t, dangerous_nid_t); /// IMPORTANT: For identifiying when a node id corresponds to the external dataset node id (OR to a previous graphs community node in SCoDA)

} // namespace RPGraph

#endif // RPDangerousTypeDefs_hpp