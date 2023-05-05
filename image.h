/****************************************************************************
 **                                                                        **
 **  BMP images must be 24-bit and each dimension must be a multiple of 4  **
 **                                                                        **
 ****************************************************************************/

void rgb_image_to_grayscale(unsigned char* buffer, unsigned int width, unsigned int height);
unsigned char* read_bmp_image(const char* filename, unsigned int* width, unsigned int* height);