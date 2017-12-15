#include "Patchmodel/Patchmodel.h"
#include "Octree/Voxelization.h"
#include <cmath>
#include "GLSL/textfile.h"
#define PI 3.1415926

using namespace std;
GLuint vShader,fShader;
int WinWidth;
int WinHeight;
static double oldmy=-1,oldmx=-1;
static int angle[4]={90,90,90,90};
static float heightz[4]={0.0f,0.0f,0.0f,0.0f};
static int depth = 7;
GLuint vboId;
GLuint vaoId;
GLuint programId;
char szTitle[8] = "3dprint";

float positionData[] = {  
        -1.0,0.0,0.0,
        1.0,0.0,0.0,
        0.0,1.0,0.0,
        0.0,-1.0,0.0};
//��ɫ����  

float colorData[] = {  
        0.0f, 0.0f, 0.0f,  
        0.0f, 0.0f, 0.0f,  
        0.0f, 0.0f, 0.0f,  
        0.0f, 0.0f, 0.0f };  

void initShader(const char *VShaderFile,const char *FShaderFile)  
{  
    //1���鿴GLSL��OpenGL�İ汾  
    const GLubyte *renderer = glGetString( GL_RENDERER );  
    const GLubyte *vendor = glGetString( GL_VENDOR );  
    const GLubyte *version = glGetString( GL_VERSION );  
    const GLubyte *glslVersion =   
        glGetString( GL_SHADING_LANGUAGE_VERSION );  
    GLint major, minor;  
    glGetIntegerv(GL_MAJOR_VERSION, &major);  
    glGetIntegerv(GL_MINOR_VERSION, &minor);  
    cout << "GL Vendor    :" << vendor << endl;  
    cout << "GL Renderer  : " << renderer << endl;  
    cout << "GL Version (string)  : " << version << endl;  
    cout << "GL Version (integer) : " << major << "." << minor << endl;  
    cout << "GLSL Version : " << glslVersion << endl;   
  
    //2��������ɫ��  
    //������ɫ�����󣺶�����ɫ��  
    vShader = glCreateShader(GL_VERTEX_SHADER);  
    //������  
    if (0 == vShader)  
    {  
        cerr << "ERROR : Create vertex shader failed" << endl;  
        exit(1);  
    }  
  
    //����ɫ��Դ�������ɫ�����������  
    const GLchar *vShaderCode = textFileRead(VShaderFile);  
    const GLchar *vCodeArray[1] = {vShaderCode};  
    glShaderSource(vShader,1,vCodeArray,NULL);  
  
    //������ɫ������  
    glCompileShader(vShader);  
  
  
    //�������Ƿ�ɹ�  
    GLint compileResult;  
    glGetShaderiv(vShader,GL_COMPILE_STATUS,&compileResult);  
    if (GL_FALSE == compileResult)  
    {  
        GLint logLen;  
        //�õ�������־����  
        glGetShaderiv(vShader,GL_INFO_LOG_LENGTH,&logLen);  
        if (logLen > 0)  
        {  
            char *log = (char *)malloc(logLen);  
            GLsizei written;  
            //�õ���־��Ϣ�����  
            glGetShaderInfoLog(vShader,logLen,&written,log);  
            cerr << "vertex shader compile log : " << endl;  
            cerr << log << endl;  
            free(log);//�ͷſռ�  
        }  
    }  
  
    //������ɫ������Ƭ����ɫ��  
    fShader = glCreateShader(GL_FRAGMENT_SHADER);  
    //������  
    if (0 == fShader)  
    {  
        cerr << "ERROR : Create fragment shader failed" << endl;  
        exit(1);  
    }  
  
    //����ɫ��Դ�������ɫ�����������  
    const GLchar *fShaderCode = textFileRead(FShaderFile);  
    const GLchar *fCodeArray[1] = {fShaderCode};  
    glShaderSource(fShader,1,fCodeArray,NULL);  
  
    //������ɫ������  
    glCompileShader(fShader);  
  
    //�������Ƿ�ɹ�  
    glGetShaderiv(fShader,GL_COMPILE_STATUS,&compileResult);  
    if (GL_FALSE == compileResult)  
    {  
        GLint logLen;  
        //�õ�������־����  
        glGetShaderiv(fShader,GL_INFO_LOG_LENGTH,&logLen);  
        if (logLen > 0)  
        {  
            char *log = (char *)malloc(logLen);  
            GLsizei written;  
            //�õ���־��Ϣ�����  
            glGetShaderInfoLog(fShader,logLen,&written,log);  
            cerr << "fragment shader compile log : " << endl;  
            cerr << log << endl;  
            free(log);//�ͷſռ�  
        }  
    }  
  
    //3��������ɫ������  
    //������ɫ������  
    GLuint programHandle = glCreateProgram();  
    if (!programHandle)  
    {  
        cerr << "ERROR : create program failed" << endl;  
        exit(1);  
    }  
    //����ɫ���������ӵ��������ĳ�����  
    glAttachShader(programHandle,vShader);  
    glAttachShader(programHandle,fShader); 

    glBindAttribLocation(programHandle, 0, "VertexPosition");
    glBindAttribLocation(programHandle, 1, "VertexColor");
 
    //����Щ�������ӳ�һ����ִ�г���  
    glLinkProgram(programHandle);  
    //��ѯ���ӵĽ��  
    GLint linkStatus;  
    glGetProgramiv(programHandle,GL_LINK_STATUS,&linkStatus);  
    if (GL_FALSE == linkStatus)  
    {  
        cerr << "ERROR : link shader program failed" << endl;  
        GLint logLen;  
        glGetProgramiv(programHandle,GL_INFO_LOG_LENGTH,  
            &logLen);  
        if (logLen > 0)  
        {  
            char *log = (char *)malloc(logLen);  
            GLsizei written;  
            glGetProgramInfoLog(programHandle,logLen,  
                &written,log);  
            cerr << "Program log : " << endl;  
            cerr << log << endl;  
        }  
    }  
    else//���ӳɹ�����OpenGL������ʹ����Ⱦ����  
    {  
        glUseProgram(programHandle);  
    }  
}  

