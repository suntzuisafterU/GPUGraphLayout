#ifndef RPDangerousTypeDefs_hpp
#define RPDangerousTypeDefs_hpp

#include "RPTypeDefs.hpp"

namespace RPGraph {

/// See definition and explaination in RPTypeDefs.hpp
#ifdef __WILL_NOT_COMPILE__USE_FOR_STATIC_TYPE_CHECKING__
	struct danger_nid_tag {}; 
	using dangerous_nid_t = uint_id<danger_nid_tag>; /// IMPORTANT: For identifiying when a node id corresponds to the external dataset node id (OR to a previous graphs community node in SCoDA)
#else
	typedef uint32_t dangerous_nid_t;
#endif

} // namespace RPGraph

#endif // RPDangerousTypeDefs_hpp