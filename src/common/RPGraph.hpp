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

namespace RPGraph
{
    // Type to represent node IDs.
    // NOTE: we limit to 4,294,967,296 nodes through uint32_t.
    typedef uint32_t nid_t;

    // Type to represent edge IDs.
    // NOTE: uint32_t limits density to 50% for directed graphs.
    typedef uint32_t eid_t;

	// TODO: Define another typedef for internal node_ids vs external node_ids.  Then we can use casting to convert when required. Can typedefs have inheritance? Then we could us a more generic version for some functions where appropriate.

    // Virtual base class to derive different Graph types from.
    class Graph
    {
        public:
            virtual nid_t num_nodes() = 0;
            virtual nid_t num_edges() = 0;
            virtual nid_t degree(nid_t nid) = 0;
            virtual nid_t in_degree(nid_t nid) = 0;
            virtual nid_t out_degree(nid_t nid) = 0;
            virtual std::vector<nid_t> neighbors_with_geq_id(nid_t nid) = 0; /**< Returns adjacency list associated with nid. Used by CPU-FA2 and PNG-writer only */
            virtual ~Graph() = 0; /**< Pure virtual method, specified by `= 0;`. Means that deriving class must override, but can use optional implementation provided by superclass via the `= default;` keyword. see https://stackoverflow.com/questions/34383516/should-i-default-virtual-destructors */

    };

	/**
     * Very basic (adjacency list) representation of an undirected graph. NOTE: All internal nid_t references are mapped to be contigous.  See node_map.
	 */
    class UGraph : public Graph
    {
    private:
        nid_t node_count, edge_count;
        std::unordered_map<nid_t, nid_t> degrees; /**< Maps nid_t to degrees? */
        std::unordered_map<nid_t, std::vector<nid_t>> adjacency_list; /**< adjacency_list: Maps nid_t to list of nodes adjacent AND with ids greater than the mapped id. */

        void add_node(nid_t nid); /* Moved add_node back to private section for safety. */

        bool has_node(nid_t nid);
		// friend class UG_Iter; // TODO: Is this the proper way to associate an iterator with a container?

    public:
        explicit UGraph();
        ~UGraph(); /* Explicity declare and define destructors. */

        /**
         * Construct UGraph from edgelist. IDs in edgelist are mapped to
         * [0, 1, ..., num_nodes-1]. Removes any self-edges.
         */
        void read_edgelist_file(std::string edgelist_path); /**< read file at path and initialize graph. */
        std::unordered_map<nid_t, nid_t> node_map; /**< el id => UGraph id. IMPORTANT: This is necessary so that we can produce a contigous array for the CUDA implementation to work on.  You have been warned. */
        std::unordered_map<nid_t, nid_t> node_map_r; /**< UGraph id => el id. Only used by writeToBin() and writeToCsv() */

        void add_edge(nid_t s, nid_t t); /**< Adding an edge also adds any nodes. */
        bool has_edge(nid_t s, nid_t t); // TODO: Moved for testing purposes. Make private again later.

        virtual nid_t num_nodes() override;
        virtual nid_t num_edges() override;
        virtual nid_t degree(nid_t nid) override;
        virtual nid_t in_degree(nid_t nid) override; /**< Redundant */
        virtual nid_t out_degree(nid_t nid) override; /**< Redundant */

        std::vector<nid_t> neighbors_with_geq_id(nid_t nid) override; /**< IMPORTANT: adjacency list only stores the ids of neighbors with greaterthan or equal id. */
    };

	// class U_Edge {
	// public:
	// 	explicit U_Edge(nid_t src, nid_t dst) { src, dst };
	// 	nid_t get_src() { return src; };
	// 	nid_t get_dst() { return dst; };
	// };

	// // template<typename node_id, class edge_id<typename node_id> >
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
        nid_t *edges;   /**< All edgelists, concatenated. */
        nid_t *offsets; /**< For each node, into edges. */
        nid_t node_count, edge_count;
        nid_t first_free_id, edges_seen;

    public:
        std::unordered_map<nid_t, nid_t> nid_to_offset;
        nid_t *offset_to_nid;

        CSRUGraph(nid_t num_nodes, nid_t num_edges);
        ~CSRUGraph();

        /// Inserts node_id and its edges. Once inserted, edges
        /// can't be altered for this node.
        void insert_node(nid_t node_id, std::vector<nid_t> nbr_ids);
        void fix_edge_ids(); // this should go...

        virtual nid_t num_nodes() override;
        virtual nid_t num_edges() override;
        virtual nid_t degree(nid_t nid) override;
        virtual nid_t in_degree(nid_t nid) override;
        virtual nid_t out_degree(nid_t nid) override;

        nid_t nbr_id_for_node(nid_t nid, nid_t nbr_no);
    };
}

#endif /* Graph_h */