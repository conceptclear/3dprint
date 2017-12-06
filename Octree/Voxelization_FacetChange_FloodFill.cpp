#include "Voxelization.h"
using namespace std;

void Voxelization::FacetChange_FloodFill(OctreePoint point1, OctreePoint point2, OctreePoint point3)
{
    int v1x=point2.x-point1.x;
    int v1y=point2.y-point1.y;
    int v1z=point2.z-point1.z;
    int v2x=point3.x-point2.x;
    int v2y=point3.y-point2.y;
    int v2z=point3.z-point2.z;
    int Normal_x=v1y*v2z-v1z*v2y;
    int Normal_y=v1z*v2x-v2z*v1x;
    int Normal_z=v1x*v2y-v1y*v2x;
    int Normal_max=max(max(abs(Normal_x),abs(Normal_y)),abs(Normal_z));
    int Normalnum=0;
    if(Normal_max==abs(Normal_x))
        Normalnum=1;
    else if(Normal_max==abs(Normal_y))
        Normalnum=2;
    else
        Normalnum=3;
    //the function of this facet can be given by:
    //Normal_x*(x-point1.x)+Normal_y*(y-point1.y)+Normal_z*(z-point1.z)=0
    //z=point1.z+Normal_x/Normal_z*(x-point1.x)+Normal_y/Normal_z*(y-point1.y)
    //due to the accuracy of the floating point, we use minus to check
    if(Normal_y==0&&Normal_z==0)
        ParallelToSurfaceFacet_FloodFill(point1,point2,point3,1);
    else if(Normal_x==0&&Normal_z==0)
        ParallelToSurfaceFacet_FloodFill(point1,point2,point3,2);
    else if(Normal_y==0&&Normal_z==0)
        ParallelToSurfaceFacet_FloodFill(point1,point2,point3,3);
    else if(Normal_x==0)
        PerpendicularToSurfaceFacet_FloodFill(point1,point2,point3,1,Normalnum);
    else if(Normal_y==0)
        PerpendicularToSurfaceFacet_FloodFill(point1,point2,point3,2,Normalnum);
    else if(Normal_z==0)
        PerpendicularToSurfaceFacet_FloodFill(point1,point2,point3,3,Normalnum);
    else
        //due to the rounding error, we should recalculate the normal vector
        GeneralLocationFacet_FloodFill(point1,point2,point3,Normal_x,Normal_y,Normal_z,Normalnum);
}

void Voxelization::ParallelToSurfaceFacet_FloodFill(OctreePoint point1, OctreePoint point2, OctreePoint point3, int serial)
{
    opoint2D.clear();
    set<Point2D>::iterator it;
    switch(serial)
    {
        case 1:
            {
                CoverVoxel2D_FloodFill(point1,point2,point3,1);
                for(it=opoint2D.begin();it!=opoint2D.end();it++)
                {
                    SurfacePoint(point1.x,it->x,it->y);
                }
            }
            break;
        case 2:
            {
                CoverVoxel2D_FloodFill(point1,point2,point3,2);
                for(it=opoint2D.begin();it!=opoint2D.end();it++)
                {
                    SurfacePoint(it->x,point1.y,it->y);
                }
            }
            break;
        case 3:
            {
                CoverVoxel2D_FloodFill(point1,point2,point3,3);
                for(it=opoint2D.begin();it!=opoint2D.end();it++)
                {
                    SurfacePoint(it->x,it->y,point1.z);
                }
            }
            break;
        default:
            cout<<"error occured in ParallelToSurfaceFacet_FloodFill"<<endl;
            break;
    }
}

