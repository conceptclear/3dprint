#include "Patchmodel.h"
void Patchmodel::drawPatch(void)
{
    for (unsigned long i = 0; i < m_VectorFacet.size(); i++)
    {
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINE_LOOP);
        glNormal3f(m_VectorFacet[i].m_Normal[0],m_VectorFacet[i].m_Normal[1],m_VectorFacet[i].m_Normal[2]);
        glVertex3f(m_VectorPoint[m_VectorFacet[i].PointIndex[0]].m_Point[0], m_VectorPoint[m_VectorFacet[i].PointIndex[0]].m_Point[1], m_VectorPoint[m_VectorFacet[i].PointIndex[0]].m_Point[2]);
        glVertex3f(m_VectorPoint[m_VectorFacet[i].PointIndex[1]].m_Point[0], m_VectorPoint[m_VectorFacet[i].PointIndex[1]].m_Point[1], m_VectorPoint[m_VectorFacet[i].PointIndex[1]].m_Point[2]);
        glVertex3f(m_VectorPoint[m_VectorFacet[i].PointIndex[2]].m_Point[0], m_VectorPoint[m_VectorFacet[i].PointIndex[2]].m_Point[1], m_VectorPoint[m_VectorFacet[i].PointIndex[2]].m_Point[2]);
        glEnd();
        glColor3f(0.0, 1.0, 1.0);
        glBegin(GL_TRIANGLES);
        glNormal3f(m_VectorFacet[i].m_Normal[0],m_VectorFacet[i].m_Normal[1],m_VectorFacet[i].m_Normal[2]);
        glVertex3f(m_VectorPoint[m_VectorFacet[i].PointIndex[0]].m_Point[0], m_VectorPoint[m_VectorFacet[i].PointIndex[0]].m_Point[1], m_VectorPoint[m_VectorFacet[i].PointIndex[0]].m_Point[2]);
        glVertex3f(m_VectorPoint[m_VectorFacet[i].PointIndex[1]].m_Point[0], m_VectorPoint[m_VectorFacet[i].PointIndex[1]].m_Point[1], m_VectorPoint[m_VectorFacet[i].PointIndex[1]].m_Point[2]);
        glVertex3f(m_VectorPoint[m_VectorFacet[i].PointIndex[2]].m_Point[0], m_VectorPoint[m_VectorFacet[i].PointIndex[2]].m_Point[1], m_VectorPoint[m_VectorFacet[i].PointIndex[2]].m_Point[2]);
        glEnd();
    }
}

