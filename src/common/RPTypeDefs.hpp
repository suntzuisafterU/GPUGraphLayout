#ifndef RPTypeDefs_hpp
#define RPTypeDefs_hpp

#include <unordered_map>

namespace RPGraph {
// Type to represent node IDs.
// NOTE: we limit to 4,294,967,296 nodes through uint32_t.
typedef uint32_t contiguous_nid_t;

// Type to represent community ids.
typedef uint32_t comm_id_t;

// Container for mapping contiguous_nid_t to comm_ids
typedef std::unordered_map<RPGraph::contiguous_nid_t, RPGraph::comm_id_t> nid_comm_map_t;

  namespace Enums {
    // Method to use to display.
    enum OutputMethod { PNG_WRITER };

    // Community algos available.
    enum CommAlgo { SCoDA_ENUM };
  } // namespace Enums

} // namespace RPGraph

#endif // RPTypeDefs_hpp 
