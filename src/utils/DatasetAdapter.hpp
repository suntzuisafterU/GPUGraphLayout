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

    // typedef for node ids.
    // TODO: What is the range of this unsigned integer type? Implement safety checks in the appropriate places to avoid uint overflow.
    typedef uint32_t nid_t;

    class DatasetAdapter {
        public:
            /**
             * Create an adapter that is associated with a single file.  
             * Maintains an internal mapping to be used for IO operations.
             */
            DatasetAdapter(std::string file_path);
            DatasetAdapter(const DatasetAdapter& other) = delete;
            DatasetAdapter & operator=(const DatasetAdapter& other) = delete;

            // TODO: Instead of returning an iterator, try turning this class into a functor.
            inline bool membership(nid_t node_id); // TODO: What is this for again?
            operator bool() const;
			std::istream& operator>> (std::istream& stream, DatasetAdapter& da);
            // operator ()(); // Where the magic happens.  Returns data from file as iterator....

            // class RPFileIterator : RPIterator; // TODO: Is this inheritence syntax correct?
            /// DatasetAdapter is unfinished until the iterator is used.
            // RPFileIterator get_file_iterator(); // Only return this once, subsequent calls should fail.
        private:
            std::string file_path;

            std::unordered_map<nid_t, contiguous_nid_t> nid_to_contig; /// For input.
            std::unordered_map<contiguous_nid_t, nid_t> contig_to_nid; /// For output. TODO: Swap with vector after testing.

		inline contiguous_nid_t DatasetAdapter::translate_nid(const nid_t node_id) {
			return this->nid_to_contig[node_id];
		}

		inline nid_t DatasetAdapter::translate_contig(const contiguous_nid_t node_id) {
			return this->contig_to_nid[node_id];
		}
    };

    /**
     * Don't allow the use of RPFileIterator without DatasetAdapter.
     * 
     * TODO: Might use the CRTP to implement compile time polymorphism. https://en.wikipedia.org/wiki/Template_metaprogramming#Static_polymorphism
     *       This is used in the boost.iterator library.
     */
    //class DatasetAdapter::RPFileIterator : RPIterator { // TODO: Is this inheritance pattern valid?
    //    public:
    //        inline bool membership(nid_t node_id);
    //        operator bool() const; // TODO: safe-bool idiom: https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Safe_bool
    //        std::pair<contiguous_nid_t, contiguous_nid_t> operator()(); // Value returned when iterator is used.
    //    private:
    //        RPFileIterator(std::string file_path);

    //        RPFileIterator(const RPFileIterator& other) = delete;
    //        RPFileIterator & operator=(const RPFileIterator& other) = delete;

    //        ~RPFileIterator(); // Close file
    //    // private:
    //        contiguous_nid_t current_idx;
    //        std::fstream file_stream;

    //};

} // namespace RPGraph

#endif // DatasetAdapter_hpp
