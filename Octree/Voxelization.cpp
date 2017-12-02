#include "Voxelization.h"
using namespace std;

void Voxelization::GetExtremum(float x_max,float x_min,float y_max,float y_min,float z_max,float z_min)
{
    xmax=x_max;
    xmin=x_min;
    ymax=y_max;
    ymin=y_min;
    zmax=z_max;
    zmin=z_min;
}

void Voxelization::SurfacePoint(unsigned int x, unsigned int y, unsigned int z)
{
    OctreeNode* node = root;
    int x_direction; 
    int y_direction; 
    int z_direction; 
    int position;
    for(int j=0;j<max_height-1;j++)
    {
        //use bit manipulation to change to binary
        x_direction = x>>(max_height-2-j)&1;
        y_direction = y>>(max_height-2-j)&1;
        z_direction = z>>(max_height-2-j)&1;
        position=x_direction*4+y_direction*2+z_direction;
        //            cout<<position<<endl;
        switch(position)
        {
            case 0:
                node=node->Back_Left_Bottom;
                break;
            case 1:
                node=node->Back_Left_Top;
                break;
            case 2:
                node=node->Back_Right_Bottom;
                break;
            case 3:
                node=node->Back_Right_Top;
                break;
            case 4:
                node=node->Front_Left_Bottom;
                break;
            case 5:
                node=node->Front_Left_Top;
                break;
            case 6:
                node=node->Front_Right_Bottom;
                break;
            case 7:
                node=node->Front_Right_Top;
                break;
            default:
                cerr<<"error"<<endl;
                break;
        }
    }
    node->flag=0;
    //            cout<<node->orderstr<<endl;
}

unsigned int Voxelization::ChangeCoordinate(float coordinate,float max,float min)
{
    unsigned int data = (coordinate-min)*pow(2,max_height-1)/(max-min);
    //    cout<<data<<endl;
    unsigned int output = CheckNum(data);
    return output;
}

unsigned int Voxelization::CheckNum(unsigned int num)
{
    if(num==pow(2,max_height-1))
        num--;
    return num;
}


void Voxelization::PointToVoxel(vector<CVertex> VectorPoint)
{
    for(int i=0;i<int(VectorPoint.size());i++)
    {
        unsigned int voxelx = ChangeCoordinate(VectorPoint[i].m_Point[0],xmax,xmin);
        unsigned int voxely = ChangeCoordinate(VectorPoint[i].m_Point[1],ymax,ymin);
        unsigned int voxelz = ChangeCoordinate(VectorPoint[i].m_Point[2],zmax,zmin);
        //        cout<<binaryx<<binaryy<<binaryz<<endl;
        SurfacePoint(voxelx,voxely,voxelz);
    }
}

void Voxelization::EdgeToVoxel(vector<CEdge> VectorEdge, vector<CVertex> VectorPoint)
{
    for(int i=0;i<int(VectorEdge.size());i++)
    {
        //change the coordinate system
        unsigned int strx1 = ChangeCoordinate(VectorPoint[VectorEdge[i].PointIndex[0]].m_Point[0],xmax,xmin);
        unsigned int strx2 = ChangeCoordinate(VectorPoint[VectorEdge[i].PointIndex[1]].m_Point[0],xmax,xmin);
        unsigned int stry1 = ChangeCoordinate(VectorPoint[VectorEdge[i].PointIndex[0]].m_Point[1],ymax,ymin);
        unsigned int stry2 = ChangeCoordinate(VectorPoint[VectorEdge[i].PointIndex[1]].m_Point[1],ymax,ymin);
        unsigned int strz1 = ChangeCoordinate(VectorPoint[VectorEdge[i].PointIndex[0]].m_Point[2],zmax,zmin);
        unsigned int strz2 = ChangeCoordinate(VectorPoint[VectorEdge[i].PointIndex[1]].m_Point[2],zmax,zmin);
        OctreePoint point1(strx1,stry1,strz1);
        OctreePoint point2(strx2,stry2,strz2);
        EdgeChange_Bresenham(point1,point2);
    }
}