void initVBO()  
{  
    // Create and populate the buffer objects  
    GLuint vboHandles[2];  
    glGenBuffers(2, vboHandles);  
    GLuint positionBufferHandle = vboHandles[0];  
    GLuint colorBufferHandle = vboHandles[1];  
  
    //��VBO�Թ�ʹ��  
    glBindBuffer(GL_ARRAY_BUFFER,positionBufferHandle);  
    //�������ݵ�VBO  
    glBufferData(GL_ARRAY_BUFFER,12 * sizeof(float),  
        positionData,GL_STATIC_DRAW);  
  
    //��VBO�Թ�ʹ��  
    glBindBuffer(GL_ARRAY_BUFFER,colorBufferHandle);  
    //�������ݵ�VBO  
    glBufferData(GL_ARRAY_BUFFER,12 * sizeof(float),  
        colorData,GL_STATIC_DRAW);  
  
    glGenVertexArrays(1,&vaoId);  
    glBindVertexArray(vaoId);  
  
    glEnableVertexAttribArray(0);//��������  
    glEnableVertexAttribArray(1);//������ɫ  
  
    //����glVertexAttribPointer֮ǰ��Ҫ���а󶨲���  
    glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);  
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );  
  
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);  
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );  
}  

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
    //ʹ��VAO��VBO����  
    glBindVertexArray(vaoId);  
    glDrawArrays(GL_LINES,0,4);  
    glBindVertexArray(0);  
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
    p->setperspective(sin(angle[0]*PI/180),cos(angle[0]*PI/180),heightz[0],0,0,0,0,0,1);
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
    tree->setperspective(sin(angle[1]*PI/180),cos(angle[1]*PI/180),heightz[1],0,0,0,0,0,1);
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
    if (state == GLFW_PRESS && x>0 && x<WinWidth/2 && y<WinHeight/2 && y>0)
    {
        angle[0]+=x-oldmx;
        heightz[0]+=0.03f*(y-oldmy);
        oldmx=x,oldmy=y;
    }
    else if (state == GLFW_PRESS && x>WinWidth/2 && x<WinWidth && y<WinHeight/2 && y>0)
    {
        angle[1]+=x-oldmx;
        heightz[1]+=0.03f*(y-oldmy);
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
        angle[0]+=x-oldmx;
        angle[1]+=x-oldmx;
        angle[2]+=x-oldmx;
        angle[3]+=x-oldmx;
        heightz[0]+=0.03f*(y-oldmy);
        heightz[1]+=0.03f*(y-oldmy);
        heightz[2]+=0.03f*(y-oldmy);
        heightz[3]+=0.03f*(y-oldmy);
        oldmx=x,oldmy=y;
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
        angle[0]=angle[1]=angle[2]=angle[3]=90;
        heightz[0]=heightz[1]=heightz[2]=heightz[3]=0.0f;
        glfwSetCursorPosCallback(window, cursor_right_pos_callback); 
    }
    else
        return;
}

void init()
{
    initShader("GLSL/basic.vert","GLSL/basic.frag");  
  
    initVBO();  
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
    p.ReadSTLFile(argv[1]);//��ȡ��ַ
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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2); // OpenGL���汾��
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0); // OpenGL���汾��
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
    glClearColor(1.0f,1.0f,1.0f,1.0f);

    glColor4f(1.0,1.0,0.0,0.0);

    init();
    while (!glfwWindowShouldClose(window))  
    {  
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //draw dividing line
        drawline();

        //first viewport
        //drawPatchmodel(&p);

        //second viewport
        //drawVoxelmodel(&tree);

        glfwSwapBuffers(window);  
        glfwPollEvents();  
    }  

    glfwDestroyWindow(window);  
    glfwTerminate();  
    return 0;
}

