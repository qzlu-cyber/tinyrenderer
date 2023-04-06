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
    } // �������㰴�� y ֵ��С��������

    // �������������Ƽ����Ҫ���Ƶ�ֱ�ߵ���ֹ������
    int totalHeight = t2.y - t0.y;
    int segmentHeight = t1.y - t0.y;

    // �ֱ������λ���
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

// �жϵ� P �Ƿ�λ�������� ABC ��
bool insideTriangle(Vec3f A, Vec3f B, Vec3f C, Vec3f P) {
    Vec3<float> AB = B - A, BC = C - B, CA = A - C, AP = P - A, BP = P - B, CP = P - C;
    // �� -1 �Ƚ�����߾���
    if ((AB ^ AP).z > -0.01 && (BC ^ BP).z > -0.01 && (CA ^ CP).z > -0.01)
        return true;
    else if ((AB ^ AP).z < -1 && (BC ^ BP).z < -0.01 && (CA ^ CP).z < -0.01)
        return true;
    return false;
}

// ������������������
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
            // �����ж�ÿ�������Ƿ�����������
            if (insideTriangle(t0, t1, t2, P)) {
                // �������������ֵ����� P ������ֵ
                Vec3f baryCoord = barycentric(pts, P);
                P.z = baryCoord.x * pts[0].z + baryCoord.y * pts[1].z + baryCoord.z * pts[2].z;
                uvP = uvs[0] * baryCoord.x + uvs[1] * baryCoord.y + uvs[2] * baryCoord.z; // ���������ֵ���� UV ֵ
                // ��ά���鵽һά�����ӳ�� => ���� (x, y) �� zBuffer �ж�Ӧ�����ֵΪ zBuffer[x + y * width]
                // TODO: ������Щ��촽�������кڵ�
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

    Vec3f light_dir(0, 0, -1); // �����Դ����

    float* zBuffer = new float[width * height]; // �洢ÿ�����ص����ֵ��ÿ���û��ƴ�����ʱ�������бȽϣ���������ƣ���������ر��ڵ������ٻ���
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
        std::vector<int> face = model->face(i); //��ȡģ�͵ĵ� i ����Ƭ
        Vec3f screenCoords[3]; // ������ i ����Ƭ�����������Ļ����
        Vec3f worldCoords[3]; // �洢�� i ����Ƭ�����������������
        for (int j = 0; j < 3; j++)
        {
            worldCoords[j] = model->vert(face[j]);
            screenCoords[j] = Vec3f((worldCoords[j].x + 1.) * width / 2., (worldCoords[j].y + 1.) * height / 2., 1.0f); // ת��Ϊ��Ļ����
        }
        Vec3f normal = (worldCoords[2] - worldCoords[0]) ^ (worldCoords[1] - worldCoords[0]); // ���������η���
        normal.normalize();
        float intensity = normal * light_dir; // ��������߽��е�˵õ���ǿ��С��δ���ǹ⴫����˥��
        if (intensity > 0) // ���߷����������η��ߵļнǴ��� 90�ȣ���cos�� < 0����intensity < 0���˿̹��������屳�棩���򲻻�����
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
