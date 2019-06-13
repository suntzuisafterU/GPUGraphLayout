/*
 ==============================================================================

 RPForceAtlas2.cpp
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

#include "RPForceAtlas2.hpp"

namespace RPGraph
{
    /**
     * CPU version of FA2 algo???
     * Why do we have the scope modifier ForceAtlas2::ForceAtlas2??
	 *
	 * TODO: Decouple layout initialization from LayoutAlgorithm, or allow for the LayoutAlgorithm to check and see if the layout has already been initialized instead of reassigning it.
     */
    ForceAtlas2::ForceAtlas2(GraphLayout &layout, bool use_barneshut,
                             bool strong_gravity, float gravity, float scale,
							 bool randomize)
    : LayoutAlgorithm(layout), use_barneshut{use_barneshut},
      strong_gravity{strong_gravity}
    {
        iteration = 0; /**< Why does the FA2 class keep track of the iteration? */

        setGravity(gravity);
        setScale(scale);

        global_speed = 1.0;
        speed_efficiency = 1.0;
        jitter_tolerance = 1.0;

        k_s = 0.1;
        k_s_max = 10.0;

        theta = 1.0;
        epssq  = 0.05 * 0.05;
        itolsq = 1.0f / (theta * theta);

        delta = 0.0;

        prevent_overlap = false;
        use_linlog = false;

        if(randomize) layout.randomizePositions(); /**< Parameterized to allow for pre-setting the layout from some other source. */
    }

    ForceAtlas2::~ForceAtlas2(){}; /**< TODO: What does this destructor do? It is a default destructor, but what happens when it is virtual? */

    /**
     * Doesn't get called!
     */
    void ForceAtlas2::doSteps(int n)
    {
        for (int i = 0; i < n; ++i) doStep();
    }

    void ForceAtlas2::setScale(float s)
    {
        k_r = s;
    }

    void ForceAtlas2::setGravity(float g)
    {
        k_g = g;
    }

    float ForceAtlas2::mass(nid_t n)
    {
        return layout.graph.degree(n) + 1.0;
    }
}
