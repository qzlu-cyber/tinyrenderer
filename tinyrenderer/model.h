#pragma once

/// 每个导入的模型，认为它拥有：
/// 1.一张可能存在的纹理；（对应Texture * texture）
/// 2.所有三角面片组成的数组。(对应std::vector<Triangle> TriangleList)

#include <vector>

#include "tgaimage.h"
#include "Texture.h"
#include "Triangle.h"
#include "global.h"


class Model {
private:
    Texture* texture = nullptr;
    int verNum, faceNum;

public:
    std::vector<Triangle> TriangleList;

    Model(const char* filename);
    ~Model();
    int nverts() const; // 获取顶点数
    int nfaces() const; // 获取面数
};