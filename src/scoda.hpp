
#ifndef scoda_hpp
#define scoda_hpp
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <unordered_map>
#include <fstream>
#include <sstream> // for istringstream() in scoda.cpp

#include "RPGraph.hpp" // For UGraph and nid_t

namespace CommunityAlgos {
    /**
     *
     */
    int scoda(int degree_threshold, std::fstream& edgelist_file,
                RPGraph::UGraph* full_graph, RPGraph::UGraph* comm_graph, 
                std::unordered_map<RPGraph::nid_t, RPGraph::nid_t>* nid_comm_vec);

}

#endif /* scoda_hpp */