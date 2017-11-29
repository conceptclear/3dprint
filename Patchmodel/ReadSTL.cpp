#include "Patchmodel.h"
bool Patchmodel::ReadASCII(const char *cfilename)
{
	int i = 0, j = 0, cnt = 0, pCnt = 4;
	int p_point = 0; //µãµÄË÷ÒýºÅ
	int p_edge = 0; //±ßµÄË÷ÒýºÅ
	int p_facet = 0; //ÃæµÄË÷ÒýºÅ
	int flag = 0;//Ñ°ÕÒµÚÒ»žöµã
	char a[100];
	char str[100];
	double x = 0, y = 0, z = 0;


	ifstream in;
	in.open(cfilename, ios::in);
	if (!in)
		return false;
	do
	{
		i = 0;
		cnt = 0;
		in.getline(a, 100, '\n');
		while (a[i] != '\0')
		{
			if (!islower((int)a[i]) && !isupper((int)a[i]) && a[i] != ' ')
				break;
			cnt++;
			i++;
		}

		while (a[cnt] != '\0')
		{
			str[j] = a[cnt];
			cnt++;
			j++;
		}
		str[j] = '\0';
		j = 0;

		if (sscanf(str, "%lf%lf%lf", &x, &y, &z) == 3)
		{
			flag++;
			if (flag % 4 == 1)
			{
				//œ«ÃæÆ¬·šÏòÁ¿ŽæÈëm_Facet
				m_Facet.m_Normal[0] = x;
				m_Facet.m_Normal[1] = y;
				m_Facet.m_Normal[2] = z;
			}
			else
			{
				int f_remainder = flag % 4 - 2; //ÈýœÇÃæÆ¬ÄÚ²¿µãË³Ðò
				if (f_remainder < 0)
					f_remainder = f_remainder + 4;
				CVertex a;
				a.m_Point[0] = x;
				a.m_Point[1] = y;
				a.m_Point[2] = z;
				set<CVertex>::iterator iter;
				iter = m_SetPoint.find(a); //²éÕÒÏàÍ¬µã
				if (iter != m_SetPoint.end())  //ÕÒµœ
				{
					m_Facet.PointIndex[f_remainder] = (*iter).m_PointIndex; //ÕÒµœœ«žÃµãË÷ÒýºÅŽæÈëm_Facet.PointIndex
				}
				else            //ÎŽÕÒµœ
				{
					a.m_PointIndex = p_point;
					m_Facet.PointIndex[f_remainder] = p_point;
					p_point++;
					m_SetPoint.insert(a);
				}
			}
			if (flag % 4 == 0 && flag != 0)
			{
				CEdge edge1, edge2, edge3;

				if (m_Facet.PointIndex[0] > m_Facet.PointIndex[1])
				{
					edge1.PointIndex[0] = m_Facet.PointIndex[0];
					edge1.PointIndex[1] = m_Facet.PointIndex[1];
				}
				else
				{
					edge1.PointIndex[0] = m_Facet.PointIndex[1];
					edge1.PointIndex[1] = m_Facet.PointIndex[0];
				}

				if (m_Facet.PointIndex[0] > m_Facet.PointIndex[2])
				{
					edge2.PointIndex[0] = m_Facet.PointIndex[0];
					edge2.PointIndex[1] = m_Facet.PointIndex[2];
				}
				else
				{
					edge2.PointIndex[0] = m_Facet.PointIndex[2];
					edge2.PointIndex[1] = m_Facet.PointIndex[0];
				}

				if (m_Facet.PointIndex[1] > m_Facet.PointIndex[2])
				{
					edge3.PointIndex[0] = m_Facet.PointIndex[1];
					edge3.PointIndex[1] = m_Facet.PointIndex[2];
				}
				else
				{
					edge3.PointIndex[0] = m_Facet.PointIndex[2];
					edge3.PointIndex[1] = m_Facet.PointIndex[1];
				}

				set<CEdge>::iterator iter1; //µÚÒ»Ìõ±ß±éÀú
				set<CEdge>::iterator iter2; //µÚ¶þÌõ±ß±éÀú
				set<CEdge>::iterator iter3; //µÚÈýÌõ±ß±éÀú

				iter1 = m_SetEdge.find(edge1); //²éÕÒÏàÍ¬±ß
				if (iter1 != m_SetEdge.end())  //ÕÒµœ
				{
					edge1.FacetIndex[0] = (*iter1).FacetIndex[0];
					edge1.FacetIndex[1] = p_facet;  //ÈôÒÑŽæÔÚÔò°Ñµ±Ç°ÃæµÄË÷ÒýºÅŽæÈëFacetIndex[1]
					edge1.m_EdgeIndex = (*iter1).m_EdgeIndex; //Ë÷ÒýÖµžŽÖÆ
					m_SetEdge.erase(iter1); //ÉŸ³ýÔ­œáµã
					m_SetEdge.insert(edge1); //²åÈëÐÂµÄœÚµã
				}
				else            //ÎŽÕÒµœ
				{
					edge1.FacetIndex[0] = p_facet;  //Èô²»ŽæÔÚÔò°Ñµ±Ç°ÃæµÄË÷ÒýºÅŽæÈëFacetIndex[0]
					edge1.m_EdgeIndex = p_edge; //¶šÒåË÷ÒýºÅ
					m_SetEdge.insert(edge1); //²åÈëµ±Ç°±ß
					p_edge++;
				}

				iter2 = m_SetEdge.find(edge2); //²éÕÒÏàÍ¬±ß
				if (iter2 != m_SetEdge.end())  //ÕÒµœ
				{
					edge2.FacetIndex[0] = (*iter2).FacetIndex[0];
					edge2.FacetIndex[1] = p_facet;  //ÈôÒÑŽæÔÚÔò°Ñµ±Ç°ÃæµÄË÷ÒýºÅŽæÈëFacetIndex[1]
					edge2.m_EdgeIndex = (*iter2).m_EdgeIndex; //Ë÷ÒýÖµžŽÖÆ
					m_SetEdge.erase(iter2); //ÉŸ³ýÔ­œáµã
					m_SetEdge.insert(edge2); //²åÈëÐÂµÄœÚµã
				}
				else            //ÎŽÕÒµœ
				{
					edge2.FacetIndex[0] = p_facet;  //Èô²»ŽæÔÚÔò°Ñµ±Ç°ÃæµÄË÷ÒýºÅŽæÈëFacetIndex[0]
					edge2.m_EdgeIndex = p_edge; //¶šÒåË÷ÒýºÅ
					m_SetEdge.insert(edge2); //²åÈëµ±Ç°±ß
					p_edge++;
				}

				iter3 = m_SetEdge.find(edge3); //²éÕÒÏàÍ¬±ß
				if (iter3 != m_SetEdge.end())  //ÕÒµœ
				{
					edge3.FacetIndex[0] = (*iter3).FacetIndex[0];
					edge3.FacetIndex[1] = p_facet;  //ÈôÒÑŽæÔÚÔò°Ñµ±Ç°ÃæµÄË÷ÒýºÅŽæÈëFacetIndex[1]
					edge3.m_EdgeIndex = (*iter3).m_EdgeIndex; //Ë÷ÒýÖµžŽÖÆ
					m_SetEdge.erase(iter3); //ÉŸ³ýÔ­œáµã
					m_SetEdge.insert(edge3); //²åÈëÐÂµÄœÚµã
				}
				else            //ÎŽÕÒµœ
				{
					edge3.FacetIndex[0] = p_facet;  //Èô²»ŽæÔÚÔò°Ñµ±Ç°ÃæµÄË÷ÒýºÅŽæÈëFacetIndex[0]
					edge3.m_EdgeIndex = p_edge; //¶šÒåË÷ÒýºÅ
					m_SetEdge.insert(edge3); //²åÈëµ±Ç°±ß
					p_edge++;
				}
				m_Facet.EdgeIndex[0] = edge1.m_EdgeIndex;
				m_Facet.EdgeIndex[1] = edge2.m_EdgeIndex;
				m_Facet.EdgeIndex[2] = edge3.m_EdgeIndex;
				m_Facet.m_FacetIndex = p_facet;
				m_VectorFacet.push_back(m_Facet);
				p_facet++;
			}
		}
		pCnt++;
	} while (!in.eof());

	//  cout << "******  ACSII FILES¡¡******" << endl;  
	//  for (int i = 0; i < coorX.size();i++)  
	//  {  
	//      cout << coorX[i] << " : " << coorY[i] << " : " << coorZ[i] << endl;  
	//  }  

	std::copy(m_SetPoint.begin(), m_SetPoint.end(), std::back_inserter(m_VectorPoint));
	std::copy(m_SetEdge.begin(), m_SetEdge.end(), std::back_inserter(m_VectorEdge));
	std::sort(m_VectorPoint.begin(), m_VectorPoint.end(), LessPointIndex());
	std::sort(m_VectorEdge.begin(), m_VectorEdge.end(), LessEdgeIndex());

	x_max = (*max_element(m_VectorPoint.begin(), m_VectorPoint.end(), LessPointx())).m_Point[0];
	x_min = (*min_element(m_VectorPoint.begin(), m_VectorPoint.end(), LessPointx())).m_Point[0];
	y_max = (*max_element(m_VectorPoint.begin(), m_VectorPoint.end(), LessPointy())).m_Point[1];
	y_min = (*min_element(m_VectorPoint.begin(), m_VectorPoint.end(), LessPointy())).m_Point[1];
	z_max = (*max_element(m_VectorPoint.begin(), m_VectorPoint.end(), LessPointz())).m_Point[2];
	z_min = (*min_element(m_VectorPoint.begin(), m_VectorPoint.end(), LessPointz())).m_Point[2];

	vector<CFacet>::iterator iterfacet;
	for (iterfacet = m_VectorFacet.begin(); iterfacet != m_VectorFacet.end(); iterfacet++)
	{
		for (int i = 0; i <= 2; i++)
		{
			if (m_VectorEdge[(*iterfacet).EdgeIndex[i]].FacetIndex[0] == (*iterfacet).m_FacetIndex)
				(*iterfacet).FacetIndex[i] = m_VectorEdge[(*iterfacet).EdgeIndex[i]].FacetIndex[1];
			else
				(*iterfacet).FacetIndex[i] = m_VectorEdge[(*iterfacet).EdgeIndex[i]].FacetIndex[0];
		}
	}

	cout << flag / 4 << " triangles." << endl;
	return true;
}

