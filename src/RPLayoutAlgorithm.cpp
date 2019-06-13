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

namespace RPGraph
{
    /**
     * An interface for RPForceAtlas2. If initialize is false, then the layout is pre-existing and we will just accept it by reference.
     */
	LayoutAlgorithm::LayoutAlgorithm(GraphLayout& layout, bool initLayout=true) {
		initLayout ? layout = GraphLayout(layout) : this->layout = layout; // TODO: TESTING!  Since this->layout is a reference member variable, we may be required to initialize it in a member intialization list, which would cause some greif.
	}
    LayoutAlgorithm::~LayoutAlgorithm(){}
}
