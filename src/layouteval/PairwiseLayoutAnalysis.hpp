#ifndef PairwiseLayoutAnalysis_hpp
#define PairwiseLayoutAnalysis_hpp

namespace RPGraph {

    struct PairwiseLayoutReport {
        float average_distances_delta; // TODO: What should actually be reported?
    };

	PairwiseLayoutReport computePairwiseCoordinateError(GraphLayout& layout1, GraphLayout& layout2);

} // namespace RPGraph

#endif // PairwiseLayoutAnalysis_hpp