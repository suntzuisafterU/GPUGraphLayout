#ifndef PairwiseLayoutAnalysis_hpp
#define PairwiseLayoutAnalysis_hpp

#include <iostream>
#include <string>

#include "../common/RPGraphLayout.hpp"

namespace RPGraph {

    struct PairwiseLayoutReport {
		std::string dataset_source;
		uint32_t num_nodes;
		double total_error;
        double average_error; // TODO: What should actually be reported?
		double largest_error;
		double smallest_error;
		double RMSE;

		friend std::ostream& operator<< (std::ostream& out, PairwiseLayoutReport& report) {
			out << "##### Pairwise Layout Difference Report #####" << std::endl;
			out << "# dataset source: " << report.dataset_source << std::endl;
			out << "# num nodes: " << report.num_nodes << std::endl;
			out << "# total error: " << report.total_error << std::endl;
			out << "# average error: " << report.average_error << std::endl;
			out << "# largest error: " << report.largest_error << std::endl;
			out << "# smallest error: " << report.smallest_error << std::endl;
			out << "# RMSE: " << report.RMSE << std::endl;
			return out;
		};
    };

	PairwiseLayoutReport computePairwiseCoordinateError(GraphLayout& layout1, GraphLayout& layout2);

} // namespace RPGraph

#endif // PairwiseLayoutAnalysis_hpp