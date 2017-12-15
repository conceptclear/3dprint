/*********************************************************************************
  *Copyright(C),2017-2017,conceptclear
  *FileName:  Voxelization.h
  *Author:  conceptclear
  *Version:  1.0
  *Date:  2017.12.5
  *Description:  use this class to voxelize the Patches
                 provide two methods to voxelize the 2dline,
                 one is SuperCoverLine2D
                 the other is Bresenham2D.
                 provide two methods to voxelize the facet,
                 one is driven by floodseedfill,
                 the other is driven by scanning
**********************************************************************************/
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


        //member variables
        //get the extremum of the body
        void GetExtremum(float x_max,float x_min,float y_max,float y_min,float z_max,float z_min);
        //get the number of surfacepoint
        void GetSurfacePointNum(void);
        //set the perspective
        void setperspective(double eyex,double eyey,double eyez,double centrex,double centrey,double centrez,double upx,double upy,double upz);
        //return xmax
        float Xmax(void);
        //return xmin
        float Xmin(void);
        //return ymax
        float Ymax(void);
        //return ymin
        float Ymin(void);
        //return zmax
        float Zmax(void);
        //return zmin
        float Zmin(void);


        //Change to voxel
        //transform the vertex positions of a primitive into a regular eulerian grid
        void PointToVoxel(vector<CVertex> VectorPoint);
        //transform the edges of a primitive into a regular eulerian grid
        void EdgeToVoxel(vector<CEdge> VectorEdge, vector<CVertex> VectorPoint);
        //transform the edegs and facet of a primitive into a regular eulerian grid
        void FacetToVoxel(vector<CFacet> VectorFacet, vector<CVertex> VectorPoint);


        //some basic funcitons
        //Change this point to 0
        void SurfacePoint(unsigned int x,unsigned int y,unsigned int z);
        //Change coordinate from patches to voxels
        unsigned int ChangeCoordinate(float coordinate,float max,float min);
        //to check if the num is at the max vertex
        unsigned int CheckNum(unsigned int num);
        //use 2D Bresenham to voxel the line
        void Bresenham2D(Point2D pt2d1, Point2D pt2d2);
        //use supercoverline to voxel the line
        void SuperCoverLine2D(Point2D pt2d1, Point2D pt2d2);


        //Edge change by Bresenham
        //Change the Edge on the part to voxel by using Bresenham
        void EdgeChange_Bresenham(OctreePoint point1, OctreePoint point2);
        //edge perpendicular to surface ,which means only one coordinate will change
        void PerpendicularToSurfaceEdge(OctreePoint point1,OctreePoint point2, int serial);
        //edge parallel to surface ,which means only two coordinate will change
        void ParallelToSurfaceEdge_Bresenham(OctreePoint point1, OctreePoint point2, int serial);
        //general location means three coordinates will change
        void GeneralLocationEdge_Bresenham(OctreePoint point1, OctreePoint point2);


        //Facet change by floodfill
        //Change the Facet on the part to voxel by using FloodFill
        void FacetChange_FloodFill(OctreePoint point1, OctreePoint point2, OctreePoint point3);
        //facet parallel to surface, which means all the voxels will have a same coordinate
        void ParallelToSurfaceFacet_FloodFill(OctreePoint point1, OctreePoint point2, OctreePoint point3, int serial);
        //facet perpendicular to surface, which means one coordinate of the normal is 0
        void PerpendicularToSurfaceFacet_FloodFill(OctreePoint point1, OctreePoint point2, OctreePoint point3, int serial, int max_normal);
        //the facet with general location
        void GeneralLocationFacet_FloodFill(OctreePoint point1, OctreePoint point2, OctreePoint point3,int Normal_x,int Normal_y,int Normal_z,int Normalnum);
        //use floodseedfill to fill the 2dfacet
        void FloodSeedFill2D(Point2D point2d);
        //calculate the voxels to be fill
        void CoverVoxel2D_FloodFill(OctreePoint point1,OctreePoint point2,OctreePoint point3,int serial);
        //use checktriangle to check whether the point in the triangle or not
        bool CheckTriangle(Point2D centroid, Point2D point2d1, Point2D point2d2, Point2D point2d3);
        //use checksameside to check whether the point in in the same side
        bool CheckSameSide(Point2D centroid, Point2D point2d1, Point2D point2d2, Point2D point2d3);


        //Facet change by scanning
        //Change the Facet on the part to voxel by using Scanning
        void FacetChange_Scanning(OctreePoint point1, OctreePoint point2, OctreePoint point3);
        //facet parallel to surface, which means all the voxels will have a same coordinate
        void ParallelToSurfaceFacet_Scanning(OctreePoint point1, OctreePoint point2, OctreePoint point3, int serial);
        //facet perpendicular to surface, which means one coordinate of the normal is 0
        void PerpendicularToSurfaceFacet_Scanning(OctreePoint point1, OctreePoint point2, OctreePoint point3, int serial, int max_normal);
        //the facet with general location
        void GeneralLocationFacet_Scanning(OctreePoint point1, OctreePoint point2, OctreePoint point3,int Normal_x,int Normal_y,int Normal_z,int Normalnum);
        //use scanning to fill the 2dfacet
        void Scanning2D(void);
        //calculate the voxels to be fill
        void CoverVoxel2D_Scanning(OctreePoint point1,OctreePoint point2,OctreePoint point3,int serial);
    private:
        float xmax,xmin,ymax,ymin,zmax,zmin;
        vector<unsigned int> surface_point_num; //the number of the voxels in all layers
        vector<int> model_impact_factor; //the number of the voxels in all layers
        set<Point2D> opoint2D; //to save the point of the 2d Bresenham line
};

