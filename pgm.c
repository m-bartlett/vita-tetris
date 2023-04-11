#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "pgm.h"

#define PGM_HEADER_MAGIC "P2"

static inline int pgm_validate_file_descriptor(FILE* f) {
    return getc(f) == PGM_HEADER_MAGIC[0] && getc(f) == PGM_HEADER_MAGIC[1];
}


static inline void normalize_values(unsigned char* buffer,
                                    unsigned int size,
                                    unsigned int maxvalue) {
    if (maxvalue == 255) return;
    for (unsigned int i = 0; i < size; ++i) {
        unsigned long v = buffer[i];
        v = (v*255)/maxvalue;
        buffer[i] = v;
    }
}


static inline void flip_vertically(unsigned char* buffer, unsigned int w, unsigned int h) {
    /* Flip the image vertically, OpenGL expects (0,0) to be the bottom-left of the image */
    unsigned char t;
    unsigned int _y=h-1, i=0, j=0;
    for (unsigned int y = 0; y < h/2; y++) {
        i = y*w;
        j = (_y--)*w;
        for (unsigned int x = 0; x < w; x++) {
            t = buffer[i];
            buffer[i] = buffer[j];
            buffer[j] = t;
            i++;
            j++;
        }
    }
}


int pgm_load_image(const char* path,
                   unsigned char* returnbuffer[],
                   unsigned int* width,
                   unsigned int* height) {
    FILE *f = fopen(path, "r");
    if (!f) return -1;
    if (!pgm_validate_file_descriptor(f)) { fclose(f); return -2; }
    unsigned int maxvalue, pixel, i=0, size;
    if (!fscanf(f, "%d %d\n%d\n", width, height, &maxvalue)) { fclose(f); return -3; }
    size=(*width)*(*height);
    unsigned char* buffer = (unsigned char*)malloc(size);
    while (fscanf(f, "%d", &pixel) && i<size) {
        buffer[i++] = pixel;
        if (getc(f) == EOF) break;
    }
    fclose(f);
    normalize_values(buffer, size, maxvalue);
    flip_vertically(buffer, *width, *height);
    (*returnbuffer) = buffer;
    return 0;
}