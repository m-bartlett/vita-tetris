/****************************************************************************
 **                                                                        **
 **  BMP images must be 24-bit and each dimension must be a multiple of 4  **
 **                                                                        **
 ****************************************************************************/

unsigned char* read_rgb_bmp_image(const char* filename,
                                  unsigned int* width,
                                  unsigned int* height);

unsigned char* read_rgb_bmp_image_as_grayscale(const char* filename,
                                               unsigned int* width,
                                               unsigned int* height);