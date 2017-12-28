#include "Patchmodel/Patchmodel.h"
#include "Octree/Voxelization.h"
#include <cmath>
#include "GLSL/textfile.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#define PI 3.1415926
#define BUFFER_OFFSET(offset)  ((GLvoid*)offset)

using namespace std;
GLuint vShader,fShader;
int WinWidth;
int WinHeight;
static double oldmy=-1,oldmx=-1;
static int angle[4]={90,90,90,90};
static float heightz[4]={0.0f,0.0f,0.0f,0.0f};
static int depth = 7;
static int layers = 50;
static float viewdistance[4]={0.0,0.0,0.0,0.0};
static float maxlength;
GLuint vaoId[5];
GLuint vboHandles[5][3];
GLuint positionBufferHandle[5];
GLuint barycentricBufferHandle[5];
GLuint programId;
GLuint projectionLocation;
GLuint cameraLocation;
GLuint modelLocation;
GLuint setcolor;
GLuint coutedge;
char szTitle[8] = "3dprint";

void initShader(const char *VShaderFile,const char *FShaderFile)
{
    //1、查看GLSL和OpenGL的版本
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

    //2、编译着色器
    //创建着色器对象：顶点着色器
    vShader = glCreateShader(GL_VERTEX_SHADER);
    //错误检测
    if (0 == vShader)
    {
        cerr << "ERROR : Create vertex shader failed" << endl;
        exit(1);
    }

    //把着色器源代码和着色器对象相关联
    const GLchar *vShaderCode = textFileRead(VShaderFile);
    const GLchar *vCodeArray[1] = {vShaderCode};
    glShaderSource(vShader,1,vCodeArray,NULL);

    //编译着色器对象
    glCompileShader(vShader);


    //检查编译是否成功
    GLint compileResult;
    glGetShaderiv(vShader,GL_COMPILE_STATUS,&compileResult);
    if (GL_FALSE == compileResult)
    {
        GLint logLen;
        //得到编译日志长度
        glGetShaderiv(vShader,GL_INFO_LOG_LENGTH,&logLen);
        if (logLen > 0)
        {
            char *log = (char *)malloc(logLen);
            GLsizei written;
            //得到日志信息并输出
            glGetShaderInfoLog(vShader,logLen,&written,log);
            cerr << "vertex shader compile log : " << endl;
            cerr << log << endl;
            free(log);//释放空间
        }
    }

    //创建着色器对象：片断着色器
    fShader = glCreateShader(GL_FRAGMENT_SHADER);
    //错误检测
    if (0 == fShader)
    {
        cerr << "ERROR : Create fragment shader failed" << endl;
        exit(1);
    }

    //把着色器源代码和着色器对象相关联
    const GLchar *fShaderCode = textFileRead(FShaderFile);
    const GLchar *fCodeArray[1] = {fShaderCode};
    glShaderSource(fShader,1,fCodeArray,NULL);

    //编译着色器对象
    glCompileShader(fShader);

    //检查编译是否成功
    glGetShaderiv(fShader,GL_COMPILE_STATUS,&compileResult);
    if (GL_FALSE == compileResult)
    {
        GLint logLen;
        //得到编译日志长度
        glGetShaderiv(fShader,GL_INFO_LOG_LENGTH,&logLen);
        if (logLen > 0)
        {
            char *log = (char *)malloc(logLen);
            GLsizei written;
            //得到日志信息并输出
            glGetShaderInfoLog(fShader,logLen,&written,log);
            cerr << "fragment shader compile log : " << endl;
            cerr << log << endl;
            free(log);//释放空间
        }
    }

    //3、链接着色器对象
    //创建着色器程序
    GLuint programHandle = glCreateProgram();
    if (!programHandle)
    {
        cerr << "ERROR : create program failed" << endl;
        exit(1);
    }
    //将着色器程序链接到所创建的程序中
    glAttachShader(programHandle,vShader);
    glAttachShader(programHandle,fShader);

    glBindAttribLocation(programHandle, 0, "VertexPosition");
    glBindAttribLocation(programHandle, 1, "VertexBarycentric");

    //将这些对象链接成一个可执行程序
    glLinkProgram(programHandle);
    //查询链接的结果
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
    else//链接成功，在OpenGL管线中使用渲染程序
    {
        glUseProgram(programHandle);
        projectionLocation = glGetUniformLocation(programHandle, "projection");
        cameraLocation = glGetUniformLocation(programHandle, "camera");
        modelLocation = glGetUniformLocation(programHandle, "model");
        setcolor = glGetUniformLocation(programHandle, "setcolor");
        coutedge = glGetUniformLocation(programHandle, "coutedge");
    }
}

