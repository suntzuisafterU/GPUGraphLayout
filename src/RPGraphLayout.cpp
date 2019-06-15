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

// Reading 


#include "RPGraphLayout.hpp"
#include "../lib/pngwriter/src/pngwriter.h" // Is this coupled directly to the layout?

#include <fstream>
#include <cmath>
#include <limits>
#include <iostream> // TODO: Temp remove

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
    GraphLayout::GraphLayout(UGraph &graph, float width, float height)
        : graph(graph), width(width), height(height)
    {
        // Mem complexity: O(|V|)
        coordinates = (Coordinate *) malloc(graph.num_nodes() * sizeof(Coordinate));
    }

    GraphLayout::~GraphLayout()
    {
        free(coordinates);
    }

    void GraphLayout::randomizePositions()
    {
        /**
         * TODO: Modify this bit when using communities to randomize node
         *       positions within some delta of the community position in the
         *       community layout that was produced originally.
         * NOTE: Keep original for laying out the community graph.
         */
        for (nid_t i = 0; i <  graph.num_nodes(); ++i)
        {
            setX(i, get_random(-width/2.0, width/2.0));
            setY(i, get_random(-height/2.0, height/2.0));
        }
    }

    float GraphLayout::getX(nid_t node_id)
    {
        return coordinates[node_id].x;
    }

    float GraphLayout::getY(nid_t node_id)
    {
        return coordinates[node_id].y;
    }

    /**
     * Find: 
     *   getXRange: only this file, getSpan, getCenter, writePng
     *   getYRange: only this file
     *   getCenter: RPCPUForceAtlas2.cpp, not important
     */
    float GraphLayout::minX()
    {
        float minX = std::numeric_limits<float>::max();
        for (nid_t n = 0; n < graph.num_nodes(); ++n)
            if (getX(n) < minX) minX = getX(n);
        return minX;
    }

    float GraphLayout::maxX()
    {
        float maxX = std::numeric_limits<float>::min();
        for (nid_t n = 0; n < graph.num_nodes(); ++n)
            if (getX(n) > maxX) maxX = getX(n);
        return maxX;
    }

    float GraphLayout::minY()
    {
        float minY = std::numeric_limits<float>::max();
        for (nid_t n = 0; n < graph.num_nodes(); ++n)
            if (getY(n) < minY) minY = getY(n);
        return minY;
    }

    float GraphLayout::maxY()
    {
        float maxY = std::numeric_limits<float>::min();
        for (nid_t n = 0; n < graph.num_nodes(); ++n)
            if (getY(n) > maxY) maxY = getY(n);
        return maxY;
    }

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
     * Usage? CPU FA2 only.
	 * 
	 * What 
     */
    float GraphLayout::getDistance(nid_t n1, nid_t n2)
    {
        const float dx = getX(n1)-getX(n2);
        const float dy = getY(n1)-getY(n2);
        return std::sqrt(dx*dx + dy*dy);
    }

    /**
     * Which of these 2 are used?
     * Vectors are used in CPU FA2 implementation.
     */
    Real2DVector GraphLayout::getDistanceVector(nid_t n1, nid_t n2)
    {
        return Real2DVector(getX(n2) - getX(n1), getY(n2) - getY(n1));
    }

    /**
     * Make that which of these 3 are used? IRRELEVANT: Commented out in CPU FA2 only.
     */
    Real2DVector GraphLayout::getNormalizedDistanceVector(nid_t n1, nid_t n2)
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
	 *
	 * TODO: Should we map through the associated UGraph by default? Or is that to much overhead? Or should this be a private field with friend specification for classes that need it, and we can overload it for the public interface?
     */
    Coordinate GraphLayout::getCoordinate(nid_t node_id)
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
    void GraphLayout::setX(nid_t node_id, float x_value)
    {
        coordinates[node_id].x = x_value;
    }

    void GraphLayout::setY(nid_t node_id, float y_value)
    {
        coordinates[node_id].y = y_value;
    }

    /**
     * Why is move only used with a 2DVector?
     */
    void GraphLayout::moveNode(nid_t n, RPGraph::Real2DVector v)
    {
        setX(n, getX(n) + v.x);
        setY(n, getY(n) + v.y);
    }

    /**
     * Never used. Why not?
     * TODO: We will probably use this and maybe the moveNode function to specify our layout after
     */
    void GraphLayout::setCoordinates(nid_t node_id, Coordinate c)
    {
        setX(node_id, c.x);
        setY(node_id, c.y);
    }

    /**
     * pngwriter entry point.
     */
    void GraphLayout::writeToPNG(const int image_w, const int image_h,
                                 std::string path)
    {
        const float xRange = getXRange();
        const float yRange = getYRange();
        const RPGraph::Coordinate center = getCenter(); // Why do we need the center?
        const float xCenter = center.x; // Probably to set the origin for layout.
        const float yCenter = center.y;
        const float minX = xCenter - xRange/2.0;
        const float minY = yCenter - yRange/2.0;
        const float xScale = image_w/xRange;
        const float yScale = image_h/yRange;

        // Here we need to do some guessing as to what the optimal
        // opacity of nodes and edges might be, given network size.
        /**
         * TODO: Adjust node opacity for best results.
         */
        const float node_opacity = 10000.0  / graph.num_nodes();
        const float edge_opacity = 10.0 / graph.num_edges();

        // Write to file.
        pngwriter layout_png(image_w, image_h, 0, path.c_str());
        layout_png.invert(); // set bg. to white

        for (nid_t n1 = 0; n1 < graph.num_nodes(); ++n1)
        {
            // Plot node,
            layout_png.filledcircle_blend((getX(n1) - minX)*xScale,
                                          (getY(n1) - minY)*yScale,
                                          3, node_opacity, 0, 0, 0);
            for (nid_t n2 : graph.neighbors_with_geq_id(n1)) {
                // ... and edge.
                layout_png.line_blend((getX(n1) - minX)*xScale, (getY(n1) - minY)*yScale,
                                      (getX(n2) - minX)*xScale, (getY(n2) - minY)*yScale,
                                      edge_opacity, 0, 0, 0);
            }
        }
        // Write it to disk.
        layout_png.write_png();
    }

    /**
     * Writing to csv may be a good way to streamline testing the decompressions effectiveness.
     * NOTE: There is no loadFromCSV method.  Would go in this file.
     */
    void GraphLayout::writeToCSV(std::string path)
    {
        if (is_file_exists(path.c_str()))
        {
            printf("Error: File exists at %s\n", path.c_str());
            exit(EXIT_FAILURE);
        }

        std::ofstream out_file(path);

        for (nid_t n = 0; n < graph.num_nodes(); ++n)
        {
            nid_t id = graph.node_map_r[n]; // id as found in edgelist
            out_file << id << "," << getX(n) << "," << getY(n) << "\n";
        }

        out_file.close();
    }

    /**
     * Do we have any use for writing to bin?
     */
    void GraphLayout::writeToBin(std::string path)
    {
        if (is_file_exists(path.c_str()))
        {
            printf("Error: File exists at %s\n", path.c_str());
            exit(EXIT_FAILURE);
        }

        std::ofstream out_file(path, std::ofstream::binary);

        for (nid_t n = 0; n < graph.num_nodes(); ++n)
        {
            nid_t id = graph.node_map_r[n]; // id as found in edgelist
            float x = getX(n);
            float y = getY(n);

            out_file.write(reinterpret_cast<const char*>(&id), sizeof(id));
            out_file.write(reinterpret_cast<const char*>(&x), sizeof(x));
            out_file.write(reinterpret_cast<const char*>(&y), sizeof(y));
        }

        out_file.close();
    }

}