void Voxelization::FacetToVoxel(vector<CFacet> VectorFacet, vector<CVertex> VectorPoint)
{
    for(int i=0;i<int(VectorFacet.size());i++)
    {
        //change the coordinate system
        unsigned int strx1 = ChangeCoordinate(VectorPoint[VectorFacet[i].PointIndex[0]].m_Point[0],xmax,xmin);
        unsigned int strx2 = ChangeCoordinate(VectorPoint[VectorFacet[i].PointIndex[1]].m_Point[0],xmax,xmin);
        unsigned int strx3 = ChangeCoordinate(VectorPoint[VectorFacet[i].PointIndex[2]].m_Point[0],xmax,xmin);
        unsigned int stry1 = ChangeCoordinate(VectorPoint[VectorFacet[i].PointIndex[0]].m_Point[1],ymax,ymin);
        unsigned int stry2 = ChangeCoordinate(VectorPoint[VectorFacet[i].PointIndex[1]].m_Point[1],ymax,ymin);
        unsigned int stry3 = ChangeCoordinate(VectorPoint[VectorFacet[i].PointIndex[2]].m_Point[1],ymax,ymin);
        unsigned int strz1 = ChangeCoordinate(VectorPoint[VectorFacet[i].PointIndex[0]].m_Point[2],zmax,zmin);
        unsigned int strz2 = ChangeCoordinate(VectorPoint[VectorFacet[i].PointIndex[1]].m_Point[2],zmax,zmin);
        unsigned int strz3 = ChangeCoordinate(VectorPoint[VectorFacet[i].PointIndex[2]].m_Point[2],zmax,zmin);
        OctreePoint point1(strx1,stry1,strz1);
        OctreePoint point2(strx2,stry2,strz2);
        OctreePoint point3(strx3,stry3,strz3);
        if((point1==point2)||(point2==point3)||(point1==point3))
            return;
        //get the normal of the facet
        float Normal_x=VectorFacet[i].m_Normal[0];
        float Normal_y=VectorFacet[i].m_Normal[1];
        float Normal_z=VectorFacet[i].m_Normal[2];
        //the function of this facet can be given by:
        //Normal_x*(x-point1.x)+Normal_y*(y-point1.y)+Normal_z*(z-point1.z)=0
        //z=point1.z+Normal_x/Normal_z*(x-point1.x)+Normal_y/Normal_z*(y-point1.y)
        if(abs(Normal_x)==1)
            ParallelToSurfaceFacet(point1,point2,point3,1);
        else if(abs(Normal_y)==1)
            ParallelToSurfaceFacet(point1,point2,point3,2);
        else if(abs(Normal_z)==1)
            ParallelToSurfaceFacet(point1,point2,point3,3);
    }
}

