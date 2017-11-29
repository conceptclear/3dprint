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

float Voxelization::ChangeCoordinate(float coordinate,float max,float min)
{
    float data = (coordinate-min)*pow(2,max_height-1)/(max-min);
    //    cout<<data<<endl;
    return data;
}

float Voxelization::CheckPoint(float num)
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
        voxelx = CheckPoint(voxelx);
        voxely = CheckPoint(voxely);
        voxelz = CheckPoint(voxelz);
        //        cout<<binaryx<<binaryy<<binaryz<<endl;
        SurfacePoint(voxelx,voxely,voxelz);
    }
}