void Voxelization::PerpendicularToSurfaceFacet_FloodFill(OctreePoint point1, OctreePoint point2, OctreePoint point3, int serial, int max_normal)
{
    opoint2D.clear();
    set<Point2D>::iterator it;
    float k;
    int diffx;
    int diffy;
    int diffz;
    switch(serial)
    {
        case 1:
            {
                if(max_normal==2)
                {
                    CoverVoxel2D_FloodFill(point1,point2,point3,2);
                    if(point2.z-point1.z==0)
                    {
                        diffz=point3.z-point1.z;
                        diffy=point3.y-point1.y;
                        k=1.0*diffy/diffz;
                    }
                    else
                    {
                        diffz=point2.z-point1.z;
                        diffy=point2.y-point1.y;
                        k=1.0*diffy/diffz;
                    }
                    //y=k*(z-z1)+y1
                    for(it=opoint2D.begin();it!=opoint2D.end();it++)
                    {
                        SurfacePoint(it->x,(k*int(it->y-point1.z)+point1.y),it->y);
                    }
                }
                else
                {
                    CoverVoxel2D_FloodFill(point1,point2,point3,3);
                    if(point2.y-point1.y==0)
                    {
                        diffz=point3.z-point1.z;
                        diffy=point3.y-point1.y;
                        k=1.0*diffz/diffy;
                    }
                    else
                    {
                        diffz=point2.z-point1.z;
                        diffy=point2.y-point1.y;
                        k=1.0*diffz/diffy;
                    }
                    //z=k*(y-y1)+z1
                    for(it=opoint2D.begin();it!=opoint2D.end();it++)
                    {
                        SurfacePoint(it->x,it->y,(k*int(it->y-point1.y)+point1.z));
                    }
                }
            }
            break;
        case 2:
            {
                if(max_normal==1)
                {
                    CoverVoxel2D_FloodFill(point1,point2,point3,1);
                    if(point2.z-point1.z==0)
                    {
                        diffz=point3.z-point1.z;
                        diffx=point3.x-point1.x;
                        k=1.0*diffx/diffz;
                    }
                    else
                    {
                        diffz=point2.z-point1.z;
                        diffx=point2.x-point1.x;
                        k=1.0*diffx/diffz;
                    }
                    //x=k*(z-z1)+x1
                    for(it=opoint2D.begin();it!=opoint2D.end();it++)
                    {
                        SurfacePoint((k*int(it->y-point1.z)+point1.x),it->x,it->y);
                    }
                }
                else
                {
                    CoverVoxel2D_FloodFill(point1,point2,point3,3);
                    if(point2.x-point1.x==0)
                    {
                        diffz=point3.z-point1.z;
                        diffx=point3.x-point1.x;
                        k=1.0*diffz/diffx;
                    }
                    else
                    {
                        diffz=point2.z-point1.z;
                        diffx=point2.x-point1.x;
                        k=1.0*diffz/diffx;
                    }
                    //z=k*(x-x1)+z1
                    for(it=opoint2D.begin();it!=opoint2D.end();it++)
                    {
                        SurfacePoint(it->x,it->y,(k*int(it->x-point1.x)+point1.z));
                    }
                }
            }
            break;
        case 3:
            {
                if(max_normal==1)
                {
                    CoverVoxel2D_FloodFill(point1,point2,point3,1);
                    if(point2.y-point1.y==0)
                    {
                        diffx=point3.x-point1.x;
                        diffy=point3.y-point1.y;
                        k=1.0*diffx/diffy;
                    }
                    else
                    {
                        diffx=point2.x-point1.x;
                        diffy=point2.y-point1.y;
                        k=1.0*diffx/diffy;
                    }
                    //x=k*(y-y1)+x1
                    for(it=opoint2D.begin();it!=opoint2D.end();it++)
                    {
                        SurfacePoint((k*int(it->x-point1.y)+point1.x),it->x,it->y);
                    }
                }
                else
                {
                    CoverVoxel2D_FloodFill(point1,point2,point3,2);
                    if(point2.x-point1.x==0)
                    {
                        diffy=point3.y-point1.y;
                        diffx=point3.x-point1.x;
                        k=1.0*diffy/diffx;
                    }
                    else
                    {
                        diffy=point2.y-point1.y;
                        diffx=point2.x-point1.x;
                        k=1.0*diffy/diffx;
                    }
                    //y=k*(x-x1)+y1
                    for(it=opoint2D.begin();it!=opoint2D.end();it++)
                    {
                        SurfacePoint(it->x,(k*int(it->x-point1.x)+point1.y),it->y);
                    }
                }
            }
            break;
        default:
            cout<<"error occured in PerpendicularToSurfaceFacet_FloodFill"<<endl;
            break;
    }
}

void Voxelization::GeneralLocationFacet_FloodFill(OctreePoint point1, OctreePoint point2, OctreePoint point3,int Normal_x, int Normal_y, int Normal_z, int Normalnum)
{
    opoint2D.clear();
    set<Point2D>::iterator it;
    int pointx,pointy,pointz;
    switch(Normalnum)
    {
        case 1:
            {
                CoverVoxel2D_FloodFill(point1,point2,point3,1);
                for(it=opoint2D.begin();it!=opoint2D.end();it++)
                {
                    pointx=(Normal_y*int(point1.y-it->x)+Normal_z*int(point1.z-it->y))/Normal_x+point1.x;
                    SurfacePoint(pointx,it->x,it->y);
                }
            }
            break;
        case 2:
            {
                CoverVoxel2D_FloodFill(point1,point2,point3,2);
                for(it=opoint2D.begin();it!=opoint2D.end();it++)
                {
                    pointy=(Normal_x*int(point1.x-it->x)+Normal_z*int(point1.z-it->y))/Normal_y+point1.y;
                    SurfacePoint(it->x,pointy,it->y);
                }
            }
            break;
        case 3:
            {
                CoverVoxel2D_FloodFill(point1,point2,point3,3);
                for(it=opoint2D.begin();it!=opoint2D.end();it++)
                {
                    pointz=(Normal_x*int(point1.x-it->x)+Normal_y*int(point1.y-it->y))/Normal_z+point1.z;
                    SurfacePoint(it->x,it->y,pointz);
                }
            }
            break;
        default:
            cout<<"error occured in GeneralLocationFacet_FloodFill"<<endl;
            break;
    }
}

