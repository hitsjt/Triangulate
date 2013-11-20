#include "stdafx.h"
#include "Triangulation.h"
#include "MathUtility.h"
#include "Intersection.h"
#include <exception>

using namespace std;
namespace Algorithms
{
    bool CTriangulation::Triangulate(vector<CEdgeLoop> &tri_faces, CRegion& region)
    {
		
		vector<CEdge> regionEdges;
		for(EdgeArray::iterator ol_it = region.OuterLoops.Edges.begin();ol_it != region.OuterLoops.Edges.end();ol_it++)
		{
			regionEdges.push_back(*ol_it);
		}

		for(vector<CEdgeLoop>::iterator il_it1 = region.InnerLoops.begin();il_it1 != region.InnerLoops.end();il_it1++)
		{
			CEdgeLoop currentInnerLoop = *il_it1;
			for(EdgeArray::iterator il_it2 = currentInnerLoop.Edges.begin();il_it2 != currentInnerLoop.Edges.end();il_it2++)
			{
				regionEdges.push_back(*il_it2);
			}
		}

		


		while(regionEdges.size() > 3)
		{
			vector<vec2> candidatePoints;
			for(int i = 1;i<regionEdges.size();i++)
			{
				if (CMathUtility::ToLeftExcludeOnLine(regionEdges[i].End,regionEdges[0]))
				{
					int k = 0;
					for(k = 1;k<regionEdges.size();k++)
					{
						CEdge edge1(regionEdges[0].Start,regionEdges[i].End);
						edge1.Commit();

						CEdge edge2(regionEdges[0].End,regionEdges[i].End);
						edge2.Commit();

                        //???????
						if(edge1.IsIntersectWith2(regionEdges[k]) || edge2.IsIntersectWith2(regionEdges[k]))
						{
							break;
						}
					}
					if(k == regionEdges.size())
					{
						candidatePoints.push_back(regionEdges[i].End);
					}


				}
			}

			double minR = MAXDWORD;
			vec2 LO2;

			
			for(int j = 0;j<candidatePoints.size();j++)
			{
				double r =CMathUtility::MinR(regionEdges[0].Start,regionEdges[0].End,candidatePoints[j]);
				if(r < minR)
				{
					minR = r;
					LO2 = candidatePoints[j];
				}

			}

			CEdgeLoop tmpTriangel;
			tmpTriangel.Edges.push_back(regionEdges[0]);
			CEdge tmpEdge1(regionEdges[0].End,LO2);
			tmpEdge1.Commit();
			tmpTriangel.Edges.push_back(tmpEdge1);
			CEdge tmpEdge2(LO2,regionEdges[0].Start);
			tmpEdge2.Commit();
			tmpTriangel.Edges.push_back(tmpEdge2);
			tmpTriangel.Commit();

			vector<vec2> newCandidatePoints;
            for(int m = 0;m<candidatePoints.size();m++)
			{
				if(CMathUtility::IsCanExistInCircle(candidatePoints[m],tmpTriangel))
				{
					newCandidatePoints.push_back(candidatePoints[m]);
				}

			}

			while (!newCandidatePoints.size() ==0)
			{
				minR=MAXDWORD;
				for(int m = 0;m < newCandidatePoints.size();m++)
				{
					double r=CMathUtility::MinR(regionEdges[0].Start,regionEdges[0].End,newCandidatePoints[m]);
					if(r < minR)
					{
						minR = r;
						LO2 = newCandidatePoints[m];
					}

				}
				
				tmpTriangel.Edges.clear();
				tmpTriangel.Edges.push_back(regionEdges[0]);
				tmpEdge1.Start = regionEdges[0].End;
				tmpEdge1.End = LO2;
				tmpTriangel.Edges.push_back(tmpEdge1);
				tmpEdge2.Start = LO2;
				tmpEdge2.End = regionEdges[0].Start;
				tmpTriangel.Edges.push_back(tmpEdge2);
				tmpTriangel.Commit();

				newCandidatePoints.clear();
				for (int j = 0;j<candidatePoints.size();j++)
				{
					if (CMathUtility::IsCanExistInCircle(candidatePoints[j],tmpTriangel))
					{
						newCandidatePoints.push_back(candidatePoints[j]);
					}
				}

			}

			tri_faces.push_back(tmpTriangel);

			CEdge L11_LO2(regionEdges[0].Start,LO2);
			L11_LO2.isBoundEdge = false;
			L11_LO2.Commit();
			CEdge L12_LO2(LO2,regionEdges[0].End);
			L12_LO2.isBoundEdge =false;
			L12_LO2.Commit();

			int pos_m = 0;
			int pos_n = 0;

			if(CMathUtility::IsBoundEdge(regionEdges,L11_LO2,pos_m))
			{
				L11_LO2.isBoundEdge = true;
			}
			if(CMathUtility::IsBoundEdge(regionEdges,L12_LO2,pos_n))
			{
				L12_LO2.isBoundEdge = true;
			}

			if(!L11_LO2.isBoundEdge && !L12_LO2.isBoundEdge)
			{
				CEdge tmpEdge(L12_LO2.Start,L12_LO2.End);
				tmpEdge.Commit();
				regionEdges.push_back(tmpEdge);
				regionEdges[0].End = LO2;
				regionEdges[0].Commit();
			}
			if(L11_LO2.isBoundEdge && !L12_LO2.isBoundEdge)
			{
				regionEdges[0].Start = LO2;
				regionEdges[0].Commit();
				regionEdges[pos_m] = regionEdges[regionEdges.size()-1];
				regionEdges.pop_back();
			}
			if(!L11_LO2.isBoundEdge && L12_LO2.isBoundEdge)
			{
				regionEdges[0].End = LO2;
				regionEdges[0].Commit();
				regionEdges[pos_n] = regionEdges[regionEdges.size()-1];
				regionEdges.pop_back();
			}
			if(L11_LO2.isBoundEdge && L12_LO2.isBoundEdge)
			{
				if (pos_n>pos_m)
				{
					regionEdges[pos_n] = regionEdges[regionEdges.size()-1];
					regionEdges.pop_back();

					regionEdges[pos_m] = regionEdges[regionEdges.size()-1];
					regionEdges.pop_back();
					
					regionEdges[0] = regionEdges[regionEdges.size()-1];
					regionEdges.pop_back();
				}
				else
				{
					regionEdges[pos_m] = regionEdges[regionEdges.size()-1];
					regionEdges.pop_back();

					regionEdges[pos_n] = regionEdges[regionEdges.size()-1];
					regionEdges.pop_back();

					regionEdges[0] = regionEdges[regionEdges.size()-1];
					regionEdges.pop_back();

				}

			}

		}

        // TODO
		if(regionEdges.size()==3)
		{
			CEdgeLoop tmpTriangle;
			tmpTriangle.Edges.push_back(regionEdges[0]);
			tmpTriangle.Edges.push_back(regionEdges[1]);
			tmpTriangle.Edges.push_back(regionEdges[2]);
			tmpTriangle.Commit();
			tri_faces.push_back(tmpTriangle);
		}
        return true;
    }


}

