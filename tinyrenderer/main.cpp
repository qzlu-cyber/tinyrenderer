#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>

#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "rasterizer.h"

const int width = 800;
const int height = 800;

Model* model = NULL;

const Vector3f cameraPos(0, 0, 3);//摄像机摆放的位置
const Vector3f lightDir(0, 0, -1);//平行光方向

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

void triangle(Vector2i t0, Vector2i t1, Vector2i t2, TGAImage& image, TGAColor color) {
    if (t0.y() > t1.y())
    {
        std::swap(t0, t1);
    }
    if (t0.y() > t2.y())
    {
        std::swap(t0, t2);
    }
    if (t1.y() > t2.y())
    {
        std::swap(t1, t2);
    } // 三个顶点按照 y 值从小到大排序

    // 根据三角形相似计算出要绘制的直线的起止点坐标
    int totalHeight = t2.y() - t0.y();
    int segmentHeight = t1.y() - t0.y();

    // 分别在两段绘制
    for (int y = t0.y(); y < t1.y() + 1; y++)
    {
        float alpha = static_cast<float>(y - t0.y()) / totalHeight;
        float beta = static_cast<float>(y - t0.y()) / segmentHeight;
        /*Vector2i A = t0 + (t2 - t0) * (alpha);
        Vector2i B = t0 + (t1 - t0) * beta;

        if (A.x() > B.x())
            std::swap(A, B);
        for (int x = A.x(); x < B.x() + 1; x++) {
            image.set(x, y, color);
        }*/
    }

    for (int y = t1.y(); y < t2.y() + 1; y++) {
        segmentHeight = t2.y() - t1.y();
        float alpha = static_cast<float>(y - t0.y()) / totalHeight;
        float beta = static_cast<float>(y - t1.y()) / segmentHeight;
        /*Vector2i A = t0 + (t2 - t0) * alpha;
        Vector2i B = t1 + (t2 - t1) * beta;

        if (A.x() > B.x())
            std::swap(A, B);
        for (int x = A.x(); x < B.x() + 1; x++) {
            image.set(x, y, color);
        }*/
    }
}

// 模型变换矩阵
Matrix4f modelMatrix()
{
    return Matrix4f::Identity();
}

// 视图变换矩阵
Matrix4f viewMatrix()
{
    return Matrix4f::Identity();
}

// 透视投影变换矩阵（原作者版本，对照我们上面的公式）
Matrix4f projectionMatrix()
{
    Matrix4f projection = Matrix4f::Identity();
    projection(3, 2) = -1.0f / cameraPos.z();
    return projection;
}

Vector3f vertexShader(const vertexShaderPayload& payload)
{
    return payload.position;
}

Vector3f normalFragmentShader(const fragmentShaderPayload& payload)
{
    Eigen::Vector3f returnColor = (payload.normal.head<3>().normalized() + Eigen::Vector3f(1.0f, 1.0f, 1.0f)) / 2.f;
    Eigen::Vector3f result;
    result << returnColor.x() * 255, returnColor.y() * 255, returnColor.z() * 255;
    return result;
}

int main(int argc, char** argv) {
    if (2 == argc) {
        model = new Model(argv[1]);
    }
    else {
        model = new Model("res/spot/spot_triangulated_good.obj");
    }

    std::cout << model->nfaces() << "  " << model->nverts() << std::endl;

    // 创建 TGA 图像
    TGAImage image(width, height, TGAImage::Format::RGB);

    // 存储所有需要绘制的三角形面片

    // 创建光栅化对象
    rst::rasterizer r(width, height);

    // 给定纹理并设置
    Texture tex("res/spot/hmap.tga");
    r.setTexture(tex);

    // 清空帧缓冲和 zBuffer
    r.clear(rst::Buffers::Color);
    r.clear(rst::Buffers::Depth);

    // 设置 MVP 矩阵
    r.setModel(modelMatrix());
    r.setView(viewMatrix());
    r.setProjection(projectionMatrix());

    // 设置顶点着色器和片元着色器
    r.setVertexShader(vertexShader);
    r.setFragmentShader(normalFragmentShader);

    // 绘制模型
    r.draw(model->TriangleList);

    // 将帧缓冲中的颜色值写入 image 中
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            Vector3f color = r.frameBuffer[j * width + i];
            image.set(i, j, TGAColor(color.x(), color.y(), color.z(), 255));
        }
    }

    image.flip_vertically();
    image.write_tga_file("output.tga");

    return 0;
}
