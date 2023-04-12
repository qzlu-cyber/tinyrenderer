#pragma once

#include <iostream>
#include <vector>
#include <optional>
#include <limits>
#include <functional>

#include "Texture.h"
#include "model.h"
#include "Shader.h"

namespace rst
{
    enum class Buffers
    {
        Color = 1,
        Depth = 2
    };

    enum class Primitive
    {
        Line,
        Triangle
    };

    class rasterizer
    {
    private:
        Matrix4f modelMatrix;
        Matrix4f viewMatrix;
        Matrix4f projectionMatrix;

        std::vector<float> depthBuffer;

        int width, height;

        std::optional<Texture> texture;

        std::function<Vector3f(fragmentShaderPayload)> fragmentShader;
        std::function<Vector3f(vertexShaderPayload)> vertexShader;

        void drawLine();
        void rasterizerTriangle(const Triangle& t);

    public:
        rasterizer(int w, int h) :width(w), height(h)
        {
            frameBuffer.resize(w * h);
            depthBuffer.resize(w * h);

            texture = std::nullopt;
        }

        void setModel(const Matrix4f& m);
        void setView(const Matrix4f& v);
        void setProjection(const Matrix4f& p);

        void setTexture(Texture tex);

        void clear(Buffers buf);

        void setPixel(Vector2i& point, Vector3f& color);

        void setVertexShader(std::function<Vector3f(vertexShaderPayload)> vertShader);
        void setFragmentShader(std::function<Vector3f(fragmentShaderPayload)> fragShader);

        void draw(std::vector<Triangle>& TriangleList);

        std::vector<Vector3f> frameBuffer;
    };
}