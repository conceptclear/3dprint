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
	GLfloat light_ambient [] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat light_diffuse [] = { 0.5, 0.5, 0.5, 1.0 };
	
	glLightfv(GL_LIGHT0, GL_AMBIENT , light_ambient );
    glLightfv(GL_LIGHT0, GL_DIFFUSE , light_diffuse );

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

void drawSTL(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    p.setperspective(sin(angle*PI/180),cos(angle*PI/180),heightz,0,0,0,0,0,1);
    GLfloat light_position[] = { 0.0, 1.0, 0.0, 1.0 }; 
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glutSolidSphere(0.2,10,10);

    p.drawPatch();
    //    p.drawAABB();
    glTranslatef(p.xmin(),p.ymin(),p.zmin());
    glScalef((p.xmax()-p.xmin())/pow(2,depth-1),(p.ymax()-p.ymin())/pow(2,depth-1),(p.zmax()-p.zmin())/pow(2,depth-1));
    tree.Traverse();
    //	p.drawsliceequalllayers(30);
    //p.drawslicefacet();
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
    glClearColor(1.0f,1.0f,1.0f,1.0f);
    SetIllumination();
    glShadeModel(GL_FLAT);//设置颜色填充模式  
    glEnable(GL_COLOR_MATERIAL);//启用颜色追踪  
    glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);  
    //物体正面的材料环境颜色和散射颜色，追踪glColor所设置的颜色

    glEnable(GL_DEPTH_TEST);
    glutReshapeFunc(Reshape);
    glutDisplayFunc(drawSTL);
    glutIdleFunc(drawSTL);
    glutMouseFunc(LeftMousePick);
    glutMotionFunc(onMouseMove);
    glutMainLoop();
    return 0;
}

