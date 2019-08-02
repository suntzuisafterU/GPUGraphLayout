#ifndef RPTypeDefs_hpp
#define RPTypeDefs_hpp

#include "RPStaticOnlyTypedefHelper.hpp"
#include "unordered_map"

namespace RPGraph {

// TODO: Define a type for tracking data such as counts, degrees, etc.
// TODO: Make sure that we have safety checks for unsigned integer overflow.

/**
 * If C++ had a basic implementation of compile time `strong_type_def` then the following #ifdef etc, and the associated `RPStaticOnlyTypedefHelper.hpp` file would not be necessary.
 * Since this is not the case, I am using this hack to coerce compilers and related tools to typecheck this code statically.  
 *   - This works for FUNCTION DEFINITIONS primarily, as well as accessing maps and vectors.
 *   - This does NOT work for any operations involving pointers, malloc, C-style arrays, etc.
 */

/// Uncomment this line to do basic static type checking.
// #define __WILL_NOT_COMPILE__USE_FOR_STATIC_TYPE_CHECKING__
#ifdef __WILL_NOT_COMPILE__USE_FOR_STATIC_TYPE_CHECKING__
	// define some tags to create uniqueness
	struct contig_nid_tag {};
	struct comm_id_tag {};

	// create some type aliases for ease of use
	using contiguous_nid_t = uint_id<contig_nid_tag>;
	using comm_id_t = uint_id<comm_id_tag>;

	// Container for mapping contiguous_nid_t to comm_ids
	typedef std::unordered_map<RPGraph::contiguous_nid_t, RPGraph::comm_id_t> nid_comm_map_t;
#else
	typedef uint32_t contiguous_nid_t;
	typedef uint32_t comm_id_t;

	// Container for mapping contiguous_nid_t to comm_ids
	typedef std::unordered_map<RPGraph::contiguous_nid_t, RPGraph::comm_id_t> nid_comm_map_t;
#endif

	namespace Enums {
		// Method to use to display.
		enum OutputMethod { PNG_WRITER };

		// Community algos available.
		enum CommAlgo { SCoDA_e }; /// Added `_e` to name of enum just for assurance during name lookup resolution.  

		// TODO: Could use an Enum and bitset to define options to GraphViewer without changing the function prototype if options are added. https://www.learncpp.com/cpp-tutorial/3-8a-bit-flags-and-bit-masks/
	} // namespace Enums

} // namespace RPGraph

#endif // RPTypeDefs_hpp 
