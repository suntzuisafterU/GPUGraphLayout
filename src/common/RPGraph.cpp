/*
 ==============================================================================

 RPGraph.cpp
 Copyright © 2016, 2017, 2018  G. Brinkmann

 This file is part of graph_viewer.

 graph_viewer is free software: you can redistribute it and/or modify
 it under the terms of version 3 of the GNU Affero General Public License as
 published by the Free Software Foundation.

 graph_viewer is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Affero General Public License for more details.

 You should have received a copy of the GNU Affero General Public License
 along with graph_viewer.  If not, see <https://www.gnu.org/licenses/>.

 ==============================================================================
*/


#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "RPGraph.hpp"

#include <iostream> // TODO: Temp, debugging.

namespace RPGraph
{
    Graph::~Graph() = default;

	UGraph::UGraph() {
        std::cout<< "in UGraph() " << std::endl;
        node_count = 0;
        edge_count = 0;
    }

    UGraph::UGraph(std::string edgelist_path) {
        std::cout<< "in UGraph::UGraph(std::string edgelist_path) " << std::endl;
        node_count = 0;
        edge_count = 0;
        this->read_edgelist_file(edgelist_path);
    }

    UGraph::~UGraph() = default;

    /**
     * Was factored out of constructor and correctness verified.
     * Moved code to scoda to be part of the streaming initialization.
     */
    void UGraph::read_edgelist_file(std::string edgelist_path) {
        std::fstream edgelist_file(edgelist_path, std::ifstream::in);

    // original loop
        std::string line;
        while(std::getline(edgelist_file, line))
        {
            // Skip any comments
            if(line[0] == '#') continue;
			if(line[0] == '%') continue;

            // Read source and target from file
            dangerous_nid_t s, t;
            std::istringstream(line) >> s >> t;

            // if(s != t and !has_edge(s, t)) add_edge(s, t); // Original behaviour of graph_viewer was to ignore duplicate edges. // TODO: Decide how to handle duplicate/weighted edges.
            add_edge_public(s, t);
        }

        edgelist_file.close();
    }

	const std::unordered_map <contiguous_nid_t, uint32_t> UGraph::get_degrees() const {
		/// Allow const access to degrees map. 
		return degrees;
	}

	void UGraph::add_edge_public(dangerous_nid_t s, dangerous_nid_t t) {
        // if(has_edge(s, t) or s == t) return; // Allow weighted edges as duplicates
        if(!has_node_private(s)) add_node_private(s);
        if(!has_node_private(t)) add_node_private(t);
        contiguous_nid_t mapped_s = external_to_contig[s];
        contiguous_nid_t mapped_t = external_to_contig[t];

		add_edge_private(mapped_s, mapped_t);
	}

    void UGraph::add_edge_private(contiguous_nid_t s, contiguous_nid_t t)
    {
        // Insert edge into adjacency_list
        adjacency_list[std::min(s, t)].push_back(std::max(s, t));
        degrees[s] += 1;
        degrees[t] += 1;
        edge_count++;
    }

	bool UGraph::has_edge_public(dangerous_nid_t s, dangerous_nid_t t) const {
        if(!has_node_private(s) || !has_node_private(t)) return false;

		/// If we get here then the maps must contain s and t.  If this fails it is an internal logic error with the UGraph design.
		contiguous_nid_t mapped_s = external_to_contig.at(s);
		contiguous_nid_t mapped_t = external_to_contig.at(t);
		return has_edge_private(mapped_s, mapped_t);
	}

    bool UGraph::has_edge_private(contiguous_nid_t nid1, contiguous_nid_t nid2) const {
		contiguous_nid_t s = std::min(nid1, nid2);
		contiguous_nid_t t = std::max(nid1, nid2);

		if (adjacency_list.count(s) == 0) return false; /// Safety, check membership in adjacency_list.
        if (adjacency_list.at(s).size() == 0) return false;

        std::vector<contiguous_nid_t> neighbors = adjacency_list.at(s);
        if(std::find(neighbors.begin(), neighbors.end(), t) == neighbors.end()) // TODO: Carfully read this and check it for correctness.
            return false;
        else
            return true;
    }

	bool UGraph::has_node_private(dangerous_nid_t nid) const {
		return external_to_contig.count(nid) > 0; /// Simple membership check;
	}

	void UGraph::add_node_private(dangerous_nid_t nid) {
		if (has_node_private(nid)) throw "ERROR: Trying to add node that exists."; /// This is an internal UGraph error if it occurs.

		external_to_contig[nid] = node_count;
		contig_to_external[node_count] = nid;
		node_count++;
	}


