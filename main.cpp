#include "Patchmodel/Patchmodel.h"
#include "Octree/Voxelization.h"
#include <cmath>
#define PI 3.1415926
int WinWidth;
int WinHeight;
Patchmodel p;
Voxelization tree;
static int oldmy=-1,oldmx=-1;
static int angle=90;
static float heightz=0.0f;
static int depth = 7;
void SetIllumination(void)
{
	GLfloat light_ambient [] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat light_diffuse [] = { 0.6, 0.6, 0.6, 1.0 };
    GLfloat light_position[] = { 0.0f,1.0f,1.0f,0.0f};
	
	glLightfv(GL_LIGHT0, GL_AMBIENT , light_ambient );
    glLightfv(GL_LIGHT0, GL_DIFFUSE , light_diffuse );
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

void drawSTL(void)
{
    glClearColor(1.0f,1.0f,1.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1.0,0.0,0.0);

    //first viewport
    glEnable(GL_SCISSOR_TEST);  
    glScissor(0,WinHeight/2,WinWidth/2,WinHeight/2);  

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
    glDisable(GL_SCISSOR_TEST);  

    glViewport(0,WinHeight/2,WinWidth/2,WinHeight/2);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 1.0*WinWidth / WinHeight, 1, 1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    p.setperspective(sin(angle*PI/180),cos(angle*PI/180),heightz,0,0,0,0,0,1);
    glPushMatrix();
    SetIllumination();
    glTranslatef(-(p.xmax()+p.xmin())/2,-(p.ymax()+p.ymin())/2,-(p.zmax()+p.zmin())/2);
    p.drawPatch();
//    p.drawAABB();
    //	p.drawsliceequalllayers(30);
    //p.drawslicefacet();
    glPopMatrix();
    
    //second viewport
    glEnable(GL_SCISSOR_TEST);  
    glScissor(WinWidth/2,WinHeight/2,WinWidth/2,WinHeight/2);  

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
    glDisable(GL_SCISSOR_TEST);  

    glViewport(WinWidth/2,WinHeight/2,WinWidth/2,WinHeight/2);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 1.0*WinWidth / WinHeight, 1, 1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    p.setperspective(sin(angle*PI/180),cos(angle*PI/180),heightz,0,0,0,0,0,1);
    glPushMatrix();
    SetIllumination();
    glTranslatef(-(p.xmax()+p.xmin())/2,-(p.ymax()+p.ymin())/2,-(p.zmax()+p.zmin())/2);
    glTranslatef(p.xmin(),p.ymin(),p.zmin());
    glScalef((p.xmax()-p.xmin())/pow(2,depth-1),(p.ymax()-p.ymin())/pow(2,depth-1),(p.zmax()-p.zmin())/pow(2,depth-1));
    tree.Traverse();
    glPopMatrix();
    glutSwapBuffers();
}

void Reshape(int w, int h)
{
    WinWidth = w;
    WinHeight = h;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 1.0*WinWidth / WinHeight, 1, 1000);
    glMatrixMode(GL_MODELVIEW);
}

void LeftMousePick( int button, int state, int x, int y)
{
    if(button == GLUT_LEFT_BUTTON)
    {
        if(state == GLUT_DOWN)
            oldmx=x,oldmy=y;
    }
}

void onMouseMove( int x,int y)
{
    angle+=x-oldmx;
    heightz+=0.03f*(y-oldmy);
    oldmx=x,oldmy=y;
}

int main(int argc, char *argv[])
{
    if(argc<3)
    {
        cout<<"Please Input the file address and the depth"<<endl;
        return -1;
    }
    p.ReadSTLFile(argv[1]);//¶ÁÈ¡µØÖ·
    depth = atoi(argv[2]);
    cout<<"xmax:"<<p.xmax()<<"\n"<<"xmin:"<<p.xmin()<<"\n"<<"ymax:"<<p.ymax()<<"\n"<<"ymin:"<<p.ymin()<<"\n"<<"zmax:"<<p.zmax()<<"\n"<<"zmin:"<<p.zmin()<<endl;
    //	p.sliceequalllayers(30);
    //	p.slicefacet(90);
    tree.GetExtremum(p.xmax(),p.xmin(),p.ymax(),p.ymin(),p.zmax(),p.zmin());
    tree.MakeOctree(depth);
    tree.PointToVoxel(p.m_VectorPoint);
//    tree.EdgeToVoxel(p.m_VectorEdge,p.m_VectorPoint);
    tree.FacetToVoxel(p.m_VectorFacet, p.m_VectorPoint);
    tree.GetSurfacePointNum();

    WinWidth = 1000;
    WinHeight = 1000;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WinWidth, WinHeight);
    glutCreateWindow("Draw");

    glEnable(GL_DEPTH_TEST);
    glutReshapeFunc(Reshape);
    glutDisplayFunc(drawSTL);
    glutIdleFunc(drawSTL);
    glutMouseFunc(LeftMousePick);
    glutMotionFunc(onMouseMove);
    glutMainLoop();
    return 0;
}

