#pragma once

#include "geometry.h"
#include "Texture.h"


struct fragmentShaderPayload
{
    fragmentShaderPayload()
    {
        texture = nullptr;
    }

    fragmentShaderPayload(const Vector3f& color, const Vector3f& normal, const Vector2f& texCoord, Texture* tex)
        : color(color), normal(normal), texCoords(texCoord), texture(tex) {}


    Vector3f viewPos;
    Vector3f color;
    Vector3f normal;
    Vector2f texCoords;
    Texture* texture;
};

struct vertexShaderPayload
{
    Vector3f position;
};

