#pragma once

#include "geometry.h"
#include "Texture.h"

class Triangle
{
private:

public:
    Vector4f v[3]; // ����������������
    Vector3f normal[3]; // ��������ķ���
    Vector2f texCoords[3]; // �����������������
    Vector3f color[3]; // �����������ɫֵ

    Triangle();
    void computeGColor(const Vector3f& light_dir);
    void computeFcolor(const Vector3f& light_dir);
};

