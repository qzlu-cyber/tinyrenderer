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
