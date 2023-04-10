#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <fstream>

#pragma pack(push,1) // 将默认的变量对齐方式推入堆栈，设置新的对齐系数为1
// TGA 头文件结构
struct TGA_Header {
    char idlength; // 本字段是1字节无符号整型，指出图像信息字段长度，其取值范围是0到255，当它为0时表示没有图像的信息字段。
    char colormaptype; // 颜色表类型（1字节）:0 表示没有颜色表，1 表示颜色表存在。由于本格式是无颜色表的，因此此项通常被忽略。
    char datatypecode; // 图像类型码: 2: 非压缩RGB格式 10: 压缩RGB格式
    short colormaporigin; // 颜色表首址: 颜色表首的入口索引，整型（低位-高位）
    short colormaplength; // 颜色表长度: 颜色表的表项总数，整型（低位-高位）
    char colormapdepth; // 颜色表项位数: 位数（bit），16代表16位TGA，24代表24位TGA，32代表32位TGA
    short x_origin; // 图像X坐标的起始位置:图像左下角X坐标的整型（低位-高位）值
    short y_origin; // 图像Y坐标的起始位置:图像左下角Y坐标的整型（低位-高位）值
    short width; // 图像宽度:以像素为单位，图像宽度的整型（低位-高位）
    short height; // 图像高度:以像素为单位，图像高度的整型（低位-高位）
    char  bitsperpixel; // 图像每像素存储占用位数(bpp):它的值为16，24或 32等等。决定了该图像是 TGA16，TGA24, TGA32等等。
    char  imagedescriptor; // 图像描述符字节
};
#pragma pack(pop) // 恢复默认的变量对齐方式
// 上述结构体的大小是18字节

// TGAColor 类
struct TGAColor {
    unsigned char bgra[4];
    unsigned char bytespp;

    TGAColor() : bgra(), bytespp(1) {
        for (int i = 0; i < 4; i++) bgra[i] = 0;
    }

    TGAColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A = 255) : bgra(), bytespp(4) {
        bgra[0] = B;
        bgra[1] = G;
        bgra[2] = R;
        bgra[3] = A;
    }

    TGAColor(unsigned char v) : bgra(), bytespp(1) {
        for (int i = 0; i < 4; i++) bgra[i] = 0;
        bgra[0] = v;
    }


    TGAColor(const unsigned char* p, unsigned char bpp) : bgra(), bytespp(bpp) {
        for (int i = 0; i < (int)bpp; i++) {
            bgra[i] = p[i];
        }
        for (int i = bpp; i < 4; i++) {
            bgra[i] = 0;
        }
    }

    TGAColor operator *(float intensity) const {
        TGAColor res = *this;
        intensity = (intensity > 1.f ? 1.f : (intensity < 0.f ? 0.f : intensity));
        for (int i = 0; i < 4; i++) res.bgra[i] = bgra[i] * intensity;
        return res;
    }
};


class TGAImage {
protected:
    unsigned char* data;
    int width;// 图像的宽度
    int height;// 图像的高度
    int bytespp;// 图像的每像素字节数

    bool load_rle_data(std::ifstream& in);
    bool unload_rle_data(std::ofstream& out);
public:
    enum Format {
        GRAYSCALE = 1, RGB = 3, RGBA = 4
    };// 图像格式：灰度图，RGB 图，RGBA 图

    TGAImage();
    TGAImage(int w, int h, int bpp);
    TGAImage(const TGAImage& img);
    bool read_tga_file(const char* filename);
    bool write_tga_file(const char* filename, bool rle = true);
    bool flip_horizontally();
    bool flip_vertically();
    bool scale(int w, int h);
    TGAColor get(int x, int y);// 获取（x,y）位置处像素的颜色值
    bool set(int x, int y, TGAColor& c);// 将（x, y）位置处的像素设置为颜色 c
    bool set(int x, int y, const TGAColor& c);
    ~TGAImage();
    TGAImage& operator =(const TGAImage& img);
    int get_width();// 获取图像宽度
    int get_height();// 获取图像高度
    int get_bytespp();// 获取图像每像素字节数
    unsigned char* buffer();
    void clear();
};

#endif // __IMAGE_H__