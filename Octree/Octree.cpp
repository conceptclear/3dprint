#include "Octree.h"
using namespace std;

void Octree::MakeRoot(OctreeNode *pRoot)
{
    if(pRoot->depth<=1)return;
    unsigned int rootx = 2*pRoot->orderstrx;
    unsigned int rooty = 2*pRoot->orderstry;
    unsigned int rootz = 2*pRoot->orderstrz;
    pRoot->Back_Left_Bottom = new OctreeNode(rootx,rooty,rootz,pRoot->depth-1);
    pRoot->Back_Left_Top = new OctreeNode(rootx,rooty,rootz+1,pRoot->depth-1);
    pRoot->Back_Right_Bottom = new OctreeNode(rootx,rooty+1,rootz,pRoot->depth-1);
    pRoot->Back_Right_Top = new OctreeNode(rootx,rooty+1,rootz+1,pRoot->depth-1);
    pRoot->Front_Left_Bottom = new OctreeNode(rootx+1,rooty,rootz,pRoot->depth-1);
    pRoot->Front_Left_Top = new OctreeNode(rootx+1,rooty,rootz+1,pRoot->depth-1);
    pRoot->Front_Right_Bottom = new OctreeNode(rootx+1,rooty+1,rootz,pRoot->depth-1);
    pRoot->Front_Right_Top = new OctreeNode(rootx+1,rooty+1,rootz+1,pRoot->depth-1);
    MakeRoot(pRoot->Back_Left_Bottom);
    MakeRoot(pRoot->Back_Left_Top);
    MakeRoot(pRoot->Back_Right_Bottom);
    MakeRoot(pRoot->Back_Right_Top);
    MakeRoot(pRoot->Front_Left_Bottom);
    MakeRoot(pRoot->Front_Left_Top);
    MakeRoot(pRoot->Front_Right_Bottom);
    MakeRoot(pRoot->Front_Right_Top);
}

void Octree::MakeOctree(int height)
{
    if(max_height<height)
        max_height = height;
    if(max_height<1)
        cerr<<"error input"<<endl;
    else if(max_height==1)
    {
        return;
    }
    else
    {
        root->depth=height;
        MakeRoot(root);
    }
}

void Octree::DeleteRoot(OctreeNode *pRoot)
{
    if(pRoot==NULL)
        return;
    OctreeNode* back_left_bottom = pRoot->Back_Left_Bottom;
    OctreeNode* back_left_top = pRoot->Back_Left_Top;
    OctreeNode* back_right_bottom = pRoot->Back_Right_Bottom;
    OctreeNode* back_right_top = pRoot->Back_Right_Top;
    OctreeNode* front_left_bottom = pRoot->Front_Left_Bottom;
    OctreeNode* front_left_top = pRoot->Front_Left_Top;
    OctreeNode* front_right_bottom = pRoot->Front_Right_Bottom;
    OctreeNode* front_right_top = pRoot->Front_Right_Top;

    //    cout<<pRoot->orderstr<<endl;

    delete pRoot;
    if(back_left_bottom!=NULL)DeleteRoot(back_left_bottom);
    if(back_left_top!=NULL)DeleteRoot(back_left_top);
    if(back_right_bottom!=NULL)DeleteRoot(back_right_bottom);
    if(back_right_top!=NULL)DeleteRoot(back_right_top);
    if(front_left_bottom!=NULL)DeleteRoot(front_left_bottom);
    if(front_left_top!=NULL)DeleteRoot(front_left_top);
    if(front_right_bottom!=NULL)DeleteRoot(front_right_bottom);
    if(front_right_top!=NULL)DeleteRoot(front_right_top);
    return;
}

void Octree::Traverse(void)
{
    PreOrder(root);
}

void Octree::PreOrder(OctreeNode *pRoot)
{
    if(pRoot!=NULL)
    {
        GetPoint(pRoot);
        if(pRoot->depth==1)
            return;
        PreOrder(pRoot->Back_Left_Bottom);
        PreOrder(pRoot->Back_Left_Top);
        PreOrder(pRoot->Back_Right_Bottom);
        PreOrder(pRoot->Back_Right_Top);
        PreOrder(pRoot->Front_Left_Bottom);
        PreOrder(pRoot->Front_Left_Top);
        PreOrder(pRoot->Front_Right_Bottom);
        PreOrder(pRoot->Front_Right_Top);
    }
}

void Octree::GetPoint(OctreeNode *pRoot)
{
    if(pRoot->depth!=1)
    {
        return;
    }
    if(pRoot->flag!=0)
    {
        return;
    }
    else
    {
        SetImpactFactor(pRoot);
        OctreePoint opoint(pRoot->orderstrx,pRoot->orderstry,pRoot->orderstrz);
        if(pRoot->pointdown==0)
            point_on_surface_on.insert(opoint);
        else if(pRoot->pointdown==-1)
            point_on_surface_out.insert(opoint);
        else if(pRoot->pointdown==1)
            point_on_surface_in.insert(opoint);
        else
            cerr<<"error occured in GetPoint"<<endl;
    }
}

