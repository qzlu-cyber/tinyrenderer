#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>

#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 0, 255, 255);
const int width = 800;
const int height = 800;

Model* model = NULL;

/// <summary>
/// 遍历 (x0, x1) 之间所有的 x 坐标，然后插值得到 y 坐标
/// </summary>
/// <param name="x0">起点 x 坐标</param>
/// <param name="y0">起点 y 坐标</param>
/// <param name="x1">终点 x 坐标</param>
/// <param name="y1">终点 y 坐标</param>
/// <param name="image">要绘制的 image</param>
/// <param name="color">颜色</param>
void lineX(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    for (int x = x0; x < x1 + 1; x++) {
        float k = static_cast<float>(x - x0) / (x1 - x0);
        int y = k * (y1 - y0) + y0;
        image.set(x, y, color);
    }
}

/// <summary>
/// 当遍历 (x0, x1) 之间所有的 x 坐标后，不足以通过插值得到所有 y 坐标，但反过来可以，
/// 则遍历 (y0, y1) 之间所有的 y 坐标，然后插值得到 x 坐标
/// </summary>
/// <param name="x0">起点 x 坐标</param>
/// <param name="y0">起点 y 坐标</param>
/// <param name="x1">终点 x 坐标</param>
/// <param name="y1">终点 y 坐标</param>
/// <param name="image">要绘制的 image</param>
/// <param name="color">颜色</param>
void lineY(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    for (int y = y0; y < y1 + 1; y++) {
        float k = static_cast<float>(y - y0) / (y1 - y0);
        int x = k * (x1 - x0) + x0;
        image.set(x, y, color);
    }
}

void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    if (std::abs(x1 - x0) > std::abs(y1 - y0)) // 此时能通过遍历所有的 x 得到 y
    {
        if (x1 > x0) // 正向绘制
            lineX(x0, y0, x1, y1, image, color);
        else // 反向绘制
            lineX(x1, y1, x0, y0, image, color);
    }
    else
    {
        if (y1 > y0)
            lineY(x0, y0, x1, y1, image, color);
        else
            lineY(x1, y1, x0, y0, image, color);
    }
}

/// <summary>
/// bresenham 算法绘制直线 原理：https://www.bilibili.com/video/BV1364y1d7Lo
/// </summary>
void bresenhamLine(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    bool flag = 0; // 标记能否通过遍历 y 得到所有 x
    if (std::abs(x1 - x0) < std::abs(y1 - y0)) // 此时不能通过遍历 x 得到所有 y
    {
        std::swap(x0, y0); // 交换起始坐标点
        std::swap(x1, y1);
        flag = 1;
    }

    if (x0 > x1) // 是否为反向绘制
    {
        std::swap(x0, x1); // 如果是就交换
        std::swap(y0, y1);
    }

    int dx = x1 - x0, dy = std::abs(y1 - y0);
    int increaseEast = 2 * dy, increaseNorthEast = 2 * (dy - dx);
    int d = 2 * dy - dx;

    int x = x0, y = y0;

    // 绘制第一个像素点
    if (flag) {
        image.set(y, x, color);
    }
    else {
        image.set(x, y, color);
    }

    for (int x = x0 + 1; x < x1 + 1; x++) {
        if (d < 0.5) {
            d += increaseEast;
        }
        else
        {
            d += increaseNorthEast;
            if (y1 > y0)
                y++;
            else
                y--;
        }

        if (flag) {
            image.set(y, x, color);
        }
        else {
            image.set(x, y, color);
        }
    }

}

