#pragma once
#include "Vector.h"

class CBoundingBox
{
public:
    vec2 Min;
    vec2 Max;
public:
    CBoundingBox(void) {}
    CBoundingBox(const vec2& min, const vec2& max): Min(min), Max(max) {}
    ~CBoundingBox(void) {}

public:
    bool IsIntersectWith(const CBoundingBox& box) const;
    bool Contains(const vec2& point) const;
    void Union(const CBoundingBox& box);
};

