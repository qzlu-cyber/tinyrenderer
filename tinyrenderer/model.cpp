#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include "model.h"
#include "Triangle.h"

Model::Model(const char* filename)
    : verNum(0), faceNum(0)
{
    std::vector<Vector4f> vertexs; // 保存所有的顶点
    std::vector<std::vector<Vector3i>> faces; // 保存面
    std::vector<Vector3f> normals; // 保存所有的法线
    std::vector<Vector2f> uvs; // 保存 uv 坐标

    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail()) return;

    std::string line;

    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vector4f v;
            for (int i = 0; i < 3; i++) iss >> v[i];
            v[3] = 1.0f;
            vertexs.push_back(v);
        }
        else if (!line.compare(0, 3, "vt ")) {
            iss >> trash >> trash;
            Vector2f uv;
            for (int i = 0; i < 2; i++) iss >> uv[i];
            uvs.push_back(uv);
        }
        else if (!line.compare(0, 3, "vn ")) {
            iss >> trash >> trash;
            Vector3f normal;
            for (int i = 0; i < 3; i++) iss >> normal[i];
            normals.push_back(normal);
        }
        else if (!line.compare(0, 2, "f ")) {
            std::vector<Vector3i> f;
            Vector3i tmp;
            iss >> trash;
            while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2]) {
                for (int i = 0; i < 3; i++) tmp[i]--;
                f.push_back(tmp);
            }
            faces.push_back(f);
        }
    }

    std::cerr << "# v# " << vertexs.size() << " f# " << faces.size() << " vt# " << uvs.size() << " vn# " << normals.size() << std::endl;

    TriangleList.resize(faces.size());
    TriangleList.clear();

    for (size_t i = 0; i < faces.size(); i++)
    {
        Triangle tmp;

        std::vector<Vector3i> face = faces[i];
        Vector3i face_vertex_1 = face[0];
        Vector3i face_vertex_2 = face[1];
        Vector3i face_vertex_3 = face[2];

        tmp.v[0] = vertexs[face_vertex_1.x()];
        tmp.v[1] = vertexs[face_vertex_2.x()];
        tmp.v[2] = vertexs[face_vertex_3.x()];

        tmp.texCoords[0] = uvs[face_vertex_1.y()];
        tmp.texCoords[1] = uvs[face_vertex_2.y()];
        tmp.texCoords[2] = uvs[face_vertex_3.y()];

        tmp.normal[0] = normals[face_vertex_1.z()];
        tmp.normal[1] = normals[face_vertex_2.z()];
        tmp.normal[2] = normals[face_vertex_3.z()];


        TriangleList.push_back(tmp);
    }

    verNum = static_cast<int>(vertexs.size());
    faceNum = static_cast<int>(faces.size());
}

Model::~Model() {
}

int Model::nverts() {
    return verNum;
}

int Model::nfaces() {
    return faceNum;
}