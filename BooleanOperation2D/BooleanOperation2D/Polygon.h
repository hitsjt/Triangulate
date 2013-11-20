#pragma once
#include "Vector.h"
#include "BoundingBox.h"
#include <vector>

using namespace Math;

enum EdgeIncludeType // 线段包含测试时的返回类型
{
    Include,
    NotInclude,
    OverlapPlus,    // 两条边重合方向相同
    OverlapMinus    // 两条边重合但方向相反
};

enum ELoopType // 环的种类
{
    Unknown = 0,
    OuterLoop , // 逆时针为外环
    InnerLoop   // 顺时针为内环
};


class CEdge
{
public:
	bool isBoundEdge; //modified
    vec2 Start;         // start point of edge
    vec2 End;
    CBoundingBox Box;
    EdgeIncludeType Type;

public:
    CEdge(void) : Start(vec2(0.0, 0.0)), End(vec2(0.0, 0.0)) {Commit();}
    CEdge(const vec2& start, const vec2& end): Start(start), End(end) {Commit();}
    // update bounding box, call this when Start or End is modified
    void Commit();
    bool IsValid();
    bool IsIntersectWith(const CEdge& edge) const;  // 只要有共同点，均视为相交
    bool IsIntersectWith1(const CEdge& edge) const; // 线段一个顶点重合，视为不交
	
    bool IsPointOn(const vec2 point, int & a, const CTolerance &tolerance) const;
    void Move(const vec2 direction); // 平移

	bool IsIntersectWith2(const CEdge& edge) const; // 线段一个顶点重合\线段共线，视为不交
};


typedef std::vector<CEdge> EdgeArray;
class CEdgeLoop
{
public:
    EdgeArray Edges;
    ELoopType Type;
    CBoundingBox Box;

public:
    void Commit();
    bool IsValid();
    bool IsIntersectWith(const CEdgeLoop &edgeloop) const;
    bool IsPointOn(const vec2 &point, int & a, int &b, const CTolerance &tolerance) const;
    void Move(const vec2 direction); // 平移
};


typedef std::vector<CEdgeLoop> LoopArray;
typedef std::vector<vec2> PointArray;
class CRegion
{
public:
    CEdgeLoop OuterLoops; // only one
    LoopArray InnerLoops; // anyone
    CBoundingBox Box;

public:
    void Commit();
    bool IsIntersectWith(const CRegion &region) const;
    void AddOuterloop(const PointArray &points);
    void AddInnerloops(const PointArray &points);
    bool IsPointOn(const vec2 &point, int &a, int &b, int &c, const CTolerance &tolerance) const;
    void Move(const vec2 direction); // 平移
};


typedef std::vector<CRegion> RegionArray;
class CPolygon
{
public:
    RegionArray Regions;
    CBoundingBox Box;

public:
    void Commit();
    void Serialize(CArchive& ar);
    void Dispose();
    bool IsPointOn(const vec2 &point, int &a, int &b, int &c, int &d, const CTolerance &tolerance) const;
    void Move(const vec2 direction); // 平移

private:
    void ParseVector(vec2 &vec, const char* line);
    void ParseNumber(int &num, const char* line);
};