bool Patchmodel::ReadBinary(const char *cfilename)
{
	char str[80];
	ifstream in;
	int p_point = 0; //µãµÄË÷ÒýºÅ
	int p_edge = 0; //±ßµÄË÷ÒýºÅ
	int p_facet = 0; //ÃæµÄË÷ÒýºÅ

	in.open(cfilename, ios::in | ios::binary);

	if (!in)
		return false;

	in.read(str, 80);

	//number of triangles  
	int unTriangles;
	in.read((char*)&unTriangles, sizeof(int));

	if (unTriangles == 0)
		return false;

	for (int i = 0; i < unTriangles; i++)
	{
		float coorXYZ[12];
		in.read((char*)coorXYZ, 12 * sizeof(float));
		m_Facet.m_Normal[0] = coorXYZ[0];
		m_Facet.m_Normal[1] = coorXYZ[1];
		m_Facet.m_Normal[2] = coorXYZ[2];

		for (int j = 1; j < 4; j++)
		{

			int f_remainder = j - 1; //ÈýœÇÃæÆ¬ÄÚ²¿µãË³Ðò
			CVertex a;
			a.m_Point[0] = coorXYZ[j * 3];
			a.m_Point[1] = coorXYZ[j * 3 + 1];
			a.m_Point[2] = coorXYZ[j * 3 + 2];
			set<CVertex>::iterator iter;
			iter = m_SetPoint.find(a); //²éÕÒÏàÍ¬µã
			if (iter != m_SetPoint.end())  //ÕÒµœ
			{
				m_Facet.PointIndex[f_remainder] = (*iter).m_PointIndex; //ÕÒµœœ«žÃµãË÷ÒýºÅŽæÈëm_Facet.PointIndex
			}
			else            //ÎŽÕÒµœ
			{
				a.m_PointIndex = p_point;
				m_Facet.PointIndex[f_remainder] = p_point;
				p_point++;
				m_SetPoint.insert(a);
			}
		}

		in.read((char*)coorXYZ, 2);

		CEdge edge1, edge2, edge3;

		if (m_Facet.PointIndex[0] > m_Facet.PointIndex[1])
		{
			edge1.PointIndex[0] = m_Facet.PointIndex[0];
			edge1.PointIndex[1] = m_Facet.PointIndex[1];
		}
		else
		{
			edge1.PointIndex[0] = m_Facet.PointIndex[1];
			edge1.PointIndex[1] = m_Facet.PointIndex[0];
		}

		if (m_Facet.PointIndex[0] > m_Facet.PointIndex[2])
		{
			edge2.PointIndex[0] = m_Facet.PointIndex[0];
			edge2.PointIndex[1] = m_Facet.PointIndex[2];
		}
		else
		{
			edge2.PointIndex[0] = m_Facet.PointIndex[2];
			edge2.PointIndex[1] = m_Facet.PointIndex[0];
		}

		if (m_Facet.PointIndex[1] > m_Facet.PointIndex[2])
		{
			edge3.PointIndex[0] = m_Facet.PointIndex[1];
			edge3.PointIndex[1] = m_Facet.PointIndex[2];
		}
		else
		{
			edge3.PointIndex[0] = m_Facet.PointIndex[2];
			edge3.PointIndex[1] = m_Facet.PointIndex[1];
		}

		set<CEdge>::iterator iter1; //µÚÒ»Ìõ±ß±éÀú
		set<CEdge>::iterator iter2; //µÚ¶þÌõ±ß±éÀú
		set<CEdge>::iterator iter3; //µÚÈýÌõ±ß±éÀú

		iter1 = m_SetEdge.find(edge1); //²éÕÒÏàÍ¬±ß
		if (iter1 != m_SetEdge.end())  //ÕÒµœ
		{
			edge1.FacetIndex[0] = (*iter1).FacetIndex[0];
			edge1.FacetIndex[1] = p_facet;  //ÈôÒÑŽæÔÚÔò°Ñµ±Ç°ÃæµÄË÷ÒýºÅŽæÈëFacetIndex[1]
			edge1.m_EdgeIndex = (*iter1).m_EdgeIndex; //Ë÷ÒýÖµžŽÖÆ
			m_SetEdge.erase(iter1); //ÉŸ³ýÔ­œáµã
			m_SetEdge.insert(edge1); //²åÈëÐÂµÄœÚµã
		}
		else            //ÎŽÕÒµœ
		{
			edge1.FacetIndex[0] = p_facet;  //Èô²»ŽæÔÚÔò°Ñµ±Ç°ÃæµÄË÷ÒýºÅŽæÈëFacetIndex[0]
			edge1.m_EdgeIndex = p_edge; //¶šÒåË÷ÒýºÅ
			m_SetEdge.insert(edge1); //²åÈëµ±Ç°±ß
			p_edge++;
		}

		iter2 = m_SetEdge.find(edge2); //²éÕÒÏàÍ¬±ß
		if (iter2 != m_SetEdge.end())  //ÕÒµœ
		{
			edge2.FacetIndex[0] = (*iter2).FacetIndex[0];
			edge2.FacetIndex[1] = p_facet;  //ÈôÒÑŽæÔÚÔò°Ñµ±Ç°ÃæµÄË÷ÒýºÅŽæÈëFacetIndex[1]
			edge2.m_EdgeIndex = (*iter2).m_EdgeIndex; //Ë÷ÒýÖµžŽÖÆ
			m_SetEdge.erase(iter2); //ÉŸ³ýÔ­œáµã
			m_SetEdge.insert(edge2); //²åÈëÐÂµÄœÚµã
		}
		else            //ÎŽÕÒµœ
		{
			edge2.FacetIndex[0] = p_facet;  //Èô²»ŽæÔÚÔò°Ñµ±Ç°ÃæµÄË÷ÒýºÅŽæÈëFacetIndex[0]
			edge2.m_EdgeIndex = p_edge; //¶šÒåË÷ÒýºÅ
			m_SetEdge.insert(edge2); //²åÈëµ±Ç°±ß
			p_edge++;
		}

		iter3 = m_SetEdge.find(edge3); //²éÕÒÏàÍ¬±ß
		if (iter3 != m_SetEdge.end())  //ÕÒµœ
		{
			edge3.FacetIndex[0] = (*iter3).FacetIndex[0];
			edge3.FacetIndex[1] = p_facet;  //ÈôÒÑŽæÔÚÔò°Ñµ±Ç°ÃæµÄË÷ÒýºÅŽæÈëFacetIndex[1]
			edge3.m_EdgeIndex = (*iter3).m_EdgeIndex; //Ë÷ÒýÖµžŽÖÆ
			m_SetEdge.erase(iter3); //ÉŸ³ýÔ­œáµã
			m_SetEdge.insert(edge3); //²åÈëÐÂµÄœÚµã
		}
		else            //ÎŽÕÒµœ
		{
			edge3.FacetIndex[0] = p_facet;  //Èô²»ŽæÔÚÔò°Ñµ±Ç°ÃæµÄË÷ÒýºÅŽæÈëFacetIndex[0]
			edge3.m_EdgeIndex = p_edge; //¶šÒåË÷ÒýºÅ
			m_SetEdge.insert(edge3); //²åÈëµ±Ç°±ß
			p_edge++;
		}
		m_Facet.EdgeIndex[0] = edge1.m_EdgeIndex;
		m_Facet.EdgeIndex[1] = edge2.m_EdgeIndex;
		m_Facet.EdgeIndex[2] = edge3.m_EdgeIndex;
		m_Facet.m_FacetIndex = p_facet;
		m_VectorFacet.push_back(m_Facet);
		p_facet++;

	}

	in.close();

	std::copy(m_SetPoint.begin(), m_SetPoint.end(), std::back_inserter(m_VectorPoint));
	std::copy(m_SetEdge.begin(), m_SetEdge.end(), std::back_inserter(m_VectorEdge));
	std::sort(m_VectorPoint.begin(), m_VectorPoint.end(), LessPointIndex());
	std::sort(m_VectorEdge.begin(), m_VectorEdge.end(), LessEdgeIndex());

	x_max = (*max_element(m_VectorPoint.begin(), m_VectorPoint.end(), LessPointx())).m_Point[0];
	x_min = (*min_element(m_VectorPoint.begin(), m_VectorPoint.end(), LessPointx())).m_Point[0];
	y_max = (*max_element(m_VectorPoint.begin(), m_VectorPoint.end(), LessPointy())).m_Point[1];
	y_min = (*min_element(m_VectorPoint.begin(), m_VectorPoint.end(), LessPointy())).m_Point[1];
	z_max = (*max_element(m_VectorPoint.begin(), m_VectorPoint.end(), LessPointz())).m_Point[2];
	z_min = (*min_element(m_VectorPoint.begin(), m_VectorPoint.end(), LessPointz())).m_Point[2];

	vector<CFacet>::iterator iterfacet;
	for (iterfacet = m_VectorFacet.begin(); iterfacet != m_VectorFacet.end(); iterfacet++)
	{
		for (int i = 0; i <= 2; i++)
		{
			if (m_VectorEdge[(*iterfacet).EdgeIndex[i]].FacetIndex[0] == (*iterfacet).m_FacetIndex)
				(*iterfacet).FacetIndex[i] = m_VectorEdge[(*iterfacet).EdgeIndex[i]].FacetIndex[1];
			else
				(*iterfacet).FacetIndex[i] = m_VectorEdge[(*iterfacet).EdgeIndex[i]].FacetIndex[0];
		}
	}

	cout << m_VectorFacet.size() << " triangles." << endl;
	return true;
}

bool Patchmodel::ReadSTLFile(const char *cfilename)
{
	if (cfilename == NULL)
		return false;

	ifstream in(cfilename, ios::in);
	if (!in)
		return false;

	string headStr;
	string SecondStr;
	getline(in, headStr);
	getline(in, SecondStr);
	in.close();

	if (headStr.empty())
		return false;

    int noempty=0;
    while(SecondStr[noempty]==' ')
        noempty++;
    if ((headStr[0] == 's') && (SecondStr[noempty] == 'f'))
	{
		cout << "ASCII File." << endl;
		ReadASCII(cfilename);
	}
	else
	{
		cout << "Binary File." << endl;
		ReadBinary(cfilename);
	}
	return true;
}

