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

const Vector3f cameraPos(0, 0, 3);//������ڷŵ�λ��
const Vector3f lightDir(0, 0, -1);//ƽ�йⷽ��

/// <summary>
/// ���� (x0, x1) ֮�����е� x ���꣬Ȼ���ֵ�õ� y ����
/// </summary>
/// <param name="x0">��� x ����</param>
/// <param name="y0">��� y ����</param>
/// <param name="x1">�յ� x ����</param>
/// <param name="y1">�յ� y ����</param>
/// <param name="image">Ҫ���Ƶ� image</param>
/// <param name="color">��ɫ</param>
void lineX(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    for (int x = x0; x < x1 + 1; x++) {
        float k = static_cast<float>(x - x0) / (x1 - x0);
        int y = k * (y1 - y0) + y0;
        image.set(x, y, color);
    }
}

/// <summary>
/// ������ (x0, x1) ֮�����е� x ����󣬲�����ͨ����ֵ�õ����� y ���꣬�����������ԣ�
/// ����� (y0, y1) ֮�����е� y ���꣬Ȼ���ֵ�õ� x ����
/// </summary>
/// <param name="x0">��� x ����</param>
/// <param name="y0">��� y ����</param>
/// <param name="x1">�յ� x ����</param>
/// <param name="y1">�յ� y ����</param>
/// <param name="image">Ҫ���Ƶ� image</param>
/// <param name="color">��ɫ</param>
void lineY(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    for (int y = y0; y < y1 + 1; y++) {
        float k = static_cast<float>(y - y0) / (y1 - y0);
        int x = k * (x1 - x0) + x0;
        image.set(x, y, color);
    }
}

void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    if (std::abs(x1 - x0) > std::abs(y1 - y0)) // ��ʱ��ͨ���������е� x �õ� y
    {
        if (x1 > x0) // �������
            lineX(x0, y0, x1, y1, image, color);
        else // �������
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
/// bresenham �㷨����ֱ�� ԭ��https://www.bilibili.com/video/BV1364y1d7Lo
/// </summary>
void bresenhamLine(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    bool flag = 0; // ����ܷ�ͨ������ y �õ����� x
    if (std::abs(x1 - x0) < std::abs(y1 - y0)) // ��ʱ����ͨ������ x �õ����� y
    {
        std::swap(x0, y0); // ������ʼ�����
        std::swap(x1, y1);
        flag = 1;
    }

    if (x0 > x1) // �Ƿ�Ϊ�������
    {
        std::swap(x0, x1); // ����Ǿͽ���
        std::swap(y0, y1);
    }

    int dx = x1 - x0, dy = std::abs(y1 - y0);
    int increaseEast = 2 * dy, increaseNorthEast = 2 * (dy - dx);
    int d = 2 * dy - dx;

    int x = x0, y = y0;

    // ���Ƶ�һ�����ص�
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
    bool flag = 0; // ����ܷ�ͨ������ y �õ����� x
    if (std::abs(x1 - x0) < std::abs(y1 - y0)) // ��ʱ����ͨ������ x �õ����� y
    {
        std::swap(x0, y0); // ������ʼ�����
        std::swap(x1, y1);
        flag = 1;
    }

    if (x0 > x1) // �Ƿ�Ϊ�������
    {
        std::swap(x0, x1); // ����Ǿͽ���
        std::swap(y0, y1);
    }

    int dx = x1 - x0, dy = std::abs(y1 - y0);
    int derror = 2 * dy; // ��ֱ��б��
    int error = 0; // �������

    for (int x = x0, y = y0; x <= x1; x++) {
        if (flag) {
            image.set(y, x, color);
        }
        else {
            image.set(x, y, color);
        }
        error += derror; // �ڶ������ص��ж� 2dy - dx > 0 �Ƿ�������������������������Ҫ +1��
        // �����ȷʵ�� > 0�������������Ҫ�ж� dx * slope > 1.5���� 4dy - 3dx > 0 => 2dy - dx + 2dy - 2dx > 0 => 4dy - 2dx > 0 �Ƿ���� 
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
    } // �������㰴�� y ֵ��С��������

    // �������������Ƽ����Ҫ���Ƶ�ֱ�ߵ���ֹ������
    int totalHeight = t2.y() - t0.y();
    int segmentHeight = t1.y() - t0.y();

    // �ֱ������λ���
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

// ģ�ͱ任����
Matrix4f modelMatrix()
{
    return Matrix4f::Identity();
}

// ��ͼ�任����
Matrix4f viewMatrix()
{
    return Matrix4f::Identity();
}

// ͸��ͶӰ�任����ԭ���߰汾��������������Ĺ�ʽ��
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

    // ���� TGA ͼ��
    TGAImage image(width, height, TGAImage::Format::RGB);

    // �洢������Ҫ���Ƶ���������Ƭ

    // ������դ������
    rst::rasterizer r(width, height);

    // ������������
    Texture tex("res/spot/hmap.tga");
    r.setTexture(tex);

    // ���֡����� zBuffer
    r.clear(rst::Buffers::Color);
    r.clear(rst::Buffers::Depth);

    // ���� MVP ����
    r.setModel(modelMatrix());
    r.setView(viewMatrix());
    r.setProjection(projectionMatrix());

    // ���ö�����ɫ����ƬԪ��ɫ��
    r.setVertexShader(vertexShader);
    r.setFragmentShader(normalFragmentShader);

    // ����ģ��
    r.draw(model->TriangleList);

    // ��֡�����е���ɫֵд�� image ��
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
