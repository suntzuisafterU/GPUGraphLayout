/*
 ==============================================================================

 RPForceAtlas2.hpp
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

#ifndef RPForceAtlas2_hpp
#define RPForceAtlas2_hpp

#include "RPLayoutAlgorithm.hpp"
#include "RPBarnesHutApproximator.hpp"

namespace RPGraph
{
    /**
     * Doxygen picks up header file comments.
     * Can be either CPU or GPU implementation.
     * 
     * Does this act like a jave interface??
     */
    class ForceAtlas2 : public LayoutAlgorithm
    {
        public:
            ForceAtlas2(GraphLayout &layout, bool use_barneshut,
                        bool strong_gravity, float gravity, float scale,
						bool randomize, bool use_linlog=false);
            virtual ~ForceAtlas2();

            virtual void doStep() = 0; /**< Pure virtual function that must be implemented by deriving class. */
            void doSteps(int n);
            void setScale(float s);
            void setGravity(float s);
            float mass(nid_t n);
            bool prevent_overlap, use_barneshut, use_linlog, strong_gravity;

        protected:
            int iteration;
            float k_r, k_g; /**< scalars for repulsive and gravitational force. */
            float delta; /**< edgeweight influence. */
            float global_speed;

            /**
             * Parameters used in adaptive temperature
             * (What is adaptive temperature?)
             */
            float speed_efficiency, jitter_tolerance;

            /**
             * Where are these magic constants used? In both GPU and CPU implementations?
             */
            float k_s, k_s_max; /**< Swing constants? */

            /**
             * TODO: Review and explain the Barnes-Hut parameters here.
             * Barnes-Hut parameters
             *
             * TODO: Found theta! Parameterize.
             */
            float theta;   /**< Accuracy */
            float epssq;   /**< Softening (Epsilon, squared) */
            float itolsq;  /**< Inverse tolerance, squared */
    };
}
#endif
