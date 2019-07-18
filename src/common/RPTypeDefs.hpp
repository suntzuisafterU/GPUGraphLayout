#ifndef RPTypeDefs_hpp
#define RPTypeDefs_hpp

#include <unordered_map>
#include <boost/serialization/strong_typedef.hpp>

namespace RPGraph {

// TODO: Define a type for tracking data such as counts, degrees, etc.
// TODO: Make sure that we have safety checks for unsigned integer overflow.

// typedefs do not provide type safety: https://stackoverflow.com/questions/9588009/typesafe-typedef-in-c
// Using boost macro instead: https://www.boost.org/doc/libs/1_49_0/libs/serialization/doc/strong_typedef.html

// Type to represent node IDs.
// NOTE: we limit to 4,294,967,296 nodes through uint32_t.
BOOST_STRONG_TYPEDEF(uint32_t, contiguous_nid_t);

// Type to represent community ids.
BOOST_STRONG_TYPEDEF(uint32_t, comm_id_t);

// Container for mapping contiguous_nid_t to comm_ids
typedef std::unordered_map<RPGraph::contiguous_nid_t, RPGraph::comm_id_t> nid_comm_map_t;

  namespace Enums {
    // Method to use to display.
    enum OutputMethod { PNG_WRITER };

    // Community algos available.
    enum CommAlgo { SCoDA_e }; /// Added `_e` to name of enum just for assurance during name lookup resolution.  

    // TODO: Could use an Enum and bitset to define options to GraphViewer without changing the function prototype if options are added. https://www.learncpp.com/cpp-tutorial/3-8a-bit-flags-and-bit-masks/
  } // namespace Enums

} // namespace RPGraph

#endif // RPTypeDefs_hpp 
