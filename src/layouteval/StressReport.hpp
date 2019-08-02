#ifndef StressReport_hpp
#define StressReport_hpp

#include <stdint.h>
#include <ostream>

namespace RPGraph {

struct StressReport {
    double stress;
	uint32_t num_nodes;
	double stress_per_node;
	uint32_t num_edges;
	double stress_per_edge;

	// override addition operator.
	friend StressReport operator+(const StressReport& sr1, const StressReport& sr2) {
		double combined_stress{ sr1.stress + sr2.stress };
		/* NOTE: sr1.num_nodes should be the same as sr2.num_nodes */
		uint32_t combined_num_nodes{ sr1.num_nodes /* + sr2.num_nodes */ }; // DON'T add the nodes together.  Should be the same.
		double combined_stress_per_node{ sr1.stress_per_node + sr2.stress_per_node /* combined_stress/combined_num_nodes */ };
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

} // namespace RPGraph

#endif // StressReport_hpp