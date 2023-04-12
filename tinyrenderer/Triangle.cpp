#include <iostream>

#include "Triangle.h"

Triangle::Triangle()
{
    v[0] = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
    v[1] = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
    v[2] = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);

    color[0] = Vector3f(0.0f, 0.0f, 0.0f);
    color[1] = Vector3f(0.0f, 0.0f, 0.0f);
    color[2] = Vector3f(0.0f, 0.0f, 0.0f);

    texCoords[0] = Vector2f(0.0f, 0.0f);
    texCoords[1] = Vector2f(0.0f, 0.0f);
    texCoords[2] = Vector2f(0.0f, 0.0f);

    normal[0] = Vector3f(0.0f, 0.0f, 0.0f);
    normal[1] = Vector3f(0.0f, 0.0f, 0.0f);
    normal[2] = Vector3f(0.0f, 0.0f, 0.0f);
}

void Triangle::computeGColor(const Vector3f& light_dir)
{
    for (int i = 0; i < 3; i++)
    {
        float intensity = std::max(0.f, normal[i].normalized().dot(light_dir.normalized()));
        //std::cout << intensity << std::endl;
        color[i] = Vector3f(255, 255, 255) * intensity;
        //std::cout << color[i] << std::endl;
    }
}

void Triangle::computeFcolor(const Vector3f& light_dir)
{
    Vector3f normal_ = (Vector3f(v[2].x(), v[2].y(), v[2].z()) - Vector3f(v[0].x(), v[0].y(), v[0].z())).cross(Vector3f(v[1].x(), v[1].y(), v[1].z()) - Vector3f(v[0].x(), v[0].y(), v[0].z()));//计算三角形法线
    float intensity = std::max(0.f, normal_.normalized().dot(light_dir.normalized()));

    for (int i = 0; i < 3; i++)
    {
        color[i] = Vector3f(255, 255, 255) * intensity;
        //std::cout << color[i] << std::endl;
    }
}
