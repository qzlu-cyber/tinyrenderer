#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"

// ģ����
class Model {
private:
	std::vector<Vec3f> verts_; // ���㼯��ÿ�����㶼����ά����
	std::vector<std::vector<int> > faces_; // ��Ƭ��
public:
	Model(const char* filename); // ����.obj�ļ�·������ģ��
	~Model();
	int nverts(); // ����ģ�Ͷ�������
	int nfaces(); // ����ģ����Ƭ����
	Vec3f vert(int i); // ���ص�i������
	std::vector<int> face(int idx); // ���ص�idx����Ƭ
};

#endif //__MODEL_H__