void Voxelization::EdgeChange_Bresenham(OctreePoint point1,OctreePoint point2)
{
    if((point1.x==point2.x)&&(point1.y==point2.y)&&(point1.z==point2.z))return;
    if((point1.x==point2.x)&&(point1.y==point2.y))
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
        
void Voxelization::ParallelToSurfaceFacet(OctreePoint point1, OctreePoint point2, OctreePoint point3, int serial)
{
    opoint2D.clear();
    strpoint2d.clear();
    switch(serial)
    {
        case 1:
            {
                Point2D point2d1(point1.y,point1.z);
                Point2D point2d2(point2.y,point2.z);
                Point2D point2d3(point3.y,point3.z);
                Bresenham2D(point2d1,point2d2);
                Bresenham2D(point2d1,point2d3);
                Bresenham2D(point2d2,point2d3);
                //the centroid of the triangle must be in this triangle
                //so the centroid can be used to set as the seed
                Point2D centroid((point1.y+point2.y+point3.y)/3,(point1.z+point2.z+point3.z)/3);
                //due to the rounding error,centroid could be out of the triangle
                //so we should check it whether it is in the triangle
                FloodSeedFill2D(centroid);
                for(unsigned long i=0;i<strpoint2d.size();i++)
                {
                    SurfacePoint(point1.x,strpoint2d[i].x,strpoint2d[i].y);
                }
            }
            break;
        case 2:
            {
                Point2D point2d1(point1.x,point1.z);
                Point2D point2d2(point2.x,point2.z);
                Point2D point2d3(point3.x,point3.z);
                Bresenham2D(point2d1,point2d2);
                Bresenham2D(point2d1,point2d3);
                Bresenham2D(point2d2,point2d3);
                //the centroid of the triangle must be in this triangle
                //so the centroid can be used to set as the seed
                Point2D centroid((point1.x+point2.x+point3.x)/3,(point1.z+point2.z+point3.z)/3);
                FloodSeedFill2D(centroid);
                for(unsigned long i=0;i<strpoint2d.size();i++)
                {
                    SurfacePoint(strpoint2d[i].x,point1.y,strpoint2d[i].y);
                }
            }
            break;
        case 3:
            {
                Point2D point2d1(point1.x,point1.y);
                Point2D point2d2(point2.x,point2.y);
                Point2D point2d3(point3.x,point3.y);
                Bresenham2D(point2d1,point2d2);
                Bresenham2D(point2d1,point2d3);
                Bresenham2D(point2d2,point2d3);
                //the centroid of the triangle must be in this triangle
                //so the centroid can be used to set as the seed
                Point2D centroid((point1.x+point2.x+point3.x)/3,(point1.y+point2.y+point3.y)/3);
                FloodSeedFill2D(centroid);
                for(unsigned long i=0;i<strpoint2d.size();i++)
                {
                    SurfacePoint(strpoint2d[i].x,strpoint2d[i].y,point1.z);
                }
            }
            break;
        default:
            cout<<"error occured in ParallelToSurfaceFacet"<<endl;
            break;
    }
}

void Voxelization::Bresenham2D(Point2D pt2d1, Point2D pt2d2)
{
    opoint2D.insert(pt2d1);
    opoint2D.insert(pt2d2);
    int diffx = pt2d1.x-pt2d2.x;
    int diffy = pt2d1.y-pt2d2.y;
    int sign_x = diffx>0?1:-1;
    int sign_y = diffy>0?1:-1;
    Point2D str2d(pt2d2.x,pt2d2.y);
    int eps = 0; //eps is cumulative error
    diffx = abs(diffx);
    diffy = abs(diffy);//can use sign to judge positive and negative
    if(diffx>=diffy)
    {
        for(;str2d.x!=pt2d1.x-sign_x;)
        {
            str2d.x+=sign_x;
            eps+=diffy;
            if((eps<<1)>=diffx)
            {
                str2d.y+=sign_y;
                eps-=diffx;
            }
            opoint2D.insert(str2d);
        }
    }
    else
    {
        for(;str2d.y!=pt2d1.y-sign_y;)
        {
            str2d.y+=sign_y;
            eps+=diffx;
            if((eps<<1)>=diffy)
            {
                str2d.x+=sign_x;
                eps-=diffy;
            }
            opoint2D.insert(str2d);
        }
    }
}
 
void Voxelization::FloodSeedFill2D(Point2D point2d)
{
    if(opoint2D.find(point2d)==opoint2D.end())
    {
        int x[4]={-1,0,1,0};
        int y[4]={0,1,0,-1};
        strpoint2d.push_back(point2d);
        opoint2D.insert(point2d);
        for(int i=0;i<4;i++)
        {
            Point2D fpoint(point2d.x+x[i],point2d.y+y[i]);
            FloodSeedFill2D(fpoint);
        }
    }
}
