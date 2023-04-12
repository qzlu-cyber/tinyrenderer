#include <cmath>

#include "rasterizer.h"

void rst::rasterizer::setModel(const Matrix4f& m)
{
    modelMatrix = m;
}

void rst::rasterizer::setView(const Matrix4f& v)
{
    viewMatrix = v;
}

void rst::rasterizer::setProjection(const Matrix4f& p)
{
    projectionMatrix = p;
}

void rst::rasterizer::setTexture(Texture tex)
{
    texture = tex;
}

void rst::rasterizer::clear(Buffers buf) {
    buf == Buffers::Color ? (std::fill(frameBuffer.begin(), frameBuffer.end(), Vector3f(0.0f, 0.0f, 0.0f))) :
        (std::fill(depthBuffer.begin(), depthBuffer.end(), -std::numeric_limits<float>::infinity()));
}

void rst::rasterizer::setPixel(Vector2i& point, Vector3f& color) {
    int idx = point.y() * width + point.x();
    frameBuffer[idx] = color;
}

void rst::rasterizer::setFragmentShader(std::function<Vector3f(fragmentShaderPayload)> fragShader)
{
    fragmentShader = fragShader;
}

void rst::rasterizer::setVertexShader(std::function<Vector3f(vertexShaderPayload)> vertShader)
{
    vertexShader = vertShader;
}

auto toVec4(const Vector3f& v3, float w = 1.0f)
{
    return Vector4f(v3.x(), v3.y(), v3.z(), w);
}

void rst::rasterizer::draw(std::vector<Triangle>& TriangleList)
{
    float f1 = (255 - 0.0) / 2.0;
    float f2 = (255 + 0.0) / 2.0;

    // 计算 MVP 矩阵
    Matrix4f mvp = projectionMatrix * viewMatrix * modelMatrix;

    // 遍历所有的三角形
    for (const auto& t : TriangleList)
    {
        int depth = 255; // 认为 n = 0.0f, f = 255.0f
        Triangle newtri = t;

        // 计算经过 MVP 变换后的顶点
        Vector4f v[3];
        for (int i = 0; i < 3; i++)
        {
            v[i] = t.v[i];
            //std::cout << v[i] << std::endl;
        }

        for (auto& vec : v)
        {
            //std::cout << "successful" << std::endl;
            Vector4f tmp;
            //std::cout << "successful" << std::endl;
            tmp = mvp * vec;
            vec.x() = tmp.x();
            vec.y() = tmp.y();
            vec.z() = tmp.z();
            vec.w() = tmp.w();
            // std::cout << vec << std::endl;
        }

        // 进行透视除法，归一化
        for (auto& vec : v)
        {
            vec.x() = vec.x() / vec.w();
            vec.y() = vec.y() / vec.w();
            vec.z() = vec.z() / vec.w();
            //std::cout << "./W" << vec << std::endl;
        }

        // 视口变换
        for (auto& vec : v)
        {
            float w = width * 3.f / 4.f;
            float h = height * 3.f / 4.f;
            float x = width / 8.f;
            float y = height / 8.f;

            vec.x() = w / 2.f * vec.x() + w / 2.f + x;
            vec.y() = h / 2.f * vec.y() + h / 2.f + y;
            vec.z() = vec.z() * f1 + f2;

            //std::cout << vec << std::endl;
        }

        //得到屏幕坐标
        for (int i = 0; i < 3; i++) {
            //屏幕顶点坐标
            newtri.v[i] = v[i];
            //std::cout << newtri.v[i] << std::endl;
        }

        //newtri.computeFcolor(lightDir);
        newtri.computeGColor(lightDir);

        //进行屏幕三角形光栅化
        rasterizerTriangle(newtri);
    }
}

// 判断像素点是否在三角形内
static bool insideTriangle(int x, int y, const Vector4f* _v)
{
    Vector3f v[3];
    for (int i = 0; i < 3; i++)
        v[i] = { _v[i].x(), _v[i].y(), 1.0 };
    Vector3f f0, f1, f2;
    f0 = v[1].cross(v[0]);
    f1 = v[2].cross(v[1]);
    f2 = v[0].cross(v[2]);
    Vector3f p(x, y, 1.);
    if ((p.dot(f0) * f0.dot(v[2]) > 0) && (p.dot(f1) * f1.dot(v[0]) > 0) && (p.dot(f2) * f2.dot(v[1]) > 0))
        return true;
    return false;
}

// 计算重心坐标
static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector4f* v)
{
    float c1 = (x * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * y + v[1].x() * v[2].y() - v[2].x() * v[1].y()) / (v[0].x() * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * v[0].y() + v[1].x() * v[2].y() - v[2].x() * v[1].y());
    float c2 = (x * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * y + v[2].x() * v[0].y() - v[0].x() * v[2].y()) / (v[1].x() * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * v[1].y() + v[2].x() * v[0].y() - v[0].x() * v[2].y());
    float c3 = (x * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * y + v[0].x() * v[1].y() - v[1].x() * v[0].y()) / (v[2].x() * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * v[2].y() + v[0].x() * v[1].y() - v[1].x() * v[0].y());
    return { c1, c2, c3 };
}

void rst::rasterizer::rasterizerTriangle(const Triangle& t) {
    const Vector4f* vertex = t.v; // 首先获取三角形 t 的三个顶点

    float minX = std::min({ vertex[0].x(), vertex[1].x(), vertex[2].x() });
    float maxX = std::max({ vertex[0].x(), vertex[1].x(), vertex[2].x() });
    float minY = std::min({ vertex[0].y(), vertex[1].y(), vertex[2].y() });
    float maxY = std::max({ vertex[0].y(), vertex[1].y(), vertex[2].y() });

    int min_X = std::floor(minX);
    int max_X = std::ceil(maxX);
    int min_Y = std::floor(minY);
    int max_Y = std::ceil(maxY);

    for (int x = min_X; x < max_X + 1; x++)
    {
        for (int y = min_Y; y < max_Y + 1; y++) {
            Vector2i point(x, y); // 待渲染像素点

            if (insideTriangle(x, y, vertex))
            {
                auto [alpha, beta, gamma] = computeBarycentric2D(x, y, vertex);

                float zInterpolation = alpha * vertex[0].z() + beta * vertex[1].z() + gamma * vertex[2].z();

                if (zInterpolation > depthBuffer[y * width + x])
                {
                    depthBuffer[y * width + x] = zInterpolation;

                    // 插值计算三角形内的各项属性值
                    Vector2f uvInterpolation = t.texCoords[0] * alpha + t.texCoords[1] * beta + t.texCoords[2] * gamma;
                    Vector3f normalInterpolation = t.normal[0] * alpha + t.normal[1] * beta + t.normal[2] * gamma;
                    Vector3f colorInterpolation = t.color[0] * alpha + t.color[1] * beta + t.color[2] * gamma;

                    // 将插值得到的信息传给片元着色器，在片元着色器中经过一些计算得到该片元的颜色
                    fragmentShaderPayload payload(colorInterpolation, normalInterpolation.normalized(), uvInterpolation, texture ? &*texture : nullptr);

                    auto pixelColor = fragmentShader(payload);
                    setPixel(point, pixelColor);
                }
            }
        }
    }
}