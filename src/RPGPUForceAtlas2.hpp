/*
 ==============================================================================

 RPGPUForceAtlas2.hpp
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

// Reading May 28th

#ifndef RPGPUForceAtlas2_hpp
#define RPGPUForceAtlas2_hpp
#include "RPForceAtlas2.hpp"

namespace RPGraph
{
    /**
     * What is the default scoping? Does this limit our scope or expand it?
     */
    class CUDAForceAtlas2: public ForceAtlas2
    {
    public:
        CUDAForceAtlas2(GraphLayout &layout, bool use_barneshut,
                        bool strong_gravity, float gravity, float scale,
						bool randomize, bool use_linlog=false);
        ~CUDAForceAtlas2();
        void doStep() override;
        void sync_layout() override;

    private:
        /// CUDA Specific stuff.
        // Host storage.
        float *body_mass; /* host float pointer (array) */
        float2 *body_pos; /* host float2 pointer (array of vectors) */
        float *fx, *fy, *fx_prev, *fy_prev; /* host float pointers (arrays) */

        // Quick way to represent a graph on the GPU
        int *sources, *targets; /* Quick way to represent a graph on the GPU */

        // Pointers to device memory (all suffixed with 'l').
        int   *errl,  *sortl, *childl, *countl, *startl; /* int pointers in device memory (arrays) */
        int   *sourcesl, *targetsl; /* int pointers in device memory (arrays) */
        float *body_massl, *node_massl; /* float pointers in device memory (arrays) */
        float2 *body_posl, *node_posl; /* float2 (vector) pointers in device memory (arrays) */
        float *minxl, *minyl, *maxxl, *maxyl; /* float pointers in device memory (arrays) */
        float *fxl, *fyl, *fx_prevl, *fy_prevl; /* float pointers in device memory (arrays) */
        float *swgl, *etral; /* float pointers in device memory (arrays) */

        int mp_count; // Number of multiprocessors on GPU.
        int max_threads_per_block;
        int nnodes;
        int nbodies;
        int nedges;

        void sendGraphToGPU();
        void sendLayoutToGPU();
        void retrieveLayoutFromGPU();
        void freeGPUMemory();
    };
};


#endif /* RPGPUForceAtlas2_hpp */
