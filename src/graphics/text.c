#include <stdint.h>

#define GLYPH_HEIGHT 6
#define GLYPH_WIDTH 4

typedef struct { uint8_t x, y, u, v; } vertex_t;

static void get_glyph_bitmap_texcoords(char c, unsigned char *u, unsigned char *v)
{
    unsigned char _u, _v;
    switch(c) {
        case ' ': 
        default:  _u=0;  _v=0;  break;
        case 'a': 
        case 'A': _u=0;  _v=0;  break;
        case 'b': 
        case 'B': _u=1;  _v=0;  break;
        case 'c': 
        case 'C': _u=2;  _v=0;  break;
        case 'd': 
        case 'D': _u=3;  _v=0;  break;
        case 'e': 
        case 'E': _u=4;  _v=0;  break;
        case 'f': 
        case 'F': _u=5;  _v=0;  break;
        case 'g': 
        case 'G': _u=6;  _v=0;  break;
        case 'h': 
        case 'H': _u=7;  _v=0;  break;
        case 'i': 
        case 'I': _u=8;  _v=0;  break;
        case 'j': 
        case 'J': _u=9;  _v=0;  break;
        case 'k': 
        case 'K': _u=10; _v=0;  break;
        case 'l': 
        case 'L': _u=0;  _v=1;  break;
        case 'm': 
        case 'M': _u=1;  _v=1;  break;
        case 'n': 
        case 'N': _u=2;  _v=1;  break;
        case 'o': 
        case 'O': _u=3;  _v=1;  break;
        case 'p': 
        case 'P': _u=4;  _v=1;  break;
        case 'q': 
        case 'Q': _u=5;  _v=1;  break;
        case 'r': 
        case 'R': _u=6;  _v=1;  break;
        case 's': 
        case 'S': _u=7;  _v=1;  break;
        case 't': 
        case 'T': _u=8;  _v=1;  break;
        case 'u': 
        case 'U': _u=9;  _v=1;  break;
        case 'v': 
        case 'V': _u=10; _v=1;  break;
        case 'w': 
        case 'W': _u=0;  _v=2;  break;
        case 'x': 
        case 'X': _u=1;  _v=2;  break;
        case 'y': 
        case 'Y': _u=2;  _v=2;  break;
        case 'z': 
        case 'Z': _u=3;  _v=2;  break;
        case '.': _u=4;  _v=2;  break;
        case '!': _u=5;  _v=2;  break;
        case '?': _u=6;  _v=2;  break;
        case '/': _u=7;  _v=2;  break;
        case '=': _u=8;  _v=2;  break;
        case ':': _u=9;  _v=2;  break;
        case ',': _u=10; _v=2;  break;
        case '-': _u=10; _v=3;  break;
        case '0': _u=0;  _v=3;  break;
        case '1': _u=1;  _v=3;  break;
        case '2': _u=2;  _v=3;  break;
        case '3': _u=3;  _v=3;  break;
        case '4': _u=4;  _v=3;  break;
        case '5': _u=5;  _v=3;  break;
        case '6': _u=6;  _v=3;  break;
        case '7': _u=7;  _v=3;  break;
        case '8': _u=8;  _v=3;  break;
        case '9': _u=9;  _v=3;  break;
    }
    *u = _u;
    *v = _v;
    return;
}

static void make_textbox_quad(const char* message, unsigned int wiqth, unsigned int height)
{
    /* 
        TO-DO: build quad per renderable character into width and height, leave empty quads 
        for spaces.
    */
}