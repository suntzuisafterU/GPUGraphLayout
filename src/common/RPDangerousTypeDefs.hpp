#ifndef RPDangerousTypeDefs_hpp
#define RPDangerousTypeDefs_hpp

namespace RPGraph {

	// Strong type defs as per: https://blog.demofox.org/2015/02/05/getting-strongly-typed-typedefs-using-phantom-types/
	BOOST_STRONG_TYPEDEF(uint32_t, dangerous_nid_t); /// IMPORTANT: For identifiying when a node id corresponds to the external dataset node id (OR to a previous graphs community node in SCoDA)

} // namespace RPGraph

#endif // RPDangerousTypeDefs_hpp