bool Patchmodel::slice(float z)
{
    if (z > z_max || z < z_min)
        return false;
    int edgesize = m_VectorEdge.size();
    vector<bool> bSlice(edgesize, false); //定义和边数量长度一致的数组，并全部赋值为false
    for (int edgenumber = 0; edgenumber != edgesize - 1; edgenumber++)
    {
        if (bSlice[edgenumber] == false)
        {
            //寻找其中为false的边（未切）
            vector<point> outline; //定义轮廓链
            if ((m_VectorPoint[m_VectorEdge[edgenumber].PointIndex[0]].m_Point[2] == m_VectorPoint[m_VectorEdge[edgenumber].PointIndex[1]].m_Point[2]) &&(m_VectorPoint[m_VectorEdge[edgenumber].PointIndex[1]].m_Point[2]==z))
                continue;//如果这个边在切平面上，按照平移算法这条边是不能算作true的，这样会形成死循环
            if ((m_VectorPoint[m_VectorEdge[edgenumber].PointIndex[0]].m_Point[2] - z)*(m_VectorPoint[m_VectorEdge[edgenumber].PointIndex[1]].m_Point[2] - z) <= 0)
            {
                float zadd = z;//定义移动后的z平面
                int iNextedge;
                float x, y;
                int lastfacet;
                int nextfacet;
                if ((m_VectorPoint[m_VectorEdge[edgenumber].PointIndex[0]].m_Point[2] - z)*(m_VectorPoint[m_VectorEdge[edgenumber].PointIndex[1]].m_Point[2] - z) == 0)
                {
                    /*
                       可以考虑将z向上移动一个微小距离，算出切边，再通过原z值计算交点
                       */
                    if (m_VectorPoint[m_VectorEdge[edgenumber].PointIndex[0]].m_Point[2] == z)
                    {
                        x = m_VectorPoint[m_VectorEdge[edgenumber].PointIndex[0]].m_Point[0];
                        y = m_VectorPoint[m_VectorEdge[edgenumber].PointIndex[0]].m_Point[1];
                    }
                    else
                    {
                        x = m_VectorPoint[m_VectorEdge[edgenumber].PointIndex[1]].m_Point[0];
                        y = m_VectorPoint[m_VectorEdge[edgenumber].PointIndex[1]].m_Point[1];
                    }
                    bSlice[edgenumber] = true;
                    point pt(x, y, edgenumber);
                    outline.push_back(pt);
                    if ((m_VectorPoint[m_VectorEdge[edgenumber].PointIndex[0]].m_Point[2] - z - 0.000001)*(m_VectorPoint[m_VectorEdge[edgenumber].PointIndex[1]].m_Point[2] - z - 0.000001) < 0)
                        zadd = z + 0.000001;
                    else
                        zadd = z - 0.000001;

                    if ((m_VectorFacet[m_VectorEdge[edgenumber].FacetIndex[0]].EdgeIndex[0] != edgenumber) && ((m_VectorPoint[m_VectorEdge[m_VectorFacet[m_VectorEdge[edgenumber].FacetIndex[0]].EdgeIndex[0]].PointIndex[0]].m_Point[2] - zadd)*(m_VectorPoint[m_VectorEdge[m_VectorFacet[m_VectorEdge[edgenumber].FacetIndex[0]].EdgeIndex[0]].PointIndex[1]].m_Point[2] - zadd) <= 0))
                    {
                        iNextedge = m_VectorFacet[m_VectorEdge[edgenumber].FacetIndex[0]].EdgeIndex[0];
                    }
                    else if ((m_VectorFacet[m_VectorEdge[edgenumber].FacetIndex[0]].EdgeIndex[1] != edgenumber) && ((m_VectorPoint[m_VectorEdge[m_VectorFacet[m_VectorEdge[edgenumber].FacetIndex[0]].EdgeIndex[1]].PointIndex[0]].m_Point[2] - zadd)*(m_VectorPoint[m_VectorEdge[m_VectorFacet[m_VectorEdge[edgenumber].FacetIndex[0]].EdgeIndex[1]].PointIndex[1]].m_Point[2] - zadd) <= 0))
                    {
                        iNextedge = m_VectorFacet[m_VectorEdge[edgenumber].FacetIndex[0]].EdgeIndex[1];
                    }
                    else
                        iNextedge = m_VectorFacet[m_VectorEdge[edgenumber].FacetIndex[0]].EdgeIndex[2];
                    lastfacet = m_VectorEdge[edgenumber].FacetIndex[0];
                    if (m_VectorEdge[edgenumber].FacetIndex[0] == m_VectorEdge[iNextedge].FacetIndex[0])
                        nextfacet = m_VectorEdge[iNextedge].FacetIndex[1];
                    else
                        nextfacet = m_VectorEdge[iNextedge].FacetIndex[0];
                }
                else
                {
                    /*
                       对于两个点(x1,y1,z1)和(x2,y2,z2)中间与z平面的交点(x,y,z)
                       (x,y,z)=(x1-x2,y1-y2,z1-z2)*(z-z2)/(z1-z2)+(x2,y2,z2)
                       */
                    x = m_VectorPoint[m_VectorEdge[edgenumber].PointIndex[1]].m_Point[0] + (z - m_VectorPoint[m_VectorEdge[edgenumber].PointIndex[1]].m_Point[2]) / (m_VectorPoint[m_VectorEdge[edgenumber].PointIndex[0]].m_Point[2] - m_VectorPoint[m_VectorEdge[edgenumber].PointIndex[1]].m_Point[2])*(m_VectorPoint[m_VectorEdge[edgenumber].PointIndex[0]].m_Point[0] - m_VectorPoint[m_VectorEdge[edgenumber].PointIndex[1]].m_Point[0]);
                    y = m_VectorPoint[m_VectorEdge[edgenumber].PointIndex[1]].m_Point[1] + (z - m_VectorPoint[m_VectorEdge[edgenumber].PointIndex[1]].m_Point[2]) / (m_VectorPoint[m_VectorEdge[edgenumber].PointIndex[0]].m_Point[2] - m_VectorPoint[m_VectorEdge[edgenumber].PointIndex[1]].m_Point[2])*(m_VectorPoint[m_VectorEdge[edgenumber].PointIndex[0]].m_Point[1] - m_VectorPoint[m_VectorEdge[edgenumber].PointIndex[1]].m_Point[1]);
                    point pt(x, y, edgenumber);
                    bSlice[edgenumber] = true;
                    outline.push_back(pt);
                    if ((m_VectorFacet[m_VectorEdge[edgenumber].FacetIndex[0]].EdgeIndex[0] != edgenumber) && ((m_VectorPoint[m_VectorEdge[m_VectorFacet[m_VectorEdge[edgenumber].FacetIndex[0]].EdgeIndex[0]].PointIndex[0]].m_Point[2] - z)*(m_VectorPoint[m_VectorEdge[m_VectorFacet[m_VectorEdge[edgenumber].FacetIndex[0]].EdgeIndex[0]].PointIndex[1]].m_Point[2] - z) <= 0))
                    {
                        iNextedge = m_VectorFacet[m_VectorEdge[edgenumber].FacetIndex[0]].EdgeIndex[0];
                    }
                    else if ((m_VectorFacet[m_VectorEdge[edgenumber].FacetIndex[0]].EdgeIndex[1] != edgenumber) && ((m_VectorPoint[m_VectorEdge[m_VectorFacet[m_VectorEdge[edgenumber].FacetIndex[0]].EdgeIndex[1]].PointIndex[0]].m_Point[2] - z)*(m_VectorPoint[m_VectorEdge[m_VectorFacet[m_VectorEdge[edgenumber].FacetIndex[0]].EdgeIndex[1]].PointIndex[1]].m_Point[2] - z) <= 0))
                    {
                        iNextedge = m_VectorFacet[m_VectorEdge[edgenumber].FacetIndex[0]].EdgeIndex[1];
                    }
                    else
                        iNextedge = m_VectorFacet[m_VectorEdge[edgenumber].FacetIndex[0]].EdgeIndex[2];
                    lastfacet = m_VectorEdge[edgenumber].FacetIndex[0];
                    if (m_VectorEdge[edgenumber].FacetIndex[0] == m_VectorEdge[iNextedge].FacetIndex[0])
                        nextfacet = m_VectorEdge[iNextedge].FacetIndex[1];
                    else
                        nextfacet = m_VectorEdge[iNextedge].FacetIndex[0];

                }
                while (iNextedge != edgenumber)
                {
                    float zadd1 = z;
                    if ((m_VectorPoint[m_VectorEdge[iNextedge].PointIndex[0]].m_Point[2] - z)*(m_VectorPoint[m_VectorEdge[iNextedge].PointIndex[1]].m_Point[2] - z) == 0)
                    {
                        /*
                           可以考虑将z向上移动一个微小距离，算出切边，再通过原z值计算交点
                           */
                        if (m_VectorPoint[m_VectorEdge[iNextedge].PointIndex[0]].m_Point[2] == z)
                        {
                            x = m_VectorPoint[m_VectorEdge[iNextedge].PointIndex[0]].m_Point[0];
                            y = m_VectorPoint[m_VectorEdge[iNextedge].PointIndex[0]].m_Point[1];
                        }
                        else
                        {
                            x = m_VectorPoint[m_VectorEdge[iNextedge].PointIndex[1]].m_Point[0];
                            y = m_VectorPoint[m_VectorEdge[iNextedge].PointIndex[1]].m_Point[1];
                        }
                        if (zadd == z)
                        {
                            if ((m_VectorPoint[m_VectorEdge[iNextedge].PointIndex[0]].m_Point[2] - z - 0.000001)*(m_VectorPoint[m_VectorEdge[iNextedge].PointIndex[1]].m_Point[2] - z - 0.000001) < 0)
                                zadd = z + 0.000001;
                            else
                                zadd = z - 0.000001;
                        }
                        zadd1 = zadd;
                    }
                    else
                    {
                        x = m_VectorPoint[m_VectorEdge[iNextedge].PointIndex[1]].m_Point[0] + (z - m_VectorPoint[m_VectorEdge[iNextedge].PointIndex[1]].m_Point[2]) / (m_VectorPoint[m_VectorEdge[iNextedge].PointIndex[0]].m_Point[2] - m_VectorPoint[m_VectorEdge[iNextedge].PointIndex[1]].m_Point[2])*(m_VectorPoint[m_VectorEdge[iNextedge].PointIndex[0]].m_Point[0] - m_VectorPoint[m_VectorEdge[iNextedge].PointIndex[1]].m_Point[0]);
                        y = m_VectorPoint[m_VectorEdge[iNextedge].PointIndex[1]].m_Point[1] + (z - m_VectorPoint[m_VectorEdge[iNextedge].PointIndex[1]].m_Point[2]) / (m_VectorPoint[m_VectorEdge[iNextedge].PointIndex[0]].m_Point[2] - m_VectorPoint[m_VectorEdge[iNextedge].PointIndex[1]].m_Point[2])*(m_VectorPoint[m_VectorEdge[iNextedge].PointIndex[0]].m_Point[1] - m_VectorPoint[m_VectorEdge[iNextedge].PointIndex[1]].m_Point[1]);
                    }
                    point pt1(x, y, iNextedge);
                    bSlice[iNextedge] = true;
                    outline.push_back(pt1);
                    int iNextedge1;
                    if ((m_VectorFacet[nextfacet].EdgeIndex[0] != iNextedge) && ((m_VectorPoint[m_VectorEdge[m_VectorFacet[nextfacet].EdgeIndex[0]].PointIndex[0]].m_Point[2] - zadd1)*(m_VectorPoint[m_VectorEdge[m_VectorFacet[nextfacet].EdgeIndex[0]].PointIndex[1]].m_Point[2] - zadd1) <= 0))
                    {
                        iNextedge1 = m_VectorFacet[nextfacet].EdgeIndex[0];
                    }
                    else if ((m_VectorFacet[nextfacet].EdgeIndex[1] != iNextedge) && ((m_VectorPoint[m_VectorEdge[m_VectorFacet[nextfacet].EdgeIndex[1]].PointIndex[0]].m_Point[2] - zadd1)*(m_VectorPoint[m_VectorEdge[m_VectorFacet[nextfacet].EdgeIndex[1]].PointIndex[1]].m_Point[2] - zadd1) <= 0))
                    {
                        iNextedge1 = m_VectorFacet[nextfacet].EdgeIndex[1];
                    }
                    else
                        iNextedge1 = m_VectorFacet[nextfacet].EdgeIndex[2];
                    lastfacet = nextfacet;
                    if (lastfacet == m_VectorEdge[iNextedge1].FacetIndex[0])
                        nextfacet = m_VectorEdge[iNextedge1].FacetIndex[1];
                    else
                        nextfacet = m_VectorEdge[iNextedge1].FacetIndex[0];
                    iNextedge = iNextedge1;
                }
                z_point.push_back(outline);
            }
        }
    }
    return true;
}

