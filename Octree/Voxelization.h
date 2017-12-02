#pragma once
#include "Octree.h"
#include <vector>
#include <cmath>
#include <set>

class Voxelization:public Octree
{
    public:
        Voxelization():Octree(),xmax(0),xmin(0),ymax(0),ymin(0),zmax(0),zmin(0){}
        Voxelization
            (unsigned int x,unsigned int y,
             unsigned int z, int depth,
             float x_max=0,float x_min=0,
             float y_max=0,float y_min=0,
             float z_max=0,float z_min=0):
                xmax(x_max),xmin(x_min),
                ymax(y_max),ymin(y_min),
                zmax(z_max),zmin(z_min)
            {
                Octree(x,y,z,depth);
            }
        ~Voxelization(){}
        //get the extremum of the body
        void GetExtremum(float x_max,float x_min,float y_max,float y_min,float z_max,float z_min);
        //transform the vertex positions of a primitive into a regular eulerian grid
        void PointToVoxel(vector<CVertex> VectorPoint);
        //transform the edges of a primitive into a regular eulerian grid
        void EdgeToVoxel(vector<CEdge> VectorEdge, vector<CVertex> VectorPoint);
        //transform the edegs and facet of a primitive into a regular eulerian grid
        void FacetToVoxel(vector<CFacet> VectorFacet, vector<CVertex> VectorPoint);
        void SurfacePoint(unsigned int x,unsigned int y,unsigned int z);
        unsigned int ChangeCoordinate(float coordinate,float max,float min);
        //to check if the num is at the max vertex
        unsigned int CheckNum(unsigned int num);
        //Change the Edge on the part to voxel by using Bresenham
        void EdgeChange_Bresenham(OctreePoint point1, OctreePoint point2);
        //edge perpendicular to surface ,which means only one coordinate will change
        void PerpendicularToSurfaceEdge(OctreePoint point1,OctreePoint point2, int serial);
        //edge parallel to surface ,which means only two coordinate will change
        void ParallelToSurfaceEdge_Bresenham(OctreePoint point1, OctreePoint point2, int serial);
        void GeneralLocationEdge_Bresenham(OctreePoint point1, OctreePoint point2);
        //facet parallel to surface, which means all the voxels will have a same coordinate
        void ParallelToSurfaceFacet(OctreePoint point1, OctreePoint point2, OctreePoint point3, int serial);
        //use 2D Bresenham to voxel the line
        void Bresenham2D(Point2D pt2d1, Point2D pt2d2);
        //use floodseedfill to fill the 2dfacet
        void FloodSeedFill2D(Point2D point2d);
    private:
        float xmax,xmin,ymax,ymin,zmax,zmin;
        set<Point2D> opoint2D; //to save the point of the 2d Bresenham line
        vector<Point2D> strpoint2d; //to save the point filled by floodseedfill2d
};