void bresenhamLine2(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    bool flag = 0; // 标记能否通过遍历 y 得到所有 x
    if (std::abs(x1 - x0) < std::abs(y1 - y0)) // 此时不能通过遍历 x 得到所有 y
    {
        std::swap(x0, y0); // 交换起始坐标点
        std::swap(x1, y1);
        flag = 1;
    }

    if (x0 > x1) // 是否为反向绘制
    {
        std::swap(x0, x1); // 如果是就交换
        std::swap(y0, y1);
    }

    int dx = x1 - x0, dy = std::abs(y1 - y0);
    int derror = 2 * dy; // 即直线斜率
    int error = 0; // 即误差项

    for (int x = x0, y = y0; x <= x1; x++) {
        if (flag) {
            image.set(y, x, color);
        }
        else {
            image.set(x, y, color);
        }
        error += derror; // 第二个像素点判断 2dy - dx > 0 是否成立，如果成立则它的纵坐标要 +1；
        // 如果它确实是 > 0，则第三个像素要判断 dx * slope > 1.5，即 4dy - 3dx > 0 => 2dy - dx + 2dy - 2dx > 0 => 4dy - 2dx > 0 是否成立 
        if (error > dx) {
            y += (y1 > y0 ? 1 : -1);
            error -= dx * 2;
        }
    }
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color) {
    if (t0.y > t1.y)
    {
        std::swap(t0, t1);
    }
    if (t0.y > t2.y)
    {
        std::swap(t0, t2);
    }
    if (t1.y > t2.y)
    {
        std::swap(t1, t2);
    } // 三个顶点按照 y 值从小到大排序

    // 根据三角形相似计算出要绘制的直线的起止点坐标
    int totalHeight = t2.y - t0.y;
    int segmentHeight = t1.y - t0.y;

    // 分别在两段绘制
    for (int y = t0.y; y < t1.y + 1; y++)
    {
        float alpha = static_cast<float>(y - t0.y) / totalHeight;
        float beta = static_cast<float>(y - t0.y) / segmentHeight;
        Vec2i A = t0 + (t2 - t0) * alpha;
        Vec2i B = t0 + (t1 - t0) * beta;

        if (A.x > B.x)
            std::swap(A, B);
        for (int x = A.x; x < B.x + 1; x++) {
            image.set(x, y, color);
        }
    }

    for (int y = t1.y; y < t2.y + 1; y++) {
        segmentHeight = t2.y - t1.y;
        float alpha = static_cast<float>(y - t0.y) / totalHeight;
        float beta = static_cast<float>(y - t1.y) / segmentHeight;
        Vec2i A = t0 + (t2 - t0) * alpha;
        Vec2i B = t1 + (t2 - t1) * beta;

        if (A.x > B.x)
            std::swap(A, B);
        for (int x = A.x; x < B.x + 1; x++) {
            image.set(x, y, color);
        }
    }
}

// 判断点 P 是否位于三角形 ABC 内
bool insideTriangle(Vec3f A, Vec3f B, Vec3f C, Vec3f P) {
    Vec3<float> AB = B - A, BC = C - B, CA = A - C, AP = P - A, BP = P - B, CP = P - C;
    // 和 -1 比较以提高精度
    if ((AB ^ AP).z > -0.01 && (BC ^ BP).z > -0.01 && (CA ^ CP).z > -0.01)
        return true;
    else if ((AB ^ AP).z < -1 && (BC ^ BP).z < -0.01 && (CA ^ CP).z < -0.01)
        return true;
    return false;
}

// 计算三角形重心坐标
Vec3f barycentric(Vec3f* pts, Vec3f P) {
    float xa = pts[0].x;
    float ya = pts[0].y;
    float xb = pts[1].x;
    float yb = pts[1].y;
    float xc = pts[2].x;
    float yc = pts[2].y;
    float x = P.x;
    float y = P.y;

    float gamma = ((ya - yb) * x + (xb - xa) * y + xa * yb - xb * ya) / ((ya - yb) * xc + (xb - xa) * yc + xa * yb - xb * ya);
    float beta = ((ya - yc) * x + (xc - xa) * y + xa * yc - xc * ya) / ((ya - yc) * xb + (xc - xa) * yb + xa * yc - xc * ya);
    float alpha = 1 - gamma - beta;

    return Vec3f(alpha, beta, gamma);
}

