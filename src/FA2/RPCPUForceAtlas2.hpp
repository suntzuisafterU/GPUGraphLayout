/*
 ==============================================================================

 RPCPUForceAtlas2.hpp
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

#ifndef RPCPUForceAtlas2_hpp
#define RPCPUForceAtlas2_hpp

#include "RPForceAtlas2.hpp"

namespace RPGraph
{
    class CPUForceAtlas2 : public ForceAtlas2
    {
    public:
        CPUForceAtlas2(GraphLayout &layout, bool use_barneshut,
                       bool strong_gravity, float gravity, float scale,
					   bool randomize, bool use_linlog=false);

        CPUForceAtlas2(const CPUForceAtlas2& other) = delete;             /// Disallow copy construction.
        CPUForceAtlas2 & operator=(const CPUForceAtlas2& other) = delete; /// Disallow copy assignment.

        ~CPUForceAtlas2();
        void doStep() override;
        void sync_layout() override;

    private:
        Real2DVector *forces, *prev_forces;
        BarnesHutApproximator BH_Approximator;

        float swg(contiguous_nid_t n);            // swinging ..
        float s(contiguous_nid_t n);              // swinging as well ..
        float tra(contiguous_nid_t n);            // traction ..

        // Substeps of one step in layout process.
        void rebuild_bh();
        void apply_repulsion(contiguous_nid_t n);
        void apply_gravity(contiguous_nid_t n);
        void apply_attract(contiguous_nid_t n);
        void updateSpeeds();
        void apply_displacement(contiguous_nid_t n);
    };
}
#endif
