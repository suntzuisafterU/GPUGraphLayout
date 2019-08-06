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
	int num_samples;

	friend std::ostream& operator<<(std::ostream& out, StressReport& report) {
		out << "##### Stress Report #####" << std::endl;
		out << "# stress: " << report.stress << std::endl;
		out << "# num nodes: " << report.num_nodes << std::endl;
		out << "# stress per node: " << report.stress_per_node << std::endl;
		out << "# num edges: " << report.num_edges << std::endl;
		out << "# stress per edge: " << report.stress_per_edge << std::endl;
		if (report.num_samples > 0) out << "# num samples: " << report.num_samples << std::endl;
		return out;
	}
};

} // namespace RPGraph

#endif // StressReport_hpp