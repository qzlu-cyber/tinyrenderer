#pragma once

/// ÿ�������ģ�ͣ���Ϊ��ӵ�У�
/// 1.һ�ſ��ܴ��ڵ���������ӦTexture * texture��
/// 2.����������Ƭ��ɵ����顣(��Ӧstd::vector<Triangle> TriangleList)

#include <vector>

#include "geometry.h"
#include "tgaimage.h"
#include "Texture.h"
#include "Triangle.h"


class Model {
private:
    Texture* texture = nullptr;
    int verNum, faceNum;

public:
    std::vector<Triangle> TriangleList;

    Model(const char* filename);
    ~Model();
    int nverts(); // ��ȡ������
    int nfaces(); // ��ȡ����
};