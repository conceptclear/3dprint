#include "Patchmodel/Patchmodel.h"
#include "Octree/Voxelization.h"
#define PI 3.1415926
#include <cmath>
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
    GLfloat light_position[] = { 1.0f,1.0f,1.0f,0.0f};
	
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

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    p.setperspective(sin(angle*PI/180),cos(angle*PI/180),heightz,0,0,0,0,0,1);
    glPushMatrix();
    glTranslatef(-(p.xmax()+p.xmin())/2,-(p.ymax()+p.ymin())/2,-(p.zmax()+p.zmin())/2);
//    p.drawPatch();
//    p.drawAABB();
    glPushMatrix();
    glTranslatef(p.xmin(),p.ymin(),p.zmin());
    glScalef((p.xmax()-p.xmin())/pow(2,depth-1),(p.ymax()-p.ymin())/pow(2,depth-1),(p.zmax()-p.zmin())/pow(2,depth-1));
    tree.Traverse();
    glPopMatrix();
    //	p.drawsliceequalllayers(30);
    //p.drawslicefacet();
    glPopMatrix();
    glutSwapBuffers();
}

void Reshape(int w, int h)
{
    WinWidth = w;
    WinHeight = h;
    // //改变显示区域，起始位置为客户端窗口左下角（非坐标原点）
    glViewport(0, 0, w, h);

    // 开启更新深度缓冲区的功能
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //宽高比改为当前值，视线区域与屏幕大小一致；
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
    p.ReadSTLFile(argv[1]);//读取地址
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

    WinWidth = 800;
    WinHeight = 800;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WinWidth, WinHeight);
    glutCreateWindow("Draw");
    SetIllumination();

    glEnable(GL_DEPTH_TEST);
    glutReshapeFunc(Reshape);
    glutDisplayFunc(drawSTL);
    glutIdleFunc(drawSTL);
    glutMouseFunc(LeftMousePick);
    glutMotionFunc(onMouseMove);
    glutMainLoop();
    return 0;
}

