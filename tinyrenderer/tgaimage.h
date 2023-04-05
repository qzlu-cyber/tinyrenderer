#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <fstream>

#pragma pack(push,1) // ��Ĭ�ϵı������뷽ʽ�����ջ�������µĶ���ϵ��Ϊ1
// TGA ͷ�ļ��ṹ
struct TGA_Header {
	char idlength; // ���ֶ���1�ֽ��޷������ͣ�ָ��ͼ����Ϣ�ֶγ��ȣ���ȡֵ��Χ��0��255������Ϊ0ʱ��ʾû��ͼ�����Ϣ�ֶΡ�
	char colormaptype; // ��ɫ�����ͣ�1�ֽڣ�:0 ��ʾû����ɫ��1 ��ʾ��ɫ����ڡ����ڱ���ʽ������ɫ��ģ���˴���ͨ�������ԡ�
	char datatypecode; // ͼ��������: 2: ��ѹ��RGB��ʽ 10: ѹ��RGB��ʽ
	short colormaporigin; // ��ɫ����ַ: ��ɫ���׵�������������ͣ���λ-��λ��
	short colormaplength; // ��ɫ����: ��ɫ��ı������������ͣ���λ-��λ��
	char colormapdepth; // ��ɫ����λ��: λ����bit����16����16λTGA��24����24λTGA��32����32λTGA
	short x_origin; // ͼ��X�������ʼλ��:ͼ�����½�X��������ͣ���λ-��λ��ֵ
	short y_origin; // ͼ��Y�������ʼλ��:ͼ�����½�Y��������ͣ���λ-��λ��ֵ
	short width; // ͼ����:������Ϊ��λ��ͼ���ȵ����ͣ���λ-��λ��
	short height; // ͼ��߶�:������Ϊ��λ��ͼ��߶ȵ����ͣ���λ-��λ��
	char  bitsperpixel; // ͼ��ÿ���ش洢ռ��λ��(bpp):����ֵΪ16��24�� 32�ȵȡ������˸�ͼ���� TGA16��TGA24, TGA32�ȵȡ�
	char  imagedescriptor; // ͼ���������ֽ�
};
#pragma pack(pop) // �ָ�Ĭ�ϵı������뷽ʽ
// �����ṹ��Ĵ�С��18�ֽ�

// TGAColor��
struct TGAColor {
	// union��СΪ4�ֽ�
	union {
		struct {
			unsigned char b, g, r, a;
		};// RGBAֵ
		unsigned char raw[4];
		unsigned int val;
	};
	int bytespp;// ͼ��ÿ���ش洢ռ���ֽ���

	TGAColor() : val(0), bytespp(1) {}

	TGAColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A) : b(B), g(G), r(R), a(A), bytespp(4) {}

	TGAColor(int v, int bpp) : val(v), bytespp(bpp) {}

	TGAColor(const TGAColor& c) : val(c.val), bytespp(c.bytespp) {} // �������캯��

	TGAColor(const unsigned char* p, int bpp) : val(0), bytespp(bpp) {
		for (int i = 0; i < bpp; i++) {
			raw[i] = p[i];
		}
	}

	TGAColor& operator =(const TGAColor& c) {
		if (this != &c) {
			bytespp = c.bytespp;
			val = c.val;
		}
		return *this;
	}// ��ֵ����
};


class TGAImage {
protected:
	unsigned char* data;
	int width;// ͼ��Ŀ��
	int height;// ͼ��ĸ߶�
	int bytespp;// ͼ���ÿ�����ֽ���

	bool load_rle_data(std::ifstream& in);
	bool unload_rle_data(std::ofstream& out);
public:
	enum Format {
		GRAYSCALE = 1, RGB = 3, RGBA = 4
	};// ͼ���ʽ���Ҷ�ͼ��RGBͼ��RGBAͼ

	TGAImage();
	TGAImage(int w, int h, int bpp);
	TGAImage(const TGAImage& img);
	bool read_tga_file(const char* filename);
	bool write_tga_file(const char* filename, bool rle = true);
	bool flip_horizontally();
	bool flip_vertically();
	bool scale(int w, int h);
	TGAColor get(int x, int y);// ��ȡ��x,y��λ�ô����ص���ɫֵ
	bool set(int x, int y, TGAColor c);// ����x,y��λ�ô�����������Ϊ��ɫ c
	~TGAImage();
	TGAImage& operator =(const TGAImage& img);
	int get_width();// ��ȡͼ����
	int get_height();// ��ȡͼ��߶�
	int get_bytespp();// ��ȡͼ��ÿ�����ֽ���
	unsigned char* buffer();
	void clear();
};

#endif // __IMAGE_H__