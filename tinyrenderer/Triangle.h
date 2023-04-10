#pragma once

#include "geometry.h"
#include "Texture.h"

class Triangle
{
private:

public:
    Vector4f v[3]; // 三个顶点的齐次坐标
    Vector3f normal[3]; // 三个顶点的法线
    Vector2f texCoords[3]; // 三个顶点的纹理坐标
    Vector3f color[3]; // 三个顶点的颜色值

    Triangle();
    void computeGColor(const Vector3f& light_dir);
    void computeFcolor(const Vector3f& light_dir);
};

