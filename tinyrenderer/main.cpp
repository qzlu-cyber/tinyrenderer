#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const int width = 800;
const int height = 800;

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

int main(int argc, char** argv) {
	TGAImage image(width, height, TGAImage::RGB);
	Model* model = new Model(".\\obj\\african_head.obj");

	for (int i = 0; i < model->nfaces(); i++) {
		std::vector<int> face = model->face(i);
		for (int j = 0; j < 3; j++) {
			Vec3f v0 = model->vert(face[j]);
			Vec3f v1 = model->vert(face[(j + 1) % 3]);
			int x0 = (v0.x + 1.) * width / 2.;
			int y0 = (v0.y + 1.) * height / 2.;
			int x1 = (v1.x + 1.) * width / 2.;
			int y1 = (v1.y + 1.) * height / 2.;
			bresenhamLine2(x0, y0, x1, y1, image, white);
		}
	}

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}