template<typename T>
void initVBO(T* positionData, float* barycentricData, int viewport, int positionsize)
{
    if(viewport>=4||viewport<0)
        return;
    // Create and populate the buffer objects
    glGenBuffers(3, vboHandles[viewport]);
    positionBufferHandle[viewport] = vboHandles[viewport][0];
    barycentricBufferHandle[viewport] = vboHandles[viewport][1];

    //绑定VBO以供使用
    glBindBuffer(GL_ARRAY_BUFFER,positionBufferHandle[viewport]);
    //加载数据到VBO
    glBufferData(GL_ARRAY_BUFFER,positionsize * sizeof(T),
            positionData,GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    //绑定VBO以供使用
    glBindBuffer(GL_ARRAY_BUFFER,barycentricBufferHandle[viewport]);
    //加载数据到VBO
    glBufferData(GL_ARRAY_BUFFER,positionsize * sizeof(float),
            barycentricData,GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    //绑定VAO
    glGenVertexArrays(1,&vaoId[viewport]);
    glBindVertexArray(vaoId[viewport]);

    glEnableVertexAttribArray(0);//顶点坐标
    glEnableVertexAttribArray(1);//顶点颜色

    //调用glVertexAttribPointer之前需要进行绑定操作
    glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle[viewport]);
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

    glBindBuffer(GL_ARRAY_BUFFER, barycentricBufferHandle[viewport]);
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
    else if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        angle[0]=angle[1]=angle[2]=angle[3]=90;
        heightz[0]=heightz[1]=heightz[2]=heightz[3]=0.0f;
        viewdistance[0]=viewdistance[1]=viewdistance[2]=viewdistance[3]=maxlength;
    }
}

void drawline(void)
{
    glViewport(0,0,WinWidth,WinHeight);
    //使用VAO、VBO绘制
    glBindVertexArray(vaoId[0]);
    // 使用uniform变量传递MVP矩阵
    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 camera = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    glUniformMatrix4fv(
            projectionLocation, 1, GL_FALSE, &projection[0][0]); // 传递投影矩阵
    glUniformMatrix4fv(
            cameraLocation, 1, GL_FALSE, &camera[0][0]);// 传递视变换矩阵
    glUniformMatrix4fv(
            modelLocation, 1, GL_FALSE, &model[0][0]); // 传递模型变换矩阵
    glUniform3f(
            setcolor, 0.0f, 0.0f, 0.0f); //传递颜色矩阵
    glUniform1i(coutedge, 2);
    glDrawArrays(GL_LINES,0,4);
    glBindVertexArray(0);
}

void drawPatchmodel(Patchmodel* p)
{
    //first viewport
    glEnable(GL_SCISSOR_TEST);
    glScissor(0,WinHeight/2,WinWidth/2,WinHeight/2);

    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    glViewport(0,WinHeight/2,WinWidth/2,WinHeight/2);
    glPushMatrix();
    glBindVertexArray(vaoId[1]);
    // 投影矩阵
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
            (GLfloat)(WinWidth)/ WinHeight, 1.0f, 1000.0f);
    glm::vec3 eyePos(GLfloat(2*viewdistance[0]*sin(angle[0]*PI/180)),GLfloat(2*viewdistance[0]*cos(angle[0]*PI/180)),viewdistance[0]*heightz[0]);
    // 视变换矩阵
    glm::mat4 camera = glm::lookAt(eyePos,
            glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // 模型变换矩阵
    glm::mat4 model = glm::translate(glm::mat4(), glm::vec3(-(p->xmax()+p->xmin())/2,-(p->ymax()+p->ymin())/2,-(p->zmax()+p->zmin())/2));
    // 使用uniform变量传递MVP矩阵
    glUniformMatrix4fv(
            projectionLocation, 1, GL_FALSE, &projection[0][0]); // 传递投影矩阵
    glUniformMatrix4fv(
            cameraLocation, 1, GL_FALSE, &camera[0][0]);// 传递视变换矩阵
    glUniformMatrix4fv(
            modelLocation, 1, GL_FALSE, &model[0][0]); // 传递模型变换矩阵
    //使用VAO、VBO绘制
    glUniform3f(
            setcolor, 0.0f, 1.0f, 1.0f); //传递颜色矩阵
    glUniform1i(coutedge, 3);
    glDrawArrays(GL_TRIANGLES,0,p->m_VectorFacet.size()*9);
    glBindVertexArray(0);
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
    glPushMatrix();
    glBindVertexArray(vaoId[2]);
    // 投影矩阵
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
            (GLfloat)(WinWidth)/ WinHeight, 1.0f, 1000.0f);
    glm::vec3 eyePos(GLfloat(2*viewdistance[1]*sin(angle[1]*PI/180)),GLfloat(2*viewdistance[1]*cos(angle[1]*PI/180)),viewdistance[1]*heightz[1]);
    // 视变换矩阵
    glm::mat4 camera = glm::lookAt(eyePos,
            glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // 模型变换矩阵
    glm::mat4 model = glm::translate(glm::mat4(), glm::vec3(tree->Xmin(),tree->Ymin(),tree->Zmin()));
    model = glm::translate(model, glm::vec3(-(tree->Xmax()+tree->Xmin())/2,-(tree->Ymax()+tree->Ymin())/2,-(tree->Zmax()+tree->Zmin())/2));
    model = glm::scale(model, glm::vec3((tree->Xmax()-tree->Xmin())/pow(2,depth-1),(tree->Ymax()-tree->Ymin())/pow(2,depth-1),(tree->Zmax()-tree->Zmin())/pow(2,depth-1)));
    // 使用uniform变量传递MVP矩阵
    glUniformMatrix4fv(
            projectionLocation, 1, GL_FALSE, &projection[0][0]); // 传递投影矩阵
    glUniformMatrix4fv(
            cameraLocation, 1, GL_FALSE, &camera[0][0]);// 传递视变换矩阵
    glUniformMatrix4fv(
            modelLocation, 1, GL_FALSE, &model[0][0]); // 传递模型变换矩阵
    //使用VAO、VBO绘制
    glUniform3f(
            setcolor, 0.0f, 1.0f, 0.0f); //传递颜色矩阵
    glUniform1i(coutedge, 4);
    glDrawArrays(GL_QUADS,0,tree->point_on_surface.size()*72);
    glBindVertexArray(0);
    glPopMatrix();
}

