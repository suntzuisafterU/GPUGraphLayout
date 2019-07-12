#include "DatasetAdapter.hpp"

namespace RPGraph
{

    // using coro = boost::coroutines; // TODO: Is this in the right place?
    // typedef coro::generator< std::pair<contiguous_nid_t, contiguous_nid_t> > edge_generator; // TODO: Use after testing opeartor()() overloading

    DatasetAdapter::DatasetAdapter(std::string file_path) {
        this->file_path = file_path;
    }

    DatasetAdapter::RPFileIterator DatasetAdapter::get_file_iterator() {
        return DatasetAdapter::RPFileIterator(this->file_path);
    }

    DatasetAdapter::RPFileIterator(std::string file_path) {
        this->file_stream = std::istream(file_path);
    }

    DatasetAdapter::RPFileIterator bool() const {
        return this->file_stream; // TODO: Does this work???? Or do we have to check for EOF or something?
    }

    // TODO: Can we just return this as a string instead??
    std::pair<contiguous_nid_t, contiguous_nid_t> DatasetAdapter::RPFileIterator operator()() {
        nid_t src, dst;
        this->file_stream >> src >> dst; // this-> is Iterator

        // If nodes not already mapped, then add them to the map. Otherwise read map.
        contiguous_nid_t c_src = !membership(src) ? add_to_maps(src) : translate(src); // TODO: Scoping??
        contiguous_nid_t c_dst = !membership(dst) ? add_to_maps(dst) : translate(dst);

        // Return mapped node ids.
        return std::pair(src, dst);
    }

    bool DatasetAdapter::RPFileIterator membership(nid_t node_id) {
        return this->nid_to_contig.count(node_id);
    }

    contiguous_nid_t DatasetAdapter::RPFileIterator add_to_maps(nid_t node_id) {
        this->nid_to_contig[node_id] = this->current_idx;
        this->contig_to_nid[this->current_idx] = node_id;
        return this->current_idx++; // Return and then increment. Optimization.
    }

    nid_t DatasetAdapter::translate(const contiguous_nid_t node_id) {
        // TODO: I don't think this compiles.
        return this->contig_to_nid[node_id];
    }

    contiguous_nid_t DatasetAdapter::translate(const nid_t node_id) {
        return this->nid_to_contig[node_id];
    }

} // namespace RPGraph