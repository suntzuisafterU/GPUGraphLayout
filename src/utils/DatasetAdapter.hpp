#ifndef DatasetAdapter_hpp
#define DatasetAdapter_hpp

// #include <boost/coroutine/generator.hpp> // TODO: Can't find this file
#include <string>
#include <unordered_map>
#include <vector>
#include <utility>
#include <istream>

#include "RPIterator.hpp"
#include "../common/RPTypeDefs.hpp"


namespace RPGraph {
    class DatasetAdapter {
        public:
            /**
             * Create an adapter that is associated with a single file.  
             * Maintains an internal mapping to be used for IO operations.
             */
            DatasetAdapter(std::string file_path);
            DatasetAdapter(const DatasetAdapter& other) = delete;
            DatasetAdapter & operator=(const DatasetAdapter& other) = delete;

            class RPFileIterator : RPIterator; // TODO: Is this inheritence syntax correct?
            /// DatasetAdapter is unfinished until the iterator is used.
            RPFileIterator get_file_iterator(); // Only return this once, subsequent calls should fail.
        private:
            std::string file_path;

            std::unordered_map<nid_t, contiguous_nid_t> nid_to_contig;
            std::unordered_map<contiguous_nid_t, nid_t> contig_to_nid; // TODO: Swap with vector after testing.

            inline nid_t translate(const contiguous_nid_t node_id);
            inline contiguous_nid_t translate(const nid_t node_id);
    };

    /**
     * Don't allow the use of RPFileIterator without DatasetAdapter.
     */
    class DatasetAdapter::RPFileIterator : RPIterator { // TODO: Is this inheritance pattern valid?
        public:
            inline bool membership(nid_t node_id);
            operator bool() const; // TODO: safe-bool idiom: https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Safe_bool
            std::pair<contiguous_nid_t, contiguous_nid_t> operator()(); // Value returned when iterator is used.
        private:
            RPFileIterator(std::string file_path);

            RPFileIterator(const RPFileIterator& other) = delete;
            RPFileIterator & operator=(const RPFileIterator& other) = delete;

            ~RPFileIterator(); // Close file
        // private:
            contiguous_nid_t current_idx;
            std::fstream file_stream;

    };

} // namespace RPGraph

#endif // DatasetAdapter_hpp