void drawequalllayers(Patchmodel* p)
{
    //first viewport
    glEnable(GL_SCISSOR_TEST);
    glScissor(0,0,WinWidth/2,WinHeight/2);

    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    glViewport(0,0,WinWidth/2,WinHeight/2);
    glPushMatrix();
    glBindVertexArray(vaoId[3]);
    // 投影矩阵
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
            (GLfloat)(WinWidth)/ WinHeight, 1.0f, 1000.0f);
    glm::vec3 eyePos(GLfloat(2*viewdistance[2]*sin(angle[2]*PI/180)),GLfloat(2*viewdistance[2]*cos(angle[2]*PI/180)),viewdistance[2]*heightz[2]);
    // 视变换矩阵
    glm::mat4 camera = glm::lookAt(eyePos,
            glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // 模型变换矩阵
    glm::mat4 model = glm::translate(glm::mat4(), glm::vec3(-(p->xmax()+p->xmin())/2,-(p->ymax()+p->ymin())/2,-(p->zmax()+p->zmin())/2));
    // 使用uniform变量传递MVP矩阵
    glUniformMatrix4fv(
            projectionLocation, 1, GL_FALSE, &projection[0][0]); // 传递投影矩阵
    glUniformMatrix4fv(
            cameraLocation, 1, GL_FALSE, &camera[0][0]);// 传递视变换矩阵
    glUniformMatrix4fv(
            modelLocation, 1, GL_FALSE, &model[0][0]); // 传递模型变换矩阵
    //使用VAO、VBO绘制
    glUniform1i(coutedge, 1);
    int count = 0;
    for(unsigned long i=0;i<p->slicingdata.size();i++)
    {
        for(unsigned long j=0;j<p->slicingdata[i].size();j++)
        {
            glDrawArrays(GL_LINE_LOOP,count,p->slicingdata[i][j].size());
            count+=p->slicingdata[i][j].size();
        }
    }
    glBindVertexArray(0);
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
    else if (state == GLFW_PRESS && x>0 && x<WinWidth/2 && y<WinHeight && y>WinHeight/2)
    {
        angle[2]+=x-oldmx;
        heightz[2]+=0.03f*(y-oldmy);
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
        glfwSetCursorPosCallback(window, cursor_right_pos_callback);
    }
    else
        return;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    double xpos,ypos;
    glfwGetCursorPos(window,&xpos,&ypos);
    if(xpos>0 && xpos<WinWidth/2 && ypos>0 && ypos<WinHeight/2)
        viewdistance[0]*=(1-yoffset/10);
    else if(xpos>WinWidth/2 && xpos<WinWidth && ypos>0 && ypos<WinHeight/2)
        viewdistance[1]*=(1-yoffset/10);
    else if(xpos>0 && xpos<WinWidth/2 && ypos>WinHeight/2 && ypos<WinHeight)
        viewdistance[2]*=(1-yoffset/10);
    else if(xpos>WinWidth/2 && xpos<WinWidth && ypos>WinHeight/2 && ypos<WinHeight)
        viewdistance[3]*=(1-yoffset/10);
    else
        return;
}

