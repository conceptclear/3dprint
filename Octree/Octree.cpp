#include "Octree.h"
using namespace std;

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

void Octree::PreOrder(OctreeNode *pRoot)
{
    if(pRoot!=NULL)
    {
        DisplayOctree(pRoot);
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
        DrawVoxel(pRoot->orderstrx,pRoot->orderstry,pRoot->orderstrz);
    }
}

void Octree::DrawVoxel(unsigned int x,unsigned int y,unsigned int z)
{
    glColor3f(1.0f,0.0f,0.0f);
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
    glColor3f(0.0f,1.0f,1.0f);
    glBegin(GL_POLYGON);
    glVertex3i(x,y,z);
    glVertex3i(x+1,y,z);
    glVertex3i(x+1,y+1,z);
    glVertex3i(x,y+1,z);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex3i(x,y,z+1);
    glVertex3i(x+1,y,z+1);
    glVertex3i(x+1,y+1,z+1);
    glVertex3i(x,y+1,z+1);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex3i(x,y,z);
    glVertex3i(x,y,z+1);
    glVertex3i(x+1,y,z+1);
    glVertex3i(x+1,y,z);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex3i(x+1,y+1,z);
    glVertex3i(x+1,y+1,z+1);
    glVertex3i(x,y+1,z+1);
    glVertex3i(x,y+1,z);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex3i(x+1,y+1,z);
    glVertex3i(x+1,y+1,z+1);
    glVertex3i(x+1,y,z+1);
    glVertex3i(x+1,y,z);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex3i(x,y+1,z);
    glVertex3i(x,y+1,z+1);
    glVertex3i(x,y,z+1);
    glVertex3i(x,y,z);
    glEnd();
    //    cout<<x<<":"<<y<<":"<<z<<endl;
}


