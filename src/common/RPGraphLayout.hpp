/*
 ==============================================================================

 RPGraphLayout.cpp
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

#ifndef RPGraphLayout_hpp
#define RPGraphLayout_hpp

#include "RPGraph.hpp" // Using UGraph
#include "RPCommon.hpp" // Using Real2DVector
#include <string>

namespace RPGraph
{
    class GraphLayout
    {
    private:
        Coordinate *coordinates;

    protected:
        float width, height;
        float minX(), minY(), maxX(), maxY();

    public:
        GraphLayout(RPGraph::UGraph &graph,
                    float width = 10000, float height = 10000);
        
        GraphLayout(const GraphLayout& other) = delete;             /// Disallow copy construction.
        GraphLayout & operator=(const GraphLayout& other) = delete; /// Disallow copy assignment.

        ~GraphLayout();

        UGraph &graph; // to lay-out

        // randomize the layout position of all nodes.
        void randomizePositions();

        float getX(contiguous_nid_t node_id), getY(contiguous_nid_t node_id);
        float getXRange(), getYRange(), getSpan();
        float getDistance(contiguous_nid_t n1, contiguous_nid_t n2);
        Real2DVector getDistanceVector(contiguous_nid_t n1, contiguous_nid_t n2);
        Real2DVector getNormalizedDistanceVector(contiguous_nid_t n1, contiguous_nid_t n2);
        Coordinate getCoordinate(contiguous_nid_t node_id);
        Coordinate getCenter();


        void setX(contiguous_nid_t node_id, float x_value), setY(contiguous_nid_t node_id, float y_value);
        void moveNode(contiguous_nid_t , Real2DVector v);
        void setCoordinates(contiguous_nid_t node_id, Coordinate c);
    };
}

#endif /* RPGraphLayout_hpp */