    uint32_t UGraph::num_nodes() const
    {
        return node_count;
    }

    uint32_t UGraph::num_edges() const
    {
        return edge_count;
    }

	/**
	 * Returns UNMAPPED DEGREE
	 *
	 * TODO: Make these functions that use internal nids private, or friend + private and define new functions that also map the nids for us...
	 */
    uint32_t UGraph::degree(contiguous_nid_t nid)
    {
        return degrees.at(nid);
    }

    /**
     * Index via MAPPED nids
     */
    const std::vector<contiguous_nid_t> UGraph::neighbors_with_geq_id(contiguous_nid_t nid)
    {
		// TODO: I believe the issue is here, somehow we are allowing someone to overwrite the adjacency_list.
		if (adjacency_list.count(nid) > 0) return adjacency_list.at(nid);
		else {
			// Create and return an empty vector.
			std::vector<contiguous_nid_t> temp; /// Temporary empty vector to indicate that nid does not refer to a source node for any edges.
			return temp;
		}
    }

//	UG_Iter::UG_Iter(const& UGraph) {
//
//	}
//
//	void UG_Iter::operator++() {
//		
//	}
//
//	U_Edge UG_Iter::get_edge() {
//		return 
//	}

    /* Definitions for CSRUGraph */

// CSRUGraph represents an undirected graph using a
// compressed sparse row (CSR) datastructure.
/**
 * IMPORTANT: Ignore this data structure.  Not used in implementation.
 */
    CSRUGraph::CSRUGraph(uint32_t num_nodes, uint32_t num_edges)
    {
        // `edges' is a concatenation of all edgelists (i.e. flattened edge list)
        // `offsets' contains offset (in `edges`) for each nodes' edgelist. (index into flattened edge list)
        // `nid_to_offset` maps nid to index to be used in `offset'  (index into array of mappings id -> offset, then use offset to index into flattened array of edges.)

        // e.g. the edgelist of node with id `nid' starts at
        // edges[offsets[nid_to_offset[nid]]] and ends at edges[offsets[nid_to_offset[nid]] + 1] 
        // (left bound inclusive right bound exclusive) (normal indexing of sublists)

        edge_count = num_edges; // num_edges counts each bi-directional edge once.
        node_count = num_nodes;
        edges =   (contiguous_nid_t *) malloc(sizeof(contiguous_nid_t) * 2 * edge_count); // flattened array
        offsets = (contiguous_nid_t *) malloc(sizeof(contiguous_nid_t) * node_count); // map 
        offset_to_nid = (contiguous_nid_t *) malloc(sizeof(contiguous_nid_t) * node_count);

        // Create a map from original ids to ids used throughout CSRUGraph
        // std::unordered_map<nid_t, nid_t> // secret hidden header variable! 
        nid_to_offset.reserve(node_count); // IMPORTANT: reserves space for at least n elements (dynamic datastructure.)

        first_free_id = 0;
        edges_seen = 0;
    }

    CSRUGraph::~CSRUGraph()
    {
        free(edges);
        free(offsets);
        free(offset_to_nid);
    }

    void CSRUGraph::insert_node(contiguous_nid_t node_id, std::vector<contiguous_nid_t> nbr_ids)
    {
        contiguous_nid_t source_id_old = node_id;
        contiguous_nid_t source_id_new = first_free_id;
        nid_to_offset[source_id_old] = first_free_id;
        offset_to_nid[first_free_id] = source_id_old;
        first_free_id++;

        offsets[source_id_new] = edges_seen;
        for (auto nbr_id : nbr_ids)
        {
            contiguous_nid_t dest_id_old = nbr_id;
            edges[edges_seen] = dest_id_old;
            edges_seen++;
        }
    }

    void CSRUGraph::fix_edge_ids()
    {
        for (eid_t ei = 0; ei < 2*edge_count; ei++)
        {
            edges[ei] = nid_to_offset[edges[ei]];
        }
    }

    uint32_t CSRUGraph::degree(contiguous_nid_t nid)
    {
        // If nid is last element of `offsets'... we prevent out of bounds.
        uint32_t r_bound;
        if (nid < node_count - 1) r_bound = offsets[nid+1];
        else r_bound = edge_count * 2;
        uint32_t l_bound = offsets[nid];
        return (r_bound - l_bound);
    }

    contiguous_nid_t CSRUGraph::nbr_id_for_node(contiguous_nid_t nid, contiguous_nid_t edge_no)
    {
        return edges[offsets[nid] + edge_no];
    }

    uint32_t CSRUGraph::num_nodes() const
    {
        return node_count;
    }

    uint32_t CSRUGraph::num_edges() const
    {
        return edge_count;
    }
};
