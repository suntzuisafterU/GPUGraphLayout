#ifndef RPDangerousTypeDefs_hpp
#define RPDangerousTypeDefs_hpp

#include <boost/serialization/strong_typedef.hpp>

namespace RPGraph {

	// typedefs do not provide type safety: https://stackoverflow.com/questions/9588009/typesafe-typedef-in-c
	// Using boost macro instead: https://www.boost.org/doc/libs/1_49_0/libs/serialization/doc/strong_typedef.html
	BOOST_STRONG_TYPEDEF(uint32_t, dangerous_nid_t); /// IMPORTANT: For identifiying when a node id corresponds to the external dataset node id (OR to a previous graphs community node in SCoDA)

} // namespace RPGraph

#endif // RPDangerousTypeDefs_hpp