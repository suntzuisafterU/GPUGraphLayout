#include "PairwiseLayoutAnalysis.hpp"

#include <cmath>
#include <iostream>
#include <limits>

#include "../common/RPGraph.hpp"
#include "../common/RPTypeDefs.hpp"

namespace RPGraph {

	double EuclideanDistance(Coordinate c1, Coordinate c2) {
		double result{ std::sqrt(std::pow(c1.x - c2.x, 2) + std::pow(c1.y - c2.y, 2)) };
		return result;
	};

	PairwiseLayoutReport computePairwiseCoordinateError(GraphLayout& correct_layout, GraphLayout& community_expanded_layout) {
		/* Ensure layouts are from the same graph. */
		if (!(correct_layout.graph == community_expanded_layout.graph)) {
			std::cout << "ERROR: Trying to compare layouts of different graphs!!" << std::endl;
			exit(EXIT_FAILURE);
		}

		/* Iterate over nodes, get coordinates of each, compute the difference and RMSE. */
		uint32_t N{ correct_layout.graph.num_nodes() };
		double total_error{ 0 };
		double RMSE{ 0 };
		double largest_error{ 0 };
		double smallest_error{ std::numeric_limits<double>::infinity() };

		for (contiguous_nid_t i = 0; i < N; ++i) {
			Coordinate c1 = correct_layout.getCoordinateFromContig(i);
			Coordinate c2 = community_expanded_layout.getCoordinateFromContig(i);
			double Udist = EuclideanDistance(c1, c2);
			if (Udist > largest_error) largest_error = Udist;
			if (Udist < smallest_error) smallest_error = Udist;
			total_error += Udist;
			RMSE += std::pow(Udist, 2);
		}

		RMSE /= N;
		double average_error{ total_error / N };

		/* In report, consider the size of the layout. Also compute the RMSE. */
		PairwiseLayoutReport report = {
			correct_layout.graph.get_dataset_source(),
			N,
			total_error,
			average_error,
			largest_error,
			smallest_error,
			RMSE
		};

		return report;
	}

} // namespace RPGraph
