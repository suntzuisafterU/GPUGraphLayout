#include "DatasetAdapter.hpp"

namespace RPGraph
{

    // using coro = boost::coroutines; // TODO: Is this in the right place?
    // typedef coro::generator< std::pair<contiguous_nid_t, contiguous_nid_t> > edge_generator; // TODO: Use after testing opeartor()() overloading

    DatasetAdapter::DatasetAdapter(std::string file_path) {
        this->file_path = file_path;
    }

	/**
	 * Usage: 
	 *   contiguous node_id src, dst;
	 *   da >> src >> dst; // Reads nid_t from file and inserts mapped values into src and dst.
	 */
	std::istream& operator>> (std::istream& stream, DatasetAdapter& da) {
		nid_t node_id;
		da.file_stream >> node_id; // TODO: ERROR, no file_stream member in dataset adapter!

		// NOTE: The internal class does not have special access to this pointer.  Does it have access to functions?j
		// If nodes not already mapped, then add them to the map. Otherwise read map.
		contiguous_nid_t contig_node_id = !membership(node_id) ? add_to_maps(node_id) : translate_nidt(node_id); // TODO: Scoping??

		stream>> node_id; // TODO: How do we insert something into the input stream??
		return stream;
	}



    // DatasetAdapter::RPFileIterator DatasetAdapter::get_file_iterator() {
    //     return DatasetAdapter::RPFileIterator(this->file_path);
    // }

    // DatasetAdapter::RPFileIterator(std::string file_path) {
    //     this->file_stream = std::istream(file_path);
    // }

    // DatasetAdapter::RPFileIterator bool() const {
    //     return this->file_stream; // TODO: Does this work???? Or do we have to check for EOF or something?
    // }

    // // TODO: Can we just return this as a string instead??
    // std::pair<contiguous_nid_t, contiguous_nid_t> DatasetAdapter::RPFileIterator operator()() {
    //     nid_t src, dst;
    //     this->file_stream >> src >> dst; // this-> is Iterator

	//     // NOTE: The internal class does not have special access to this pointer.  Does it have access to functions?j
    //     // If nodes not already mapped, then add them to the map. Otherwise read map.
    //     contiguous_nid_t c_src = !membership(src) ? add_to_maps(src) : translate_nidt(src); // TODO: Scoping??
    //     contiguous_nid_t c_dst = !membership(dst) ? add_to_maps(dst) : translate_nidt(dst);

    //     // Return mapped node ids.
    //     return std::pair(src, dst);
    // }

    // contiguous_nid_t DatasetAdapter::RPFileIterator add_to_maps(nid_t node_id) {
    //     this->nid_to_contig[node_id] = this->current_idx;
    //     this->contig_to_nid[this->current_idx] = node_id;
    //     return this->current_idx++; // Return and then increment. Optimization.
    // }

} // namespace RPGraph