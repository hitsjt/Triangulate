#pragma once
#include "Vector.h"
#include "Polygon.h"
#include "BoundingBox.h"

namespace Math
{
    enum ToLeftTestResult
    {
        Right = 0,
        Left = 1,
        OnLine = 2,
    };

    class CMathUtility
    {
    public:
		static bool IsCanExistInCircle(vec2& point,CEdgeLoop & triangel)
		{
			bool exist=FALSE;
			if(point != triangel.Edges[1].End)
			{
				if(CMathUtility::ToLeftExcludeOnLine(point,triangel.Edges[0])&&CMathUtility::ToLeftExcludeOnLine(point,triangel.Edges[1])&&CMathUtility::ToLeftExcludeOnLine(point,triangel.Edges[2]))
				{
					exist = TRUE;
				}
			}
			return exist;
		}
		static bool IsBoundEdge(std::vector<CEdge>& regionEdges,CEdge &edge,int &M)
		{
			for (int i = 1;i<regionEdges.size();i++)
			{
				if ((regionEdges[i].Start == edge.Start && regionEdges[i].End==edge.End) ||(regionEdges[i].Start == edge.End && regionEdges[i].End==edge.Start))
				{
					
					M = i;
					return true;
				}
			}
			return false;
		}

		static double MinR(vec2& p1,vec2& p2,vec2& p3)                    //ÇóÍâ½ÓÔ²°ë¾¶
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

        // to left test
        static ToLeftTestResult ToLeft(const vec2& p, CEdge &edge)
        {
            return ToLeft(p, edge.Start, edge.End);
        }

        static ToLeftTestResult ToLeft(const vec2& p, const vec2& a, const vec2& b)
        {
            double cross = BasicToLeftTest(p, a, b);
            if (Tolerance.IsZero(cross))
                return ToLeftTestResult::OnLine;
            return cross < 0.0 ? ToLeftTestResult::Left : ToLeftTestResult::Right;
        }

        static bool ToLeftIncludeOnLine(const vec2& p, CEdge& edge)
        {
            return ToLeftIncludeOnLine(p, edge.Start, edge.End);
        }

        static bool ToLeftIncludeOnLine(const vec2& p, const vec2& a, const vec2& b)
        {
            ToLeftTestResult result = ToLeft(p, a, b);
            return (result == ToLeftTestResult::Left || result == ToLeftTestResult::OnLine);
        }

        static bool ToLeftExcludeOnLine(const vec2& p, CEdge edge)
        {
            return ToLeftExcludeOnLine(p, edge.Start, edge.End);
        }

        static bool ToLeftExcludeOnLine(const vec2& p, const vec2& a, const vec2& b)
        {
            ToLeftTestResult result = ToLeft(p, a, b);
            return (result == ToLeftTestResult::Left);
        }
    private:
        static double BasicToLeftTest(const vec2& p, const vec2& a, const vec2& b)
        {
            vec2 ab = b - a;
            vec2 ap = p - a;
            return ab ^ ap;
        }

    public:

        // get the parameter t of the intersection point P of AB and CD
        // P = A + t*AB
        // return false if t is invalid result (maybe AB and CD are parallel or overlap)
        static bool SolveIntersectParameter(double& t, const vec2& A, const vec2& B, const vec2& C, const vec2& D)
        {
            vec2 AB = B - A;  // AB
            vec2 CD = D - C;  // CD
            bool is_dir0_zero = Tolerance.IsZero(AB);
            bool is_dir1_zero = Tolerance.IsZero(CD);
            if (is_dir0_zero && is_dir1_zero) // same point ?
            {
                vec2 mid_ab = (A + B) * 0.5;
                vec2 mid_cd = (C + D) * 0.5;
                if (Tolerance.AreEqual(mid_ab, mid_cd))
                {
                    t = 0.5;
                    return true;
                }
                else
                    return false;
            }
            else if (is_dir0_zero) // A/B on CD ?
            {
                vec2 mid_ab = (A + B) * 0.5;
                ToLeftTestResult result = ToLeft(mid_ab, C, D);
                if (result == ToLeftTestResult::OnLine)
                {
                    t = 0.5;
                    return true;
                }
                else
                    return false;
            }
            else if (is_dir1_zero) // C/D on AB ?
            {
                vec2 mid_cd = (C + D) * 0.5;
                ToLeftTestResult result = ToLeft(mid_cd, A, B);
                if (result == ToLeftTestResult::OnLine)
                {
                    t = 0.5;
                    return true;
                }
                else
                    return false;
            }

            double cross = AB ^ CD;

            if (Tolerance.IsZero(cross))
                return false;

            vec2 p = C - A;
            t = (p ^ CD) / cross;
            return true;
        }
    };
}