void Patchmodel::drawslice(float z)
{
    glColor3f(0.0, 1.0, 0.2);
    for (unsigned long i = 0; i < z_point.size(); i++)
    {
        glBegin(GL_LINE_LOOP);
        for (unsigned long j = 0; j < z_point[i].size(); j++)
        {
            glVertex3f(z_point[i][j].x, z_point[i][j].y, z);
        }
        glEnd();
    }
}

void Patchmodel::setperspective(double eyex,double eyey,double eyez,double centrex,double centrey,double centrez,double upx,double upy,double upz)
{
    float maxlength = max(max(x_max-x_min,y_max-y_min),z_max-z_min);
    gluLookAt( 2 * eyex * maxlength, 2 * eyey * maxlength, eyez * maxlength, centrex, centrey, centrez, upx, upy, upz);
}

bool Patchmodel::sliceequalllayers(int layernumber)
{
    if (layernumber <= 1)
    {
        cout << "层数应该大于1" << endl;
        return false;
    }
    else
    {
        float layerthickness = (z_max - z_min) / (layernumber - 1);
        for (float i = z_min; i <= z_max; i = i + layerthickness)
        {
            slice(i);
            slicingdata.push_back(z_point);
            z_point.clear();
        }
        return true;
    }
}

void Patchmodel::drawsliceequalllayers(int layernumber)
{
    glColor3f(0.0, 1.0, 0.2);
    if (layernumber <= 1)
    {
        cout << "层数应该大于1" << endl;
    }
    else
    {
        float layerthickness = (z_max - z_min) / (layernumber - 1);
        for (unsigned long i = 0; i < slicingdata.size(); i++)
        {
            for (unsigned long j = 0; j < slicingdata[i].size(); j++)
            {
                glBegin(GL_LINE_LOOP);
                for (unsigned long k = 0; k < slicingdata[i][j].size(); k++)
                {
                    glVertex3f(slicingdata[i][j][k].x, slicingdata[i][j][k].y, z_min + i*layerthickness);
                }
                glEnd();
            }
        }
    }
}

