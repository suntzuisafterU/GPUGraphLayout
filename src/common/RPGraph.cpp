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

    // UGraph::UGraph(const UGraph& other):
    //     node_count{other.node_count},
    //     edge_count{other.edge_count},
    //     degrees{other.degrees},
    //     adjacency_list{other.adjacency_list} { 
    //         std::cout<< "in UGraph::UGraph(const UGraph& other)" << std::endl;

    //     }

    // UGraph& UGraph::operator= (const UGraph& other) {
    //     std::cout<<"in UGraph& UGraph::operator= (const UGraph& other)" << std::endl;
    //     if(&other == this)
    //         return *this;
    //     
    //     node_count = other.node_count;
    //     edge_count = other.edge_count;
    //     degrees = other.degrees;
    //     adjacency_list = other.adjacency_list;

    //     return *this;
    // }

    UGraph::~UGraph() = default;

    /**
     * Was factored out of constructor and correctness verified.
     * Moved code to scoda to be part of the streaming initialization.
     */
    void UGraph::read_edgelist_file(std::string edgelist_path) {
        std::fstream edgelist_file(edgelist_path, std::ifstream::in);

    typedef uint32_t nid_t;

    std::unordered_map<nid_t, contiguous_nid_t> node_map;

    auto has_node_internal = [&](nid_t nid) {
        return node_map.count(nid) > 0;
    };

    // has edge
    // auto has_edge_internal = [&](nid_t s, nid_t t) {
    //     if(has_node_internal(s) || has_node_internal(t)) return true;

    //     contiguous_nid_t s_mapped = node_map[s];
    //     contiguous_nid_t t_mapped = node_map[t];

    //     if(adjacency_list.count(std::min(s_mapped, t_mapped)) == 0) return false;

    //     std::vector<nid_t> neighbors = adjacency_list[std::min(s_mapped, t_mapped)];
    //     if(std::find(neighbors.begin(), neighbors.end(), std::max(s_mapped, t_mapped)) == neighbors.end())
    //         return false;
    //     else
    //         return true;
    // };

    // add node
    auto add_node_internal = [&](nid_t nid) {
        node_map[nid] = node_count;
        node_count++;
    };

    // add edge
    auto add_edge_internal = [&](nid_t s, nid_t t){
        // if(has_edge(s, t) or s == t) return; // Allow weighted edges as duplicates
        if(!has_node_internal(s)) add_node_internal(s);
        if(!has_node_internal(t)) add_node_internal(t);
        contiguous_nid_t s_mapped = node_map[s];
        contiguous_nid_t t_mapped = node_map[t];

        // Insert edge into adjacency_list
        adjacency_list[std::min(s_mapped, t_mapped)].push_back(std::max(s_mapped, t_mapped));
        degrees[s_mapped] += 1;
        degrees[t_mapped] += 1;
        edge_count++;
    };

    // original loop
        std::string line;
        while(std::getline(edgelist_file, line))
        {
            // Skip any comments
            if(line[0] == '#') continue;
			if(line[0] == '%') continue;

            // Read source and target from file
            nid_t s, t;
            std::istringstream(line) >> s >> t;

            // if(s != t and !has_edge(s, t)) add_edge(s, t); // Original behaviour of graph_viewer was to ignore duplicate edges. // TODO: Decide how to handle duplicate/weighted edges.
            add_edge_internal(s, t);
        }

        edgelist_file.close();

        // Temporary: checking values.
    }

    bool UGraph::has_node(contiguous_nid_t nid)
    {
        return degrees[nid] > 0; // Membership check by degree.
    }

    bool UGraph::has_edge(contiguous_nid_t s, contiguous_nid_t t)
    {
        if(!has_node(s) or !has_node(t)) return false;

        if(adjacency_list[std::min(s, t)].size() == 0) return false;

        std::vector<contiguous_nid_t> neighbors = adjacency_list[std::min(s, t)];
        if(std::find(neighbors.begin(), neighbors.end(), std::max(s, t)) == neighbors.end()) // TODO: Carfully read this and check it for correctness.
            return false;
        else
            return true;
    }

    inline void UGraph::add_node()
    {
        node_count++; // TODO: Revert to old version, get everything else working.
    }

    void UGraph::add_edge(contiguous_nid_t s, contiguous_nid_t t)
    {
        // if(has_edge(s, t) or s == t) return; // Allow weighted edges as duplicates
        if(!has_node(s)) add_node();
        if(!has_node(t)) add_node();

        // Insert edge into adjacency_list
        adjacency_list[std::min(s, t)].push_back(std::max(s, t));
        degrees[s] += 1;
        degrees[t] += 1;
        edge_count++;
    }

    contiguous_nid_t UGraph::num_nodes() const
    {
        return node_count;
    }

    contiguous_nid_t UGraph::num_edges() const
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
        return degrees[nid];
    }

    /**
     * Index via MAPPED nids
     */
    std::vector<contiguous_nid_t> UGraph::neighbors_with_geq_id(contiguous_nid_t nid)
    {
        return adjacency_list[nid];
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
