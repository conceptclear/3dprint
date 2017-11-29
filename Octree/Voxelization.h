#pragma once
#include "Octree.h"
#include <vector>
#include <cmath>

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
        void SurfacePoint(unsigned int x,unsigned int y,unsigned int z);
        float ChangeCoordinate(float coordinate,float max,float min);
        //to check if the num is at the max vertex
        float CheckPoint(float num);
    private:
        float xmax,xmin,ymax,ymin,zmax,zmin;
};