void Patchmodel::GetLayerData(float* positionData)
{
    float layerthickness = (z_max - z_min) / slicingdata.size();
    int count=0;
    for (unsigned long i = 0; i < slicingdata.size(); i++)
    {
        for (unsigned long j = 0; j < slicingdata[i].size(); j++)
        {
            for (unsigned long k = 0; k < slicingdata[i][j].size(); k++)
            {
                positionData[count+0] = slicingdata[i][j][k].x;
                positionData[count+1] = slicingdata[i][j][k].y;
                positionData[count+2] = z_min+i*layerthickness;
                count+=3;
            }
        }
    }
}

bool Patchmodel::slicefacet(unsigned long layernumber)
{
    if (layernumber > slicingdata.size() || layernumber <= 0)
    {
        cout << "选择正确的层数" << endl;
        return false;
    }
    else
    {
        for (unsigned long i = 0; i < slicingdata[layernumber].size(); i++)
        {
            set<int> facetnumber;
            for (unsigned long j = 0; j < slicingdata[layernumber][i].size(); j++)
            {
                facetnumber.insert(m_VectorEdge[slicingdata[layernumber][i][j].edgenumber].FacetIndex[0]);
                facetnumber.insert(m_VectorEdge[slicingdata[layernumber][i][j].edgenumber].FacetIndex[1]);
            }
            slicefacetnumber.push_back(facetnumber);
        }
        return true;
    }
}

