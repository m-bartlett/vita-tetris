#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "image.h"

/****************************************************************************
 **                                                                        **
 **  BMP images must be 24-bit and each dimension must be a multiple of 4  **
 **                                                                        **
 ****************************************************************************/


unsigned char* read_rgb_bmp_image(const char* filename,
                                  unsigned int* width,
                                  unsigned int* height)
{
    FILE* f = fopen(filename, "rb");
    unsigned char info[54];

    // read the 54-byte header
    fread(info, sizeof(unsigned char), 54, f); 

    // extract image height and width from header
    unsigned int _width = *(unsigned int*)&info[18];
    unsigned int _height = *(unsigned int*)&info[22];
    *width = _width;
    *height = _height;

    // allocate 3 bytes per pixel
    unsigned int size = 3 * _width * _height;
    unsigned char* buffer =  (unsigned char*)malloc(size);

    // read the rest of the data at once
    fread(buffer, sizeof(unsigned char), size, f); 
    fclose(f);

    for(unsigned int i = 0; i < size; i += 3) {
        // flip the order of every 3 bytes
        unsigned char tmp = buffer[i];
        buffer[i] = buffer[i+2];
        buffer[i+2] = tmp;
    }

    return buffer;
}


unsigned char* read_rgb_bmp_image_as_grayscale(const char* filename,
                                               unsigned int* width,
                                               unsigned int* height)
{
    unsigned char* buffer = read_rgb_bmp_image(filename, width, height);
    const unsigned int row_size = (*width) * 3;
    const unsigned int pixel_count = row_size * (*height);
    unsigned int i = 0;
    for (unsigned int y = 0; y < pixel_count; y+=row_size) {
       for (unsigned int x = 0; x < row_size; x+=3) {
          unsigned int _x = y+x;
          unsigned int r = buffer[_x], g = buffer[_x+1], b = buffer[_x+2];
          unsigned char magnitude = sqrt((r*r) + (g*g) + (b*b)) * 255 / sqrt(255*255*3);
          buffer[i++] = magnitude;
       }
    }
    return buffer;
}