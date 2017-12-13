#include "Patchmodel/Patchmodel.h"
#include "Octree/Voxelization.h"
#include <cmath>
#define PI 3.1415926
int WinWidth;
int WinHeight;
static double oldmy=-1,oldmx=-1;
static int angle=90;
static float heightz=0.0f;
static int depth = 7;

char szTitle[8] = "3dprint";

static void error_callback(int error, const char* description)
{
    cout<<"error:"<<error<<"\n"<<"description:"<<description<<endl;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)  
{  
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)  
        glfwSetWindowShouldClose(window, GL_TRUE);  
}  

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

void drawline(void)
{
    glViewport(0,0,WinWidth,WinHeight);
    glColor3f(1.0, 0.0, 0.0); 
    glBegin(GL_LINES);  
    glVertex2f(0, WinHeight/2);  
    glVertex2f(WinWidth, WinHeight/2);  
    glVertex2f(WinWidth/2, 0);  
    glVertex2f(WinWidth/2, WinHeight);  
    glEnd();  
}

void drawPatchmodel(Patchmodel* p)
{
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
    p->setperspective(sin(angle*PI/180),cos(angle*PI/180),heightz,0,0,0,0,0,1);
    glPushMatrix();
    SetIllumination();
    glTranslatef(-(p->xmax()+p->xmin())/2,-(p->ymax()+p->ymin())/2,-(p->zmax()+p->zmin())/2);
    p->drawPatch();
//    p.drawAABB();
    //	p.drawsliceequalllayers(30);
    //p.drawslicefacet();
    glPopMatrix();
}

void drawVoxelmodel(Voxelization* tree)
{
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
    gluLookAt(10,10,10,0,0,0,0,0,1);
    glPushMatrix();
    SetIllumination();
    glTranslatef(-(tree->Xmax()+tree->Xmin())/2,-(tree->Ymax()+tree->Ymin())/2,-(tree->Zmax()+tree->Zmin())/2);
    glTranslatef(tree->Xmin(),tree->Ymin(),tree->Zmin());
    glScalef((tree->Xmax()-tree->Xmin())/pow(2,depth-1),(tree->Ymax()-tree->Ymin())/pow(2,depth-1),(tree->Zmax()-tree->Zmin())/pow(2,depth-1));
    tree->Traverse();
    glPopMatrix();
}

void framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
    WinWidth = w;
    WinHeight = h;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 1.0*WinWidth / WinHeight, 1, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void cursor_left_pos_callback(GLFWwindow* window, double x,double y)
{
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (state == GLFW_PRESS)
    {
        angle+=x-oldmx;
        heightz+=0.03f*(y-oldmy);
        oldmx=x,oldmy=y;
    }
    else
        return;
}

void cursor_right_pos_callback(GLFWwindow* window, double x,double y)
{
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    if (state == GLFW_PRESS)
    {
    }
    else
        return;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    glfwGetCursorPos(window, &oldmx, &oldmy);
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        glfwSetCursorPosCallback(window, cursor_left_pos_callback); 
    }
    else if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        glfwSetCursorPosCallback(window, cursor_right_pos_callback); 
    }
    else
        return;
}

int main(int argc, char *argv[])
{
    if(argc<3)
    {
        cout<<"Please Input the file address and the depth"<<endl;
        return -1;
    }
    Patchmodel p;
    Voxelization tree;
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

    WinWidth = 1000;
    WinHeight = 1000;
    GLFWwindow *window;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) return -1;  
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2); // OpenGL主版本号
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0); // OpenGL副版本号
    window = glfwCreateWindow(WinWidth,WinHeight,szTitle,NULL,NULL);  
    if (!window)  
    {  
        glfwTerminate();  
        exit(EXIT_FAILURE);  
    }  

    glfwMakeContextCurrent(window);  
  
    glfwSetKeyCallback(window, key_callback);  
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); 
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwGetFramebufferSize(window, &WinWidth, &WinHeight);  
    framebuffer_size_callback(window, WinWidth, WinHeight);  
  
    glewExperimental = GL_TRUE;  
    glewInit();  
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))  
    {  
        glClearColor(1.0f,1.0f,1.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //draw dividing line
        drawline();

        //first viewport
        drawPatchmodel(&p);

        //second viewport
        drawVoxelmodel(&tree);

        glfwSwapBuffers(window);  
        glfwPollEvents();  
    }  

    glfwDestroyWindow(window);  
    glfwTerminate();  
    return 0;
}

