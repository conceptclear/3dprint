#include "Octree.h"
using namespace std;

bool OctreePoint:: operator==(const OctreePoint &point)const
{
    return ((x==point.x)&&(y==point.y)&&(z==point.z));
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
        unsigned int rootx = 2*root->orderstrx;
        unsigned int rooty = 2*root->orderstry;
        unsigned int rootz = 2*root->orderstrz;
        Octree* back_left_bottom =new Octree(rootx,rooty,rootz,height-1);
        Octree* back_left_top =new Octree(rootx,rooty,rootz+1,height-1);
        Octree* back_right_bottom =new Octree(rootx,rooty+1,rootz,height-1);
        Octree* back_right_top =new Octree(rootx,rooty+1,rootz+1,height-1);
        Octree* front_left_bottom =new Octree(rootx+1,rooty,rootz,height-1);
        Octree* front_left_top =new Octree(rootx+1,rooty,rootz+1,height-1);
        Octree* front_right_bottom =new Octree(rootx+1,rooty+1,rootz,height-1);
        Octree* front_right_top =new Octree(rootx+1,rooty+1,rootz+1,height-1);
        root->Back_Left_Bottom = back_left_bottom->root;
        root->Back_Left_Top = back_left_top->root;
        root->Back_Right_Bottom = back_right_bottom->root;
        root->Back_Right_Top = back_right_top->root;
        root->Front_Left_Bottom = front_left_bottom->root;
        root->Front_Left_Top = front_left_top->root;
        root->Front_Right_Bottom = front_right_bottom->root;
        root->Front_Right_Top = front_right_top->root;
        back_left_bottom->MakeOctree(height-1);
        back_left_top->MakeOctree(height-1);
        back_right_bottom->MakeOctree(height-1);
        back_right_top->MakeOctree(height-1);
        front_left_bottom->MakeOctree(height-1);
        front_left_top->MakeOctree(height-1);
        front_right_bottom->MakeOctree(height-1);
        front_right_top->MakeOctree(height-1);
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

unsigned int Octree::TraverseZ(unsigned int z)
{
    unsigned int output;
    layer_impactfactor=0;
    output=PreOrderZ(root,z);
    return output;
}

void Octree::PreOrder(OctreeNode *pRoot)
{
    if(pRoot!=NULL)
    {
        DisplayOctree(pRoot);
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

unsigned int Octree::PreOrderZ(OctreeNode *pRoot, unsigned int z)
{
    int z_direction;
    unsigned int num;
    if(pRoot!=NULL)
    {
        num=GetContour(pRoot);
        SetImpactFactor(pRoot);
        if(pRoot->depth==1)
            return num;
        if(pRoot->depth==0)
            pRoot->depth=max_height;
        z_direction = z>>(pRoot->depth-2)&1;
        if(z_direction==0)
        {
            num+=PreOrderZ(pRoot->Back_Left_Bottom,z);
            num+=PreOrderZ(pRoot->Back_Right_Bottom,z);
            num+=PreOrderZ(pRoot->Front_Left_Bottom,z);
            num+=PreOrderZ(pRoot->Front_Right_Bottom,z);
        }
        else
        {
            num+=PreOrderZ(pRoot->Back_Left_Top,z);
            num+=PreOrderZ(pRoot->Back_Right_Top,z);
            num+=PreOrderZ(pRoot->Front_Left_Top,z);
            num+=PreOrderZ(pRoot->Front_Right_Top,z);
        }
        return num;
    }
    return 0;
}

void Octree::DisplayOctree(OctreeNode *pRoot)
{
    if(pRoot->depth!=1)
    {
        /*
        cout<<"x="<<pRoot->orderstrx<<endl;
        cout<<"y="<<pRoot->orderstry<<endl;
        cout<<"z="<<pRoot->orderstrz<<endl;
        cout<<"depth="<<pRoot->depth<<endl;
        */
        return;
    }
    if(pRoot->flag!=0)
    {
        /*
        cout<<"x="<<pRoot->orderstrx<<endl;
        cout<<"y="<<pRoot->orderstry<<endl;
        cout<<"z="<<pRoot->orderstrz<<endl;
        cout<<"depth="<<pRoot->depth<<endl;
        */
        return;
    }
    else
    {
        /*
        cout<<"x="<<pRoot->orderstrx<<endl;
        cout<<"y="<<pRoot->orderstry<<endl;
        cout<<"z="<<pRoot->orderstrz<<endl;
        */
        OctreePoint opoint(pRoot->orderstrx,pRoot->orderstry,pRoot->orderstrz);
        point_on_surface.push_back(opoint);
        //DrawVoxel(pRoot->orderstrx,pRoot->orderstry,pRoot->orderstrz);
    }
}

unsigned int Octree::GetContour(OctreeNode *pRoot)
{
    if(pRoot->depth!=1)
        return 0;
    else if(pRoot->flag==0)
        return 1;
    else
        return 0;
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
    int flagup,flagdown;
    flagup=GetUp(pRoot);
    flagdown=GetDown(pRoot);
    pRoot->impactfactor=flagup+flagdown;
    layer_impactfactor+=pRoot->impactfactor;
}

void Octree::DrawVoxel(unsigned int x,unsigned int y,unsigned int z)
{
    glColor3f(0.0f,0.0f,0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex3i(x,y,z);
    glVertex3i(x+1,y,z);
    glVertex3i(x+1,y+1,z);
    glVertex3i(x,y+1,z);
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex3i(x,y,z+1);
    glVertex3i(x+1,y,z+1);
    glVertex3i(x+1,y+1,z+1);
    glVertex3i(x,y+1,z+1);
    glEnd();
    glBegin(GL_LINES);
    glVertex3i(x,y,z);
    glVertex3i(x,y,z+1);
    glEnd();
    glBegin(GL_LINES);
    glVertex3i(x+1,y,z);
    glVertex3i(x+1,y,z+1);
    glEnd();
    glBegin(GL_LINES);
    glVertex3i(x+1,y+1,z);
    glVertex3i(x+1,y+1,z+1);
    glEnd();
    glBegin(GL_LINES);
    glVertex3i(x,y+1,z);
    glVertex3i(x,y+1,z+1);
    glEnd();
    glColor3f(1.0f,1.0f,0.0f);
    glBegin(GL_POLYGON);
    glNormal3f(0,0,1);
    glVertex3i(x,y,z);
    glVertex3i(x+1,y,z);
    glVertex3i(x+1,y+1,z);
    glVertex3i(x,y+1,z);
    glEnd();
    glBegin(GL_POLYGON);
    glNormal3f(0,0,1);
    glVertex3i(x,y,z+1);
    glVertex3i(x+1,y,z+1);
    glVertex3i(x+1,y+1,z+1);
    glVertex3i(x,y+1,z+1);
    glEnd();
    glBegin(GL_POLYGON);
    glNormal3f(0,1,0);
    glVertex3i(x,y,z);
    glVertex3i(x,y,z+1);
    glVertex3i(x+1,y,z+1);
    glVertex3i(x+1,y,z);
    glEnd();
    glBegin(GL_POLYGON);
    glNormal3f(0,1,0);
    glVertex3i(x+1,y+1,z);
    glVertex3i(x+1,y+1,z+1);
    glVertex3i(x,y+1,z+1);
    glVertex3i(x,y+1,z);
    glEnd();
    glBegin(GL_POLYGON);
    glNormal3f(1,0,0);
    glVertex3i(x+1,y+1,z);
    glVertex3i(x+1,y+1,z+1);
    glVertex3i(x+1,y,z+1);
    glVertex3i(x+1,y,z);
    glEnd();
    glBegin(GL_POLYGON);
    glNormal3f(1,0,0);
    glVertex3i(x,y+1,z);
    glVertex3i(x,y+1,z+1);
    glVertex3i(x,y,z+1);
    glVertex3i(x,y,z);
    glEnd();
    //    cout<<x<<":"<<y<<":"<<z<<endl;
}

void Octree::GetPositionData(float* positionData)
{
    int x[4] = {0,1,1,0};
    int y[4] = {0,0,1,1};
    for(int i=0;i<int(point_on_surface.size());i++)
    {
        for(int j = 0;j < 8;j++)
        {
            positionData[72*i+3*j]=point_on_surface[i].x+x[j%4];
            positionData[72*i+3*j+1]=point_on_surface[i].y+y[j%4];
            positionData[72*i+3*j+2]=point_on_surface[i].z+int(j/4);
        }
        for(int j = 0;j < 8;j++)
        {
            positionData[72*i+3*j+24]=point_on_surface[i].x+int(j/4);
            positionData[72*i+3*j+25]=point_on_surface[i].y+x[j%4];
            positionData[72*i+3*j+26]=point_on_surface[i].z+y[j%4];
        }
        for(int j = 0;j < 8;j++)
        {
            positionData[72*i+3*j+48]=point_on_surface[i].x+y[j%4];
            positionData[72*i+3*j+49]=point_on_surface[i].y+int(j/4);
            positionData[72*i+3*j+50]=point_on_surface[i].z+x[j%4];
        }
    }
}

void Octree::GetBarycentricData(float* barycentricData)
{
    float x[4] = {0.0,1.0,1.0,0.0};
    float y[4] = {0.0,0.0,1.0,1.0};
    for(int i=0;i<int(point_on_surface.size());i++)
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