void init(Patchmodel* p, Voxelization* tree)
{
    //dividing line
    float positionData0[12] = {
        -1.0,0.0,0.0,
        1.0,0.0,0.0,
        0.0,1.0,0.0,
        0.0,-1.0,0.0
    };

    float barycentricData0[12] = {
        0.0f, 0.0f, 0.0f, 
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
    };
    initVBO(positionData0,barycentricData0,0,12);

    //first viewport: patch model
    int positionsize1 = p->m_VectorFacet.size()*9;
    float *positionData1 = new float[positionsize1];
    p->GetPositionData(positionData1);
    float *barycentricData1 = new float[positionsize1];
    float x1[3]={1.0f,0.0f,0.0f};
    float y1[3]={0.0f,1.0f,0.0f};
    float z1[3]={0.0f,0.0f,1.0f};
    for(int i=0;i<positionsize1/3;i++)
    {
        barycentricData1[3*i+0]=x1[i%3];
        barycentricData1[3*i+1]=y1[i%3];
        barycentricData1[3*i+2]=z1[i%3];
    }
    initVBO(positionData1,barycentricData1,1,positionsize1);
    delete[] positionData1;
    delete[] barycentricData1;

    //second viewport: voxel model
    int positionsize2 = tree->point_on_surface.size()*72;
    float *positionData2 = new float[positionsize2];
    tree->GetPositionData(positionData2);
    float *barycentricData2 = new float[positionsize2];
    tree->GetBarycentricData(barycentricData2);
    initVBO(positionData2,barycentricData2,2,positionsize2);
    delete[] positionData2;
    delete[] barycentricData2;

    //third viewport: equallayers
    int positionsize3 = 0;
    for(unsigned long i=0;i<p->slicingdata.size();i++)
    {
        for(unsigned long j=0;j<p->slicingdata[i].size();j++)
        {
            positionsize3+=p->slicingdata[i][j].size();
        }
    }
    positionsize3=positionsize3*3;
    float *positionData3 = new float[positionsize3];
    p->GetLayerData(positionData3);
    float *barycentricData3 = new float[positionsize3];
    initVBO(positionData3,barycentricData3,3,positionsize3);
    delete[] positionData3;
    delete[] barycentricData3;
}

int main(int argc, char *argv[])
{
    if(argc<4)
    {
        cout<<"Please Input the file address ,the depth and the layers"<<endl;
        return -1;
    }

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
    glfwSetScrollCallback(window, scroll_callback);
    framebuffer_size_callback(window, WinWidth, WinHeight);

    glewExperimental = GL_TRUE;
    glewInit();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(1.0f,1.0f,1.0f,1.0f);
    initShader("GLSL/basic.vert","GLSL/basic.frag");

    Patchmodel p;
    Voxelization tree;
    p.ReadSTLFile(argv[1]);//读取地址
    depth = atoi(argv[2]);
    if(depth<=1)
    {
        cerr<<"error depth"<<endl;
        return -1;
    }
    layers = atoi(argv[3]);
    if(layers<=1)
    {
        cerr<<"error layers"<<endl;
        return -1;
    }
    cout<<"xmax:"<<p.xmax()<<"\n"<<"xmin:"<<p.xmin()<<"\n"<<"ymax:"<<p.ymax()<<"\n"<<"ymin:"<<p.ymin()<<"\n"<<"zmax:"<<p.zmax()<<"\n"<<"zmin:"<<p.zmin()<<endl;
    p.sliceequalllayers(layers);
    //	p.slicefacet(90);
    tree.GetExtremum(p.xmax(),p.xmin(),p.ymax(),p.ymin(),p.zmax(),p.zmin());
    tree.MakeOctree(depth);
    tree.PointToVoxel(p.m_VectorPoint);
    //    tree.EdgeToVoxel(p.m_VectorEdge,p.m_VectorPoint);
    tree.FacetToVoxel(p.m_VectorFacet, p.m_VectorPoint);
    //tree.GetSurfacePointNum();
    tree.Traverse();
    maxlength = max(max(p.xmax()-p.xmin(),p.ymax()-p.ymin()),p.zmax()-p.zmin());
    viewdistance[0]=viewdistance[1]=viewdistance[2]=viewdistance[3]=maxlength;

    init(&p,&tree);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //draw dividing line
        drawline();

        //first viewport
        drawPatchmodel(&p);

        //second viewport
        drawVoxelmodel(&tree);

        //third viewport
        drawequalllayers(&p);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    glUseProgram(0);
    return 0;
}

