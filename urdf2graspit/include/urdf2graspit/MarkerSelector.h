/**
    Copyright (C) 2015 Jennifer Buehler

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software Foundation,
    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**/

#ifndef URDF2GRASPIT_MARKERSELECTOR_H
#define URDF2GRASPIT_MARKERSELECTOR_H
// Copyright Jennifer Buehler

#include <Inventor/Qt/SoQt.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>

#include <Eigen/Geometry>

#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace urdf2graspit
{
namespace markerselector
{
/**
 * \brief Runs a viewer using the Inventor package and allows to select and save markers on the model.
 * \author Jennifer Buehler
 * \date last edited October 2015
 */
class MarkerSelector
{
public:
    /**
     * \brief represents a marker with x/y/z coordinates and a normal, associated with a link (and visual).
     */
    class Marker
    {
    public:
        Marker() {}
        Marker(const Marker& m):
            linkName(m.linkName),
            visualNum(m.visualNum),
            coords(m.coords),
            normal(m.normal) {}

        ~Marker() {}


        friend std::ostream& operator<<(std::ostream& o, const Marker& m)
        {
            o << m.linkName << "; visual " << m.visualNum << "; Coords " << m.coords
              << "; normal " << m.normal;
            return o;
        }

        void setCoords(const double x, const double y, const double z)
        {
            coords = Eigen::Vector3d(x, y, z);
        }
        void setNormal(const double x, const double y, const double z)
        {
            normal = Eigen::Vector3d(x, y, z);
        }

        // coordinates in link reference frame
        Eigen::Vector3d coords;

        // normal
        Eigen::Vector3d normal;

        // name of the link the marker is on
        std::string linkName;
        // name of the visual the marker is on
        int visualNum;
    };

    typedef std::map<std::string, std::vector<Marker> > MarkerMap;


    /**
     * \param _marker_size the size of the points displayed where marker is put
     * \param _faces_ccw faces are to be treated as counter-clockwise. Needed for normal calculations.
     */
    explicit MarkerSelector(float _marker_size, bool _faces_ccw = true):
        root(NULL), viewWindow(NULL), viewer(NULL),
        marker_size(_marker_size), faces_ccw(_faces_ccw) {}

    MarkerSelector(const MarkerSelector& o):
        root(o.root), viewWindow(o.viewWindow), viewer(o.viewer),
        marker_size(o.marker_size), faces_ccw(o.faces_ccw) {}
    ~MarkerSelector() {}

    // Initialize Inventor and Qt
    void init(const char * windowName = "MarkerSelector");

    void loadModel(SoNode * model);

    bool loadModel(const std::string& filename);

    void runViewer();

    /**
     * Writes all markers selected while execution runViewer() in the following format, for all
     * links with markers subsequently:
     * link-name {newline}
     * number-of-markers {newline}
     * visual_number x y z nx ny nz{newline} (coordinates and normal, repeated for all number-of-markers markers)
     */
    bool writeResults(const std::string& outputFile);

    /**
     * returns the map of markers which have been generated during running runViewer().
     * the entries for each link name will be ordered by the visual numbers.
     */
    MarkerMap getMarkers();

    std::string toString();

private:
    // helper function to sort markers
    static bool sortMarker(const Marker& i, const Marker& j);

    static bool writeToFile(const std::string& content, const std::string& filename);

    static SoNode * getLinkDesc(const SoPath * path, std::string& linkName, int& visualNum);

    static void addVisualMarker(SoNode * linkNode, const Eigen::Vector3d& pos, float _marker_size);

    /**
     * Calculates the correct face normal of the pick point.
     */
    static bool computeCorrectFaceNormal(const SoPickedPoint * pick, bool ccw_face, Eigen::Vector3d& normal);

    static void mouseBtnCB(void *userData, SoEventCallback *pNode);


    SoSelection * root;
    QWidget * viewWindow;
    SoQtExaminerViewer * viewer;
    std::vector<Marker> markers;
    // the size of the points displayed where marker is put
    float marker_size;
    bool faces_ccw;
};

}  //  namespace markerselector
}  //  namespace urdf2graspit
#endif   // URDF2GRASPIT_MARKERSELECTOR_H