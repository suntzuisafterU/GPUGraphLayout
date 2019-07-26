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
#include "RPDangerousTypeDefs.hpp"

namespace RPGraph
{
    // Type to represent edge IDs.
    // NOTE: uint32_t limits density to 50% for directed graphs.
    typedef uint32_t eid_t;

    // Virtual base class to derive different Graph types from.
    class Graph
    {
        public:
            virtual uint32_t num_nodes() const = 0;
            virtual uint32_t num_edges() const = 0;
            virtual uint32_t degree(contiguous_nid_t nid) = 0;
            virtual const std::vector<contiguous_nid_t> neighbors_with_geq_id(contiguous_nid_t nid) = 0; /**< Returns adjacency list associated with nid. Used by CPU-FA2 and PNG-writer only */
            virtual ~Graph() = 0; /**< Pure virtual method, specified by `= 0;`. Means that deriving class must override, but can use optional implementation provided by superclass via the `= default;` keyword. see https://stackoverflow.com/questions/34383516/should-i-default-virtual-destructors */
    };

	/**
     * Very basic (adjacency list) representation of an undirected graph. NOTE: All internal nid_t references are mapped to be contigous.  See node_map.
	 */
    class UGraph : public Graph
    {
    public:
        explicit UGraph();
        explicit UGraph(std::string edgelist_path);

        UGraph(const UGraph&) = delete;             /// Disallow copying.
        UGraph & operator=(const UGraph&) = delete; /// Disallow copy assignment operator.
        /// Also disallows move semantics, unless explicitly specified.

        ~UGraph(); /* Explicity declare and define destructors. */

        virtual uint32_t num_nodes() const override; /// Use uint32_t to increase the range we can support.
        virtual uint32_t num_edges() const override;
        virtual uint32_t degree(contiguous_nid_t nid);

		const std::unordered_map <contiguous_nid_t, uint32_t> get_degrees() const; /// Allow const access to degrees map.

		void add_edge_public(dangerous_nid_t, dangerous_nid_t);
		bool has_edge_public(dangerous_nid_t, dangerous_nid_t) const;

		inline bool contains(comm_id_t comm) const { return external_to_contig.count(comm) > 0; };
        inline contiguous_nid_t getContigFromExternal(dangerous_nid_t external_nid) const { return external_to_contig.at(external_nid); }; /// Basically identical to getContigFromComm once compiled, exists to provide clarity to users.
		inline contiguous_nid_t getContigFromComm(comm_id_t comm_node_id) const { return external_to_contig.at(comm_node_id); }; /// IMPORTANT: When accessing nodes use appropriiate maps.
		inline dangerous_nid_t getExternalFromContig(contiguous_nid_t nid) { return contig_to_external.at(nid); }; /// Will fail if does not exist, by design.

		// can this method be const?  Would that make a difference?
        const std::vector<contiguous_nid_t> neighbors_with_geq_id(contiguous_nid_t nid) override; /**< IMPORTANT: adjacency list only stores the ids of neighbors with greaterthan or equal id. */
    private:
        uint32_t node_count, edge_count;

		std::unordered_map<dangerous_nid_t, contiguous_nid_t> external_to_contig; /// Used on insertion of data to graph.
		std::unordered_map<contiguous_nid_t, dangerous_nid_t> contig_to_external; /// Used when extracting data from graph for IO or... 

        std::unordered_map <contiguous_nid_t, uint32_t> degrees;
        std::unordered_map <contiguous_nid_t, std::vector<contiguous_nid_t> > adjacency_list; /**< adjacency_list: Maps nid_t to list of nodes adjacent AND with ids greater than the mapped id. */

        void add_node_private(dangerous_nid_t nid);
        void add_edge_private(contiguous_nid_t s, contiguous_nid_t t); /**< Adding an edge also adds any nodes. */

        bool has_node_private(dangerous_nid_t nid) const;
        bool has_edge_private(contiguous_nid_t s, contiguous_nid_t t) const;

        void read_edgelist_file(std::string edgelist_path); /**< read file at path and initialize graph. */

    };

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
        uint32_t node_count, edge_count;
        contiguous_nid_t first_free_id;
        uint32_t edges_seen;

    public:
        std::unordered_map<contiguous_nid_t, contiguous_nid_t> nid_to_offset;
        contiguous_nid_t *offset_to_nid;

        CSRUGraph(uint32_t num_nodes, uint32_t num_edges);
        ~CSRUGraph();

        /// Inserts node_id and its edges. Once inserted, edges
        /// can't be altered for this node.
        void insert_node(contiguous_nid_t node_id, std::vector<contiguous_nid_t> nbr_ids);
        void fix_edge_ids(); // this should go...

        virtual uint32_t num_nodes() const override;
        virtual uint32_t num_edges() const override;
        virtual uint32_t degree(contiguous_nid_t nid) override;

        contiguous_nid_t nbr_id_for_node(contiguous_nid_t nid, contiguous_nid_t nbr_no);
    };

} // namespace RPGraph

#endif /* Graph_h */