void Patchmodel::drawslicefacet(void)
{
    for (unsigned long i = 0; i < slicefacetnumber.size(); i++)
    {
        set<int>::iterator it;
        for (it = slicefacetnumber[i].begin(); it != slicefacetnumber[i].end(); it++)
        {
            glBegin(GL_LINE_LOOP);
            glVertex3f(m_VectorPoint[m_VectorFacet[*it].PointIndex[0]].m_Point[0], m_VectorPoint[m_VectorFacet[*it].PointIndex[0]].m_Point[1], m_VectorPoint[m_VectorFacet[*it].PointIndex[0]].m_Point[2]);
            glVertex3f(m_VectorPoint[m_VectorFacet[*it].PointIndex[1]].m_Point[0], m_VectorPoint[m_VectorFacet[*it].PointIndex[1]].m_Point[1], m_VectorPoint[m_VectorFacet[*it].PointIndex[1]].m_Point[2]);
            glVertex3f(m_VectorPoint[m_VectorFacet[*it].PointIndex[2]].m_Point[0], m_VectorPoint[m_VectorFacet[*it].PointIndex[2]].m_Point[1], m_VectorPoint[m_VectorFacet[*it].PointIndex[2]].m_Point[2]);
            glEnd();
        }
    }
}

void Patchmodel::drawAABB(void)
{
    glColor3f(0.0, 1.0, 0.2);
    glBegin(GL_LINE_LOOP);
    glVertex3f(x_min,y_min,z_min);
    glVertex3f(x_min,y_max,z_min);
    glVertex3f(x_max,y_max,z_min);
    glVertex3f(x_max,y_min,z_min);
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex3f(x_min,y_min,z_max);
    glVertex3f(x_min,y_max,z_max);
    glVertex3f(x_max,y_max,z_max);
    glVertex3f(x_max,y_min,z_max);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(x_min,y_min,z_min);
    glVertex3f(x_min,y_min,z_max);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(x_max,y_min,z_min);
    glVertex3f(x_max,y_min,z_max);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(x_max,y_max,z_min);
    glVertex3f(x_max,y_max,z_max);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(x_min,y_max,z_min);
    glVertex3f(x_min,y_max,z_max);
    glEnd();
}