class Utils
{
public:
   // double waixin(vec2*,vec2*,vec2*);//求外接圆半径
    //int	Area2(vec2*,vec2*,vec2*);//由三点坐标求三角形面积
    //bool ToLeft(vec2*,vec2*,vec2*);//判断某点是否在一条直线的左侧

	static bool IsToLeft(vec2 point,CEdge edge)
	{
		int r = Area2(&(edge.Start),&(edge.Start),&point);
		return (r<0);

	}
     /**
    *	Check if point x lies left to the directed ray rooted at a and passing b
    */
    static bool	ToLeft(vec2* pa,vec2*	pb,vec2* px)
    {
	    int r=Area2(pa, pb, px);
	    return(r<0);
    }

    static bool intersect(vec2* pP1,vec2* pP2,vec2* pP3,vec2* pP4)              //线段求交
    {
	
	    if(pP1->index==pP3->index||pP2->index==pP3->index||pP1->index==pP4->index||pP2->index==pP4->index)
		    return FALSE;
	    else
	    {
		    if((Area2(pP1,pP2,pP3)*Area2(pP1,pP2,pP4)<=0) && (Area2(pP3,pP4,pP1)*Area2(pP3,pP4,pP2)<=0))	
			    return TRUE;
		    else 
			    return FALSE;
	    }
    }

    /**
    *	Get twice the signed area of the triangle determined by a, b, c
    *	Positive if a, b, and c define a CCW turn, and
    *	Negative if a CW turn
    */
    static int	Area2(vec2* pa,vec2* pb,vec2* pc)
    {
	    double r=(
		    pa->x * pb->y - pa->y * pb->x
		    +	pb->x * pc->y - pb->y * pc->x
		    +	pc->x * pa->y - pc->y * pa->x);
	    if (r>0)
	    {
		    return 1;
	    }
	    else if (0==r)
	    {
		    return 0;
	    }
	    else 
	    {
		    return -1;
	    }
    }

    static double waixin(vec2& p1,vec2& p2,vec2& p3)                    //求外接圆半径
    {
		double a,b,c,s;
		a = p1.Distance(p2);
		b = p2.Distance(p3);
		c = p3.Distance(p1);
        
        s=(a+b+c)/2;
        s=sqrt(s*(s-a)*(s-b)*(s-c));
        double circler=a*b*c/(4*s);
        return circler;
    }
	static bool IsCanExistInCircle(vec2& point,CEdgeLoop & triangel)
	{
		bool exist=FALSE;
		if(point != triangel.Edges[1].End)
		{
			if(Utils::IsToLeft(point,triangel.Edges[0])&&Utils::IsToLeft(point,triangel.Edges[1])&&Utils::IsToLeft(point,triangel.Edges[2]))
			{
				exist = TRUE;
			}
		}
		return exist;

	}


	static bool IsBoundEdge(vector<CEdge> &regionEdges,CEdge &edge,int &M)
	{
		for (int i = 1;i<regionEdges.size();i++)
		{
			if ((regionEdges[i].Start == edge.Start && regionEdges[i].End==edge.End) ||(regionEdges[i].Start == edge.End && regionEdges[i].End==edge.Start))
			{
				edge.isBoundEdge=true;
				M = i;
			}
		}
	}


};
