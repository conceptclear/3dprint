#include "Voxelization.h"
using namespace std;

void Voxelization::EdgeChange_Bresenham(OctreePoint point1,OctreePoint point2)
{
    if((point1.x==point2.x)&&(point1.y==point2.y)&&(point1.z==point2.z))return;
    else if((point1.x==point2.x)&&(point1.y==point2.y))
        PerpendicularToSurfaceEdge(point1,point2,3);
    else if((point1.y==point2.y)&&(point1.z==point2.z))
        PerpendicularToSurfaceEdge(point1,point2,1);
    else if((point1.x==point2.x)&&(point1.z==point2.z))
        PerpendicularToSurfaceEdge(point1,point2,2);
    else if(point1.x==point2.x)
        ParallelToSurfaceEdge_Bresenham(point1,point2,1);
    else if(point1.y==point2.y)
        ParallelToSurfaceEdge_Bresenham(point1,point2,2);
    else if(point1.z==point2.z)
        ParallelToSurfaceEdge_Bresenham(point1,point2,3);
    else
        GeneralLocationEdge_Bresenham(point1,point2);
}

void Voxelization::PerpendicularToSurfaceEdge(OctreePoint point1,OctreePoint point2, int serial)
{
    int diffx=point1.x-point2.x;
    int diffy=point1.y-point2.y;
    int diffz=point1.z-point2.z;
    int nummax,nummin;
    switch(serial)
    {
        case 1:
            {
                nummax=diffx>0?point1.x:point2.x;
                nummin=diffx>0?point2.x:point1.x;
                for(int i = nummin+1;i<nummax;i++)
                {
                    SurfacePoint(i,point1.y,point1.z);
                }
            }
            break;
        case 2:
            {
                nummax=diffy>0?point1.y:point2.y;
                nummin=diffy>0?point2.y:point1.y;
                for(int i = nummin+1;i<nummax;i++)
                {
                    SurfacePoint(point1.x,i,point1.z);
                }
            }
            break;
        case 3:
            {
                nummax=diffz>0?point1.z:point2.z;
                nummin=diffz>0?point2.z:point1.z;
                for(int i = nummin+1;i<nummax;i++)
                {
                    SurfacePoint(point1.x,point1.y,i);
                }
            }
            break;
        default:
            cout<<"error occured in PerpendicularToSurfaceEdge"<<endl;
            break;
    }
}

void Voxelization::ParallelToSurfaceEdge_Bresenham(OctreePoint point1, OctreePoint point2, int serial)
{
    int diffx = point1.x-point2.x;
    int diffy = point1.y-point2.y;
    int diffz = point1.z-point2.z;
    int sign_x = diffx>0?1:-1;
    int sign_y = diffy>0?1:-1;
    int sign_z = diffz>0?1:-1;
    unsigned int strx = point2.x;
    unsigned int stry = point2.y;
    unsigned int strz = point2.z;
    int eps = 0; //eps is cumulative error
    diffx = abs(diffx);
    diffy = abs(diffy);
    diffz = abs(diffz); //can use sign to judge positive and negative
    switch(serial)
    {
        case 1:
            {
                if(diffy>=diffz)
                {
                    for(;stry!=point1.y-sign_y;)
                    {
                        stry+=sign_y;
                        eps+=diffz;
                        if((eps<<1)>=diffy)
                        {
                            strz+=sign_z;
                            eps-=diffy;
                        }
                        SurfacePoint(strx,stry,strz);
                    }
                }
                else
                {
                     for(;strz!=point1.z-sign_z;)
                    {
                        strz+=sign_z;
                        eps+=diffy;
                        if((eps<<1)>=diffz)
                        {
                            stry+=sign_y;
                            eps-=diffz;
                        }
                        SurfacePoint(strx,stry,strz);
                    }
                }
            }
            break;
        case 2:
            {
                if(diffx>=diffz)
                {
                    for(;strx!=point1.x-sign_x;)
                    {
                        strx+=sign_x;
                        eps+=diffz;
                        if((eps<<1)>=diffx)
                        {
                            strz+=sign_z;
                            eps-=diffx;
                        }
                        SurfacePoint(strx,stry,strz);
                    }
                }
                else
                {
                     for(;strz!=point1.z-sign_z;)
                    {
                        strz+=sign_z;
                        eps+=diffx;
                        if((eps<<1)>=diffz)
                        {
                            strx+=sign_x;
                            eps-=diffz;
                        }
                        SurfacePoint(strx,stry,strz);
                    }
                }
            }
            break;
        case 3:
            {
                if(diffx>=diffy)
                {
                    for(;strx!=point1.x-sign_x;)
                    {
                        strx+=sign_x;
                        eps+=diffy;
                        if((eps<<1)>=diffx)
                        {
                            stry+=sign_y;
                            eps-=diffx;
                        }
                        SurfacePoint(strx,stry,strz);
                    }
                }
                else
                {
                     for(;stry!=point1.y-sign_y;)
                    {
                        stry+=sign_y;
                        eps+=diffx;
                        if((eps<<1)>=diffy)
                        {
                            strx+=sign_x;
                            eps-=diffy;
                        }
                        SurfacePoint(strx,stry,strz);
                    }
                }
            }
            break;
        default:
            cout<<"error occured in ParallelToSurfaceEdge_Bresenham"<<endl;
            break;
    }
}

void Voxelization::GeneralLocationEdge_Bresenham(OctreePoint point1, OctreePoint point2)
{
    int diffx = point1.x-point2.x;
    int diffy = point1.y-point2.y;
    int diffz = point1.z-point2.z;
    int sign_x = diffx>0?1:-1;
    int sign_y = diffy>0?1:-1;
    int sign_z = diffz>0?1:-1;
    unsigned int strx = point2.x;
    unsigned int stry = point2.y;
    unsigned int strz = point2.z;
    int epsx = 0; //eps is cumulative error
    int epsy = 0; //eps is cumulative error
    int epsz = 0; //eps is cumulative error
    diffx = abs(diffx);
    diffy = abs(diffy);
    diffz = abs(diffz); //can use sign to judge positive and negative
    int diffmax = max(max(diffx,diffy),diffz);
    if(diffmax==diffx)
    {
        for(;strx!=point1.x-sign_x;)
        {
            strx+=sign_x;
            epsy+=diffy;
            epsz+=diffz;
            if((epsy<<1)>=diffx)
            {
                stry+=sign_y;
                epsy-=diffx;
            }
            if((epsz<<1)>=diffx)
            {
                strz+=sign_z;
                epsz-=diffx;
            }
            SurfacePoint(strx,stry,strz);
        }
    }
    else if(diffmax==diffy)
    {
        for(;stry!=point1.y-sign_y;)
        {
            stry+=sign_y;
            epsx+=diffx;
            epsz+=diffz;
            if((epsx<<1)>=diffy)
            {
                strx+=sign_x;
                epsx-=diffy;
            }
            if((epsz<<1)>=diffy)
            {
                strz+=sign_z;
                epsz-=diffy;
            }
            SurfacePoint(strx,stry,strz);
        }
    }
    else 
    {
        for(;strz!=point1.z-sign_z;)
        {
            strz+=sign_z;
            epsx+=diffx;
            epsy+=diffy;
            if((epsx<<1)>=diffz)
            {
                strx+=sign_x;
                epsx-=diffz;
            }
            if((epsy<<1)>=diffz)
            {
                stry+=sign_y;
                epsy-=diffz;
            }
            SurfacePoint(strx,stry,strz);
        }
    }
}