float Patchmodel::xmax(void)
{
    return x_max;
}

float Patchmodel::ymax(void)
{
    return y_max;
}

float Patchmodel::zmax(void)
{
    return z_max;
}

float Patchmodel::xmin(void)
{
    return x_min;
}

float Patchmodel::ymin(void)
{
    return y_min;
}

float Patchmodel::zmin(void)
{
    return z_min;
}

void Patchmodel::GetPositionData(float* positionData)
{
    for(int i=0;i<m_VectorFacet.size();i++)
    {
        positionData[9*i+0]= m_VectorPoint[m_VectorFacet[i].PointIndex[0]].m_Point[0];
        positionData[9*i+1]= m_VectorPoint[m_VectorFacet[i].PointIndex[0]].m_Point[1];
        positionData[9*i+2]= m_VectorPoint[m_VectorFacet[i].PointIndex[0]].m_Point[2];
        positionData[9*i+3]= m_VectorPoint[m_VectorFacet[i].PointIndex[1]].m_Point[0];
        positionData[9*i+4]= m_VectorPoint[m_VectorFacet[i].PointIndex[1]].m_Point[1];
        positionData[9*i+5]= m_VectorPoint[m_VectorFacet[i].PointIndex[1]].m_Point[2];
        positionData[9*i+6]= m_VectorPoint[m_VectorFacet[i].PointIndex[2]].m_Point[0];
        positionData[9*i+7]= m_VectorPoint[m_VectorFacet[i].PointIndex[2]].m_Point[1];
        positionData[9*i+8]= m_VectorPoint[m_VectorFacet[i].PointIndex[2]].m_Point[2];
    }
}

void Patchmodel::GetIndexData(unsigned int* IndexData)
{
    for(int i=0;i<m_VectorFacet.size();i++)
    {
        IndexData[3*i+0]= m_VectorFacet[i].PointIndex[0];
        IndexData[3*i+1]= m_VectorFacet[i].PointIndex[1];
        IndexData[3*i+2]= m_VectorFacet[i].PointIndex[2];
    }
}