int Octree::GetUp(OctreeNode *pRoot)
{
    if(pRoot->depth!=1)
        return 0;
    else if(pRoot->flag!=0)
        return 0;
    else
    {
        unsigned int zup = pRoot->orderstrz+1;
        unsigned int yup = pRoot->orderstry;
        unsigned int xup = pRoot->orderstrx;
        OctreeNode* node = root;
        int x_direction; 
        int y_direction; 
        int z_direction; 
        int position;
        if(zup==(1<<(max_height-1)))
            return 0;
        for(int i=0;i<max_height-1;i++)
        {
            //use bit manipulation to change to binary
            x_direction = xup>>(max_height-2-i)&1;
            y_direction = yup>>(max_height-2-i)&1;
            z_direction = zup>>(max_height-2-i)&1;
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
        return node->flag;
    }
}

int Octree::GetDown(OctreeNode *pRoot)
{
    if(pRoot->depth!=1)
        return 0;
    else if(pRoot->flag!=0)
        return 0;
    else
    {
        unsigned int zdown = pRoot->orderstrz-1;
        unsigned int ydown = pRoot->orderstry;
        unsigned int xdown = pRoot->orderstrx;
        OctreeNode* node = root;
        int x_direction; 
        int y_direction; 
        int z_direction; 
        int position;
        if(pRoot->orderstrz==0)
            return 0;
        for(int i=0;i<max_height-1;i++)
        {
            //use bit manipulation to change to binary
            x_direction = xdown>>(max_height-2-i)&1;
            y_direction = ydown>>(max_height-2-i)&1;
            z_direction = zdown>>(max_height-2-i)&1;
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
        return node->flag;
    }
}

void Octree::SetImpactFactor(OctreeNode *pRoot)
{
    pRoot->pointup=GetUp(pRoot);
    pRoot->pointdown=GetDown(pRoot);
    layer_pointup+=pRoot->pointup;
    layer_pointdown+=pRoot->pointdown;
}

void Octree::GetPositionData(float* positionData)
{
    int x[4] = {0,1,1,0};
    int y[4] = {0,0,1,1};
    set<OctreePoint>::iterator iter;
    int i=0;
    for(iter=point_on_surface_on.begin();iter!=point_on_surface_on.end();iter++)
    {
        for(int j = 0;j < 8;j++)
        {
            positionData[72*i+3*j+0]=iter->x+x[j%4];
            positionData[72*i+3*j+1]=iter->y+y[j%4];
            positionData[72*i+3*j+2]=iter->z+int(j/4);
        }
        for(int j = 0;j < 8;j++)
        {
            positionData[72*i+3*j+24]=iter->x+int(j/4);
            positionData[72*i+3*j+25]=iter->y+x[j%4];
            positionData[72*i+3*j+26]=iter->z+y[j%4];
        }
        for(int j = 0;j < 8;j++)
        {
            positionData[72*i+3*j+48]=iter->x+y[j%4];
            positionData[72*i+3*j+49]=iter->y+int(j/4);
            positionData[72*i+3*j+50]=iter->z+x[j%4];
        }
        i++;
    }
    for(iter=point_on_surface_out.begin();iter!=point_on_surface_out.end();iter++)
    {
        for(int j = 0;j < 8;j++)
        {
            positionData[72*i+3*j+0]=iter->x+x[j%4];
            positionData[72*i+3*j+1]=iter->y+y[j%4];
            positionData[72*i+3*j+2]=iter->z+int(j/4);
        }
        for(int j = 0;j < 8;j++)
        {
            positionData[72*i+3*j+24]=iter->x+int(j/4);
            positionData[72*i+3*j+25]=iter->y+x[j%4];
            positionData[72*i+3*j+26]=iter->z+y[j%4];
        }
        for(int j = 0;j < 8;j++)
        {
            positionData[72*i+3*j+48]=iter->x+y[j%4];
            positionData[72*i+3*j+49]=iter->y+int(j/4);
            positionData[72*i+3*j+50]=iter->z+x[j%4];
        }
        i++;
    }
    for(iter=point_on_surface_in.begin();iter!=point_on_surface_in.end();iter++)
    {
        for(int j = 0;j < 8;j++)
        {
            positionData[72*i+3*j+0]=iter->x+x[j%4];
            positionData[72*i+3*j+1]=iter->y+y[j%4];
            positionData[72*i+3*j+2]=iter->z+int(j/4);
        }
        for(int j = 0;j < 8;j++)
        {
            positionData[72*i+3*j+24]=iter->x+int(j/4);
            positionData[72*i+3*j+25]=iter->y+x[j%4];
            positionData[72*i+3*j+26]=iter->z+y[j%4];
        }
        for(int j = 0;j < 8;j++)
        {
            positionData[72*i+3*j+48]=iter->x+y[j%4];
            positionData[72*i+3*j+49]=iter->y+int(j/4);
            positionData[72*i+3*j+50]=iter->z+x[j%4];
        }
        i++;
    }
}

void Octree::GetBarycentricData(float* barycentricData)
{
    float x[4] = {0.0,1.0,1.0,0.0};
    float y[4] = {0.0,0.0,1.0,1.0};
    for(int i=0;i<int(point_on_surface_on.size()+point_on_surface_out.size()+point_on_surface_in.size());i++)
    {
        for(int j = 0;j < 8;j++)
        {
            barycentricData[72*i+3*j]=x[j%4];
            barycentricData[72*i+3*j+1]=y[j%4];
            barycentricData[72*i+3*j+2]=1;
        }
        for(int j = 0;j < 8;j++)
        {
            barycentricData[72*i+3*j+24]=1;
            barycentricData[72*i+3*j+25]=x[j%4];
            barycentricData[72*i+3*j+26]=y[j%4];
        }
        for(int j = 0;j < 8;j++)
        {
            barycentricData[72*i+3*j+48]=y[j%4];
            barycentricData[72*i+3*j+49]=1;
            barycentricData[72*i+3*j+50]=x[j%4];
        }
    }
}
