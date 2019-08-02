#ifndef stress_hpp
#define stress_hpp

#include <vector>
#include <iostream>
#include "../common/RPCommon.hpp" // Coordinate
#include "../common/RPGraphLayout.hpp" // GraphLayout

namespace RPGraph {

typedef std::vector< std::vector< uint32_t > > matrix;

struct StressReport {
    double stress;
	uint32_t num_nodes;
	double stress_per_node;
	uint32_t num_edges;
	double stress_per_edge;

	// override addition operator.
	friend StressReport operator+(const StressReport& sr1, const StressReport& sr2) {
		double combined_stress{ sr1.stress + sr2.stress };
		uint32_t combined_num_nodes{ sr1.num_nodes + sr2.num_nodes };
		double combined_stress_per_node{ combined_stress/combined_num_nodes };
		uint32_t combined_num_edges{ sr1.num_edges + sr2.num_edges };
		double combined_stress_per_edge{ combined_stress/combined_num_edges };

		return StressReport {
			combined_stress,
			combined_num_nodes,
			combined_stress_per_node,
			combined_num_edges,
			combined_stress_per_edge
		};
	}

    // TODO: Should this have some way to reference the graph? Parameters that were used, etc.

	friend std::ostream& operator<<(std::ostream& out, StressReport& report) {
		out << "##### Stress Report #####" << std::endl;
		out << "# stress: " << report.stress << std::endl;
		out << "# num nodes: " << report.num_nodes << std::endl;
		out << "# stress per node: " << report.stress_per_node << std::endl;
		out << "# num edges: " << report.num_edges << std::endl;
		out << "# stress per edge: " << report.stress_per_edge << std::endl;
		return out;
	}
};

StressReport stress(RPGraph::GraphLayout& layout, matrix& all_pairs_shortest, int L);

StressReport stress_single_source(GraphLayout& layout, matrix& all_pairs_shortest, int L);

/**
 * O(N**2) memory complexity, O(N**3)?? time complexity. 
 */
matrix allPairsShortestPaths(RPGraph::UGraph& graph);

void print_matrix(RPGraph::matrix& mat, const char* reason);

} // RPGraph

#endif // stress_hpp