#ifndef RPTypeDefs_hpp
#define RPTypeDefs_hpp

#include <unordered_map>
#include <utilities>

namespace RPGraph {
// Type to represent node IDs.
// NOTE: we limit to 4,294,967,296 nodes through uint32_t.
typedef uint32_t contiguous_nid_t;

// typedef for node ids.
typedef uint32_t nid_t;

// Type to represent community ids.
typedef uint32_t comm_id_t;

// Container for mapping contiguous_nid_t to comm_ids
typedef nid_comm_map_t std::unordered_map<RPGraph::contiguous_nid_t, RPGraph::comm_id_t>;

// Method to use to display.
enum OutputMethod { PNG_WRITER };

} // namespace RPGraph

#endif // RPTypeDefs_hpp 