#pragma once

#include <iostream>

#include "tgaimage.h"

class Texture
{
private:
    TGAImage imageData;
public:
    Texture(const char* filename)
    {
        imageData.read_tga_file(filename);
        imageData.flip_vertically();

        width = imageData.get_width();
        height = imageData.get_height();
    }

    int width, height;

    TGAColor getColor(float u, float v)
    {
        return imageData.get(u, v);
    }
};

