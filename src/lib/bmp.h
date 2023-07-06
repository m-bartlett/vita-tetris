/********************************************************************************
 **                                                                            **
 **  BMP images must be 24-bit RGB and each dimension must be a multiple of 4  **
 **                                                                            **
 ********************************************************************************/

#ifndef BPM_H
#define BPM_H value

unsigned char* read_rgb_bmp_image(const char* filename,
                                  unsigned int* width,
                                  unsigned int* height);

unsigned char* read_rgb_bmp_image_as_grayscale(const char* filename,
                                               unsigned int* width,
                                               unsigned int* height);

#endif