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


#include "RPGraphLayout.hpp"

#include <fstream>
#include <cmath>
#include <limits>
#include <iostream> // TODO: TEMP, DEBUGGING

/**
 * Most of the methods on the GraphLayout class seem to be associated with the CPU implementation, and not the GPU.
 */
namespace RPGraph
{
    /**
	 * Mallocs an array for the coordinates.  width and height default to 10,000
	 * 
     * Q: What are width and height for?  Size of image?
     */
    GraphLayout::GraphLayout(UGraph& graph, float width, float height)
        : width(width), height(height), graph(graph)
    {
        std::cout<<"In: GraphLayout::GraphLayout(UGraph graph, float width, float height) : width(width), height(height), graph{graph}" << std::endl;

        coordinates = (Coordinate *) malloc(graph.num_nodes() * sizeof(Coordinate));
    }

    GraphLayout::~GraphLayout()
    {
		std::cout << "In GraphLayout::~GraphLayout()" << std::endl;
        free(coordinates);
    }

    void GraphLayout::randomizePositions()
    {
        for (contiguous_nid_t i = 0; i <  graph.num_nodes(); ++i)
        {
            setX(i, get_random(-width/2.0, width/2.0));
            setY(i, get_random(-height/2.0, height/2.0));
        }
    }

    float GraphLayout::getX(contiguous_nid_t node_id)
    {
        return coordinates[node_id].x;
    }

    float GraphLayout::getY(contiguous_nid_t node_id)
    {
        return coordinates[node_id].y;
    }

    float GraphLayout::minX()
    {
        float minX = std::numeric_limits<float>::max();
        for (contiguous_nid_t n = 0; n < graph.num_nodes(); ++n)
            if (getX(n) < minX) minX = getX(n);
        return minX;
    }

    float GraphLayout::maxX()
    {
        float maxX = std::numeric_limits<float>::min();
        for (contiguous_nid_t n = 0; n < graph.num_nodes(); ++n)
            if (getX(n) > maxX) maxX = getX(n);
        return maxX;
    }

    float GraphLayout::minY()
    {
        float minY = std::numeric_limits<float>::max();
        for (contiguous_nid_t n = 0; n < graph.num_nodes(); ++n)
            if (getY(n) < minY) minY = getY(n);
        return minY;
    }

    float GraphLayout::maxY()
    {
        float maxY = std::numeric_limits<float>::min();
        for (contiguous_nid_t n = 0; n < graph.num_nodes(); ++n)
            if (getY(n) > maxY) maxY = getY(n);
        return maxY;
    }

    /**
     * Find: 
     *   getXRange: only this file, getSpan, getCenter, writePng
     *   getYRange: only this file
     *   getCenter: RPCPUForceAtlas2.cpp.
     */
    float GraphLayout::getXRange()
    {
        return maxX()- minX();
    }

    float GraphLayout::getYRange()
    {
        return maxY() - minY();
    } 
                     
    float GraphLayout::getSpan()
    {
        return ceil(fmaxf(getXRange(), getYRange()));
    }

    /**
     * Usage:
	 *   CPU FA2 apply_attract and apply_repulsion.
     */
    float GraphLayout::getDistance(contiguous_nid_t n1, contiguous_nid_t n2)
    {
        const float dx = getX(n1)-getX(n2);
        const float dy = getY(n1)-getY(n2);
        return std::sqrt(dx*dx + dy*dy);
    }

    /**
     * Which of these 2 are used?
     * Vectors are used in CPU FA2 implementation.
     */
    Real2DVector GraphLayout::getDistanceVector(contiguous_nid_t n1, contiguous_nid_t n2)
    {
        return Real2DVector(getX(n2) - getX(n1), getY(n2) - getY(n1));
    }

    /**
     * Make that which of these 3 are used? IRRELEVANT: Commented out in CPU FA2 only.
     */
    Real2DVector GraphLayout::getNormalizedDistanceVector(contiguous_nid_t n1, contiguous_nid_t n2)
    {
        const float x1 = getX(n1);
        const float x2 = getX(n2);
        const float y1 = getY(n1);
        const float y2 = getY(n2);
        const float dx = x2 - x1;
        const float dy = y2 - y1;
        const float len = std::sqrt(dx*dx + dy*dy);

        return Real2DVector(dx / len, dy / len);
    }

    /**
     * Indexes into coordinates array. node_id MUST be mapped through the associated UGraph object.
     */
    Coordinate GraphLayout::getCoordinate(contiguous_nid_t node_id) const
    {
        return coordinates[node_id];
    }

    /**
     * Usage: pngwriter only.
     */
    Coordinate GraphLayout::getCenter()
    {
        float x = minX() + getXRange()/2.0;
        float y = minY() + getYRange()/2.0;
        return Coordinate(x, y);
    }

    /**
     * Updates x in coordinates array.
     */
    void GraphLayout::setX(contiguous_nid_t node_id, float x_value)
    {
        coordinates[node_id].x = x_value;
    }

    void GraphLayout::setY(contiguous_nid_t node_id, float y_value)
    {
        coordinates[node_id].y = y_value;
    }

    /**
     * Why is move only used with a 2DVector?
     */
    void GraphLayout::moveNode(contiguous_nid_t n, RPGraph::Real2DVector v)
    {
        setX(n, getX(n) + v.x);
        setY(n, getY(n) + v.y);
    }

    void GraphLayout::setCoordinates(contiguous_nid_t node_id, Coordinate c)
    {
        setX(node_id, c.x);
        setY(node_id, c.y);
    }

}