void Voxelization::FloodSeedFill2D(Point2D point2d)
{
    if(opoint2D.find(point2d)==opoint2D.end())
    {
        int x[4]={-1,0,1,0};
        int y[4]={0,1,0,-1};
        opoint2D.insert(point2d);
        for(int i=0;i<4;i++)
        {
            Point2D fpoint(point2d.x+x[i],point2d.y+y[i]);
            FloodSeedFill2D(fpoint);
        }
    }
}

void Voxelization::CoverVoxel2D_FloodFill(OctreePoint point1,OctreePoint point2,OctreePoint point3,int serial)
{
    switch(serial)
    {
        case 1:
            {
                //the centroid of the triangle must be in this triangle
                //so the centroid can be used to set as the seed
                Point2D centroid((point1.y+point2.y+point3.y)/3,(point1.z+point2.z+point3.z)/3);
                Point2D point2d1(point1.y,point1.z);
                Point2D point2d2(point2.y,point2.z);
                Point2D point2d3(point3.y,point3.z);
                //due to the rounding error,centroid could be out of the triangle
                //so we should check it whether it is in the triangle
                if(!CheckTriangle(centroid,point2d1,point2d2,point2d3))
                    break;
                SuperCoverLine2D(point2d1,point2d2);
                SuperCoverLine2D(point2d1,point2d3);
                SuperCoverLine2D(point2d2,point2d3);
                FloodSeedFill2D(centroid);
            }
            break;
        case 2:
            {
                //the centroid of the triangle must be in this triangle
                //so the centroid can be used to set as the seed
                Point2D centroid((point1.x+point2.x+point3.x)/3,(point1.z+point2.z+point3.z)/3);
                Point2D point2d1(point1.x,point1.z);
                Point2D point2d2(point2.x,point2.z);
                Point2D point2d3(point3.x,point3.z);
                //due to the rounding error,centroid could be out of the triangle
                //so we should check it whether it is in the triangle
                if(!CheckTriangle(centroid,point2d1,point2d2,point2d3))
                    break;
                SuperCoverLine2D(point2d1,point2d2);
                SuperCoverLine2D(point2d1,point2d3);
                SuperCoverLine2D(point2d2,point2d3);
                FloodSeedFill2D(centroid);
            }
            break;
        case 3:
            {
                //the centroid of the triangle must be in this triangle
                //so the centroid can be used to set as the seed
                Point2D centroid((point1.x+point2.x+point3.x)/3,(point1.y+point2.y+point3.y)/3);
                Point2D point2d1(point1.x,point1.y);
                Point2D point2d2(point2.x,point2.y);
                Point2D point2d3(point3.x,point3.y);
                //due to the rounding error,centroid could be out of the triangle
                //so we should check it whether it is in the triangle
                if(!CheckTriangle(centroid,point2d1,point2d2,point2d3))
                    break;
                SuperCoverLine2D(point2d1,point2d2);
                SuperCoverLine2D(point2d1,point2d3);
                SuperCoverLine2D(point2d2,point2d3);
                FloodSeedFill2D(centroid);
            }
            break;
        default:
            cout<<"error occured in CoverVoxel2D_FloodFill"<<endl;
            break;
    }
}

bool Voxelization::CheckTriangle(Point2D centroid, Point2D point2d1, Point2D point2d2, Point2D point2d3)
{
    return CheckSameSide(centroid,point2d1,point2d2,point2d3)&&CheckSameSide(centroid,point2d2,point2d3,point2d1)&&CheckSameSide(centroid,point2d3,point2d1,point2d2);
}

bool Voxelization::CheckSameSide(Point2D centroid, Point2D point2d1, Point2D point2d2, Point2D point2d3)
{
    Point2D vec1 = point2d2-point2d1;
    Point2D vec2 = point2d3-point2d1;
    Point2D vecc = centroid-point2d1;

    float f1 = vec1.x*vec2.y-vec1.y*vec2.x;
    float f2 = vec1.x*vecc.y-vec1.y*vecc.x;

    return f1*f2>=0;
}
