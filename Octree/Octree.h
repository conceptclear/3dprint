/*********************************************************************************
  *Copyright(C),2017-2017,conceptclear
  *FileName:  Octree.h
  *Author:  conceptclear
  *Version:  1.0
  *Date:  2017.12.5
  *Description:  use this class to create an octree
**********************************************************************************/
#pragma once

#include <iostream>
#include <string>
#include "../basic components/CEdge.h"
#include "../basic components/CFacet.h"
#include "../basic components/CVertex.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <set>
using namespace std;

class OctreePoint
{
    public:
        OctreePoint(unsigned int _x,unsigned int _y,unsigned int _z):x(_x),y(_y),z(_z){}
        OctreePoint(const OctreePoint& copy)
        {
            x=copy.x;
            y=copy.y;
            z=copy.z;
        }
        bool operator==(const OctreePoint &point)const;
        bool operator<(const OctreePoint &point)const;
        OctreePoint operator+(const OctreePoint &point)const;
        friend class Octree;
        friend class Voxelization;
    private:
        unsigned int x;
        unsigned int y;
        unsigned int z;
};

inline bool OctreePoint:: operator==(const OctreePoint &point)const
{
    return ((x==point.x)&&(y==point.y)&&(z==point.z));
}

inline bool OctreePoint::operator<(const OctreePoint &point)const
{
    if(point.z>z)
        return true;
    else if(point.z==z)
    {
        if(point.y>y)
            return true;
        else if(point.y==y)
        {
            if(point.x>x)
                return true;
            else
                return false;
        }
        else
            return false;
    }
    else
        return false;
}

inline OctreePoint OctreePoint::operator+(const OctreePoint &point)const
{
    OctreePoint out(x+point.x,y+point.y,z+point.z);
    return out;
}

class OctreeNode
{
    //declare the octree node class
    //define back as 0,front as 1,left as 0,right as 1,bottom as 0,top as 1
    //use right hand spiral rules
    //define x as back and front direction, y as left and front direciton, z as top and bottom direction
    //as an example, Front_Left_Top means 101,ChangeToDecimal is 5. So, the orderstr in this depth equals 5
    public:
        friend class Octree;
        friend class Voxelization;
        OctreeNode():
            Back_Left_Bottom(NULL),Back_Left_Top(NULL),
            Back_Right_Bottom(NULL),Back_Right_Top(NULL),
            Front_Left_Bottom(NULL),Front_Left_Top(NULL),
            Front_Right_Bottom(NULL),Front_Right_Top(NULL){}
        OctreeNode
            (unsigned int strx=0, unsigned int stry=0,
             unsigned int strz=0, int node_depth = 0 ,
             int nodeflag=1,int node_pointup=0,
             int node_pointdown=0,
             OctreeNode *back_left_bottom =NULL,
             OctreeNode *back_left_top =NULL,
             OctreeNode *back_right_bottom =NULL,
             OctreeNode *back_right_top =NULL,
             OctreeNode *front_left_bottom =NULL,
             OctreeNode *front_left_top =NULL,
             OctreeNode *front_right_bottom =NULL,
             OctreeNode *front_right_top =NULL):
                orderstrx(strx),orderstry(stry),
                orderstrz(strz),depth(node_depth),
                flag(nodeflag),pointup(node_pointup),
                pointdown(node_pointdown),
                Back_Left_Bottom(back_left_bottom),
                Back_Left_Top(back_left_top),
                Back_Right_Bottom(back_right_bottom),
                Back_Right_Top(back_right_top),
                Front_Left_Bottom(front_left_bottom),
                Front_Left_Top(front_left_top),
                Front_Right_Bottom(front_right_bottom),
                Front_Right_Top(front_right_top){}
        ~OctreeNode(){}
    private:
        //node coding which can define the location of this node in Octree
        unsigned int orderstrx; 
        unsigned int orderstry; 
        unsigned int orderstrz; 
        //use depth to know the depth
        int depth;
        //use -1 to define the node out of the part
        //use 0 to define the node on the part
        //use 1 to define the node in the part
        int flag;
        //use pointup and pointdown to consider the influence
        int pointup;
        int pointdown;
        OctreeNode *Back_Left_Bottom;
        OctreeNode *Back_Left_Top;
        OctreeNode *Back_Right_Bottom;
        OctreeNode *Back_Right_Top;
        OctreeNode *Front_Left_Bottom;
        OctreeNode *Front_Left_Top;
        OctreeNode *Front_Right_Bottom;
        OctreeNode *Front_Right_Top;
}; 

class Octree
{
    public:
        Octree()
        {
            root = new OctreeNode(0,0,0,0,1);
            max_height = 1;
        }
        Octree(unsigned int x,unsigned int y,unsigned int z,int depth)
        {
            root = new OctreeNode(x,y,z,depth,1);
            max_height = 1;
        }
        ~Octree()
        {
            DeleteRoot(root);
        }
        //set up an octree with all nodes out of the part
        void MakeOctree(int height);
        //use perorder traversal
        void Traverse(void);
        int IsEmpty()
        {return root==NULL?1:0;}
        void GetPositionData(float* positionData); //get positiondata
        void GetBarycentricData(float* barycentricData); //get barycentricdata
        set<OctreePoint> point_on_surface_on;
        set<OctreePoint> point_on_surface_in;
        set<OctreePoint> point_on_surface_out;
   protected:
        //use iteration to delete the root
        void DeleteRoot(OctreeNode *pRoot);
        //use iteration to create root
        void MakeRoot(OctreeNode *pRoot);
        void PreOrder(OctreeNode *pRoot); //traverse the octree
        void GetPoint(OctreeNode *pRoot); //draw
        int GetUp(OctreeNode *pRoot); //get the flag num from the up voxel
        int GetDown(OctreeNode *pRoot); //get the flag num from the down voxel
        void SetImpactFactor(OctreeNode *pRoot); //set pointup and pointdown
        OctreeNode *root;
        int layer_pointup;
        int layer_pointdown;
        int max_height;
};

class Point2D
{
    public:
        Point2D(unsigned int _x=0,unsigned int _y=0):x(_x),y(_y){}
        ~Point2D(){}
        bool operator<(const Point2D &a)const
        {
            if(a.x<x)
                return true;
            else if(a.x==x)
            {
                if(a.y<y)
                    return true;
                else
                    return false;
            }
            else
                return false;
        }
        Point2D operator+(const Point2D &a)const
        {
            return Point2D(x+a.x,y+a.y);
        }
        Point2D operator-(const Point2D &a)const
        {
            return Point2D(x-a.x,y-a.y);
        }
        friend class Octree;
        friend class Voxelization;
    private:
        unsigned int x;
        unsigned int y;
};
