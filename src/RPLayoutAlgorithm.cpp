/*
 ==============================================================================

 RPLayoutAlgorithm.cpp
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

// Reading

#include "RPLayoutAlgorithm.hpp"
#include <iostream>

namespace RPGraph
{
    /**
     * An interface for RPForceAtlas2
     */
    LayoutAlgorithm::LayoutAlgorithm(GraphLayout& layout): layout(layout){ std::cout<<"Layout initializing constructor called"<<std::endl; } /**< TODO: Does this line, particularly `layout(layout) {}` initialize the layout pointer to be a newly constructed layout? YES: This mallocs a new set of coodrinates to be used for the layout algorithm.  Will factor this out. */
    LayoutAlgorithm::~LayoutAlgorithm(){}
}