void triangle2(Vec3f t0, Vec3f t1, Vec3f t2, Vec2i* uvs, float* zBuffer, TGAImage& image, float intensity) {
    float minX = std::min({ t0.x, t1.x, t2.x });
    float maxX = std::max({ t0.x, t1.x, t2.x });
    float minY = std::min({ t0.y, t1.y, t2.y });
    float maxY = std::max({ t0.y, t1.y, t2.y });

    int min_X = std::floor(minX);
    int max_X = std::ceil(maxX);
    int min_Y = std::floor(minY);
    int max_Y = std::ceil(maxY);

    Vec3f pts[3] = { t0, t1, t2 };

    for (int x = min_X; x < max_X + 1; x++) {
        for (int y = min_Y; y < max_Y + 1; y++) {
            Vec3f P(x, y, 1.0f);
            Vec2i uvP;
            // 依次判断每个像素是否在三角形内
            if (insideTriangle(t0, t1, t2, P)) {
                // 根据重心坐标插值计算出 P 点的深度值
                Vec3f baryCoord = barycentric(pts, P);
                P.z = baryCoord.x * pts[0].z + baryCoord.y * pts[1].z + baryCoord.z * pts[2].z;
                uvP = uvs[0] * baryCoord.x + uvs[1] * baryCoord.y + uvs[2] * baryCoord.z; // 重心坐标插值计算 UV 值
                // 二维数组到一维数组的映射 => 像素 (x, y) 在 zBuffer 中对应的深度值为 zBuffer[x + y * width]
                // TODO: 精度有些差，嘴唇附近会有黑点
                if (P.z > zBuffer[x + y * width]) {
                    zBuffer[x + y * width] = P.z;
                    TGAColor color = model->diffuse(uvP);
                    image.set(x, y, TGAColor(color.r * intensity, color.g * intensity, color.b * intensity, 255));
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    TGAImage image(width, height, TGAImage::RGB);

    Vec3f light_dir(0, 0, -1); // 定向光源方向

    float* zBuffer = new float[width * height]; // 存储每个像素的深度值，每当该绘制此像素时和它进行比较，大于则绘制，否则该像素被遮挡不必再绘制
    for (int i = 0; i < width * height; i++)
    {
        zBuffer[i] = -std::numeric_limits<float>::max();
    }

    if (2 == argc) {
        model = new Model(argv[1]);
    }
    else {
        model = new Model("./res/african_head.obj");
    }

    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i); //获取模型的第 i 个面片
        Vec3f screenCoords[3]; // 存贮第 i 个面片三个顶点的屏幕坐标
        Vec3f worldCoords[3]; // 存储第 i 个面片三个顶点的世界坐标
        for (int j = 0; j < 3; j++)
        {
            worldCoords[j] = model->vert(face[j]);
            screenCoords[j] = Vec3f((worldCoords[j].x + 1.) * width / 2., (worldCoords[j].y + 1.) * height / 2., 1.0f); // 转换为屏幕坐标
        }
        Vec3f normal = (worldCoords[2] - worldCoords[0]) ^ (worldCoords[1] - worldCoords[0]); // 计算三角形法线
        normal.normalize();
        float intensity = normal * light_dir; // 法线与光线进行点乘得到光强大小，未考虑光传播的衰减
        if (intensity > 0) // 光线方向与三角形法线的夹角大于 90度（即cosθ < 0，即intensity < 0，此刻光线在物体背面），则不绘制它
        {
            Vec2i uv[3];
            for (int j = 0; j < 3; j++) uv[j] = model->uv(i, j);
            triangle2(screenCoords[0], screenCoords[1], screenCoords[2], uv, zBuffer, image, intensity);
        }
    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");

    delete model;
    delete[] zBuffer;

    return 0;
}
