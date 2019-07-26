#include "PairwiseLayoutAnalysis.hpp"

#include <iostream>

namespace RPGraph {

	PairwiseLayoutReport computePairwiseCoordinateError(GraphLayout& layout1, GraphLayout& layout2) {
		/* Ensure layouts are from the same graph. */
		if (layout1.graph != layout2.graph) {
			std::cout << "ERROR: Trying to compare layouts of different graphs!!" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		/* Iterate over nodes, get coordinates of each, compute the difference and RMSE. */
		uint32_t N{ layout1.graph.num_nodes() };
		for (uint32_t i = 0; i < N; ++i) {

		}

		/* In report, consider the size of the layout. Also compute the RMSE. */
	}

} // namespace RPGraph
