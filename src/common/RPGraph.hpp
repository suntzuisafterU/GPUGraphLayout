/*
 ==============================================================================

 RPGraph.hpp
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


#ifndef RPGraph_hpp
#define RPGraph_hpp
#include <vector>
#include <string>
#include <unordered_map>
#include "RPTypeDefs.hpp"

namespace RPGraph
{
    // Type to represent edge IDs.
    // NOTE: uint32_t limits density to 50% for directed graphs.
    typedef uint32_t eid_t;

    // Virtual base class to derive different Graph types from.
    class Graph
    {
        public:
            virtual uint32_t num_nodes() = 0;
            virtual uint32_t num_edges() = 0;
            virtual uint32_t degree(contiguous_nid_t nid) = 0;
            virtual std::vector<contiguous_nid_t> neighbors_with_geq_id(contiguous_nid_t nid) = 0; /**< Returns adjacency list associated with nid. Used by CPU-FA2 and PNG-writer only */
            virtual ~Graph() = 0; /**< Pure virtual method, specified by `= 0;`. Means that deriving class must override, but can use optional implementation provided by superclass via the `= default;` keyword. see https://stackoverflow.com/questions/34383516/should-i-default-virtual-destructors */

    };

	/**
     * Very basic (adjacency list) representation of an undirected graph. NOTE: All internal nid_t references are mapped to be contigous.  See node_map.
	 */
    class UGraph : public Graph
    {
        // friend class GraphLayout;
        // friend class RPCPUForceAtlas2; // Why did I want to define these?
    private:
        uint32_t node_count, edge_count;
        std::vector <uint32_t> degrees; /**< Vector of degrees, indexed by contiguous_nid_t */
        std::vector <std::vector<contiguous_nid_t> > adjacency_list; /**< adjacency_list: Maps nid_t to list of nodes adjacent AND with ids greater than the mapped id. */

        void add_node(contiguous_nid_t nid); /* Moved add_node back to private section for safety. */
        void add_edge(contiguous_nid_t s, contiguous_nid_t t); /**< Adding an edge also adds any nodes. */

        bool has_edge(contiguous_nid_t s, contiguous_nid_t t); // TODO: Moved for testing purposes. Make private again later.
        bool has_node(contiguous_nid_t nid); // TODO: Find usage

        void read_edgelist_file(std::string edgelist_path); /**< read file at path and initialize graph. */

    public:
        UGraph();
        UGraph(std::string edgelist_path);

        UGraph(const UGraph&) = delete;             /// Disallow copying.
        UGraph & operator=(const UGraph&) = delete; /// Disallow copy assignment operator.
        /// Also disallows move semantics, unless explicitly specified.

        ~UGraph(); /* Explicity declare and define destructors. */

        virtual uint32_t num_nodes() override; /// Use uint32_t to increase the range we can support.
        virtual uint32_t num_edges() override;
        virtual int degree(contiguous_nid_t nid) override;

        std::vector<contiguous_nid_t> neighbors_with_geq_id(contiguous_nid_t nid) override; /**< IMPORTANT: adjacency list only stores the ids of neighbors with greaterthan or equal id. */
    };

	// // template<typename node_type>
	// class UG_Iter {
	// public:
	// 	explicit UG_Iter(const& UGraph ug);
	// 	void operator++();
	// 	U_Edge get_edge();
	// };

    /**
	 * Compressed sparserow (CSR) for undirected graphs.
	 * 
	 * Not currently used.
	 */
    class CSRUGraph : public Graph
    {
    private:
        contiguous_nid_t *edges;   /**< All edgelists, concatenated. */
        contiguous_nid_t *offsets; /**< For each node, into edges. */
        contiguous_nid_t node_count, edge_count;
        contiguous_nid_t first_free_id, edges_seen;

    public:
        std::unordered_map<contiguous_nid_t, contiguous_nid_t> nid_to_offset;
        contiguous_nid_t *offset_to_nid;

        CSRUGraph(contiguous_nid_t num_nodes, contiguous_nid_t num_edges);
        ~CSRUGraph();

        /// Inserts node_id and its edges. Once inserted, edges
        /// can't be altered for this node.
        void insert_node(contiguous_nid_t node_id, std::vector<contiguous_nid_t> nbr_ids);
        void fix_edge_ids(); // this should go...

        virtual uint32_t num_nodes() override;
        virtual uint32_t num_edges() override;
        virtual uint32_t degree(contiguous_nid_t nid) override;

        contiguous_nid_t nbr_id_for_node(contiguous_nid_t nid, contiguous_nid_t nbr_no);
    };

} // namespace RPGraph

#endif /* Graph_h */
