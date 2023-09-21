#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "core.h"
#include "text.h"
#include "../lib/bmp.h"

#define BITMAP_FONT_WIDTH 11
#define BITMAP_FONT_HEIGHT 4
#define GLYPH_WIDTH 20
#define GLYPH_HEIGHT 28
#define GLYPH_BORDER 1
#define GLYPH_BORDER_WIDTH  ((float)GLYPH_BORDER/(float)GLYPH_WIDTH)
#define GLYPH_BORDER_HEIGHT ((float)GLYPH_BORDER/(float)GLYPH_HEIGHT)
#define GLYPH_KERNING 0.12

#if ((BITMAP_FONT_WIDTH * GLYPH_WIDTH) % 4) != 0 || ((BITMAP_FONT_HEIGHT * GLYPH_HEIGHT) % 4) != 0
    #error The given font bitmap dimensions are not perfectly divisible by 4. The bmp library \
           included here does not have a dimension adapting feature so any .bmp images used must \
           have dimensions which are each multiples of 4.
#endif

#define TEXTURE_PATH         "texture/font.bmp"
#define VERTEX_SHADER_PATH   "shader/text.vert.cg"
#define FRAGMENT_SHADER_PATH "shader/text.frag.cg"

enum vertex_attribute_location { VERTEX_ATTRIBUTE_POSITION_LOCATION,
                                 VERTEX_ATTRIBUTE_TEXCOORD_LOCATION };

typedef struct { float x, y, u, v; } vertex_t;
typedef struct {const char* message; const float x, y, font_size; } textbox_t;

enum text_buffer_enum { TEXT_SCORE,
                        TEXT_LINES,
                        TEXT_NEXT,
                        TEXT_HELD,
                        TEXT_LEVEL,
                        TEXT_SCORE_NUMBER,
                        TEXT_LINES_NUMBER,
                        TEXT_LEVEL_NUMBER,
                        TEXT_BUFFER_QUANTITY };

const textbox_t textboxes[] = {
    [TEXT_NEXT]  = {.message = "NEXT",  .x=9.15,   .y=4.933,   .font_size=2.0},
    [TEXT_HELD]  = {.message = "HELD",  .x=-13.5, .y=5.74,   .font_size=2.0},
    [TEXT_SCORE] = {.message = "SCORE", .x=-13.9, .y=1.31,  .font_size=2.0},
    [TEXT_LEVEL] = {.message = "LEVEL", .x=-13.9, .y=-2.5, .font_size=2.0},
    [TEXT_LINES] = {.message = "LINES", .x=-13.9, .y=-6, .font_size=2.0},
    [TEXT_SCORE_NUMBER] = {.message = "000000", .x=-13.9-2/3.0, .y=0,  .font_size=2.0},
    [TEXT_LEVEL_NUMBER] = {.message = "00",   .x=-11.9-1/3.0, .y=-3.81, .font_size=2.0},
    [TEXT_LINES_NUMBER] = {.message = "000",   .x=-12.8, .y=-7.31, .font_size=2.0},
};
#define textboxes_size ARRAY_SIZE(textboxes)

static GLuint program;
static GLuint texture_id;
static GLuint vertex_buffer_ids[textboxes_size];


static void load_texture()
{ //{{{
    /* Font bitmap glyphs are "indexed" sans palette, which is defined elsewhere in the tetromino
       graphics code. This bitmap will look completely black to a human but the actual pixel values
       are index offsets from 0 which represent which block-type color to use. This make the font
       glyphs appear as if they are assembled from the same tetrominos the player is stacking. */
    unsigned int width, height;
    uint8_t *pixels = read_monochrome_bmp("app0:" TEXTURE_PATH, &width, &height);

    const size_t rgb_pixels_size = 3*width*height;
    // Vita crashes if we allocate this on the stack with larger images
    unsigned char* rgb_pixels =  (unsigned char*)malloc(rgb_pixels_size);

    #define add_rgb_pixel(r,g,b) rgb_pixels[i]=r, rgb_pixels[i+1]=g, rgb_pixels[i+2]=b

    for (unsigned int i = 0, j=0; i < rgb_pixels_size; i+=3, ++j) {
        switch(pixels[j]) {
            case 0:                add_rgb_pixel(  0,   0,   0); break;
            case TETROMINO_TYPE_I: add_rgb_pixel( 42, 168, 191); break;
            case TETROMINO_TYPE_O: add_rgb_pixel(192, 156,  25); break;
            case TETROMINO_TYPE_T: add_rgb_pixel(135,  30, 115); break;
            case TETROMINO_TYPE_J: add_rgb_pixel( 25,  67, 168); break;
            case TETROMINO_TYPE_L: add_rgb_pixel(228, 106,   8); break;
            case TETROMINO_TYPE_S: add_rgb_pixel(101, 167,  16); break;
            case TETROMINO_TYPE_Z: add_rgb_pixel(168,  19,  39); break;
            default:               add_rgb_pixel(255, 255, 255); break;
        }
    }
    
    free(pixels);

    glGenTextures(1, &texture_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(/* target */ GL_TEXTURE_2D,
                 /* level */  0,
                 /* intfmt */ GL_RGB,
                 /* width */  width,
                 /* height */ height,
                 /* border */ 0,
                 /* format */ GL_RGB,
                 /* type */   GL_UNSIGNED_BYTE,
                 /* data */   rgb_pixels);

    free(rgb_pixels);
/*}}}*/ }


static void get_glyph_bitmap_texcoords(char c, float *u, float *v)
{ //{{{
    float _u, _v;
    switch(c) {
        case ' ': 
        default:  _u=0;  _v=3;  break;
        case 'a': 
        case 'A': _u=0;  _v=3;  break;
        case 'b': 
        case 'B': _u=1;  _v=3;  break;
        case 'c': 
        case 'C': _u=2;  _v=3;  break;
        case 'd': 
        case 'D': _u=3;  _v=3;  break;
        case 'e': 
        case 'E': _u=4;  _v=3;  break;
        case 'f': 
        case 'F': _u=5;  _v=3;  break;
        case 'g': 
        case 'G': _u=6;  _v=3;  break;
        case 'h': 
        case 'H': _u=7;  _v=3;  break;
        case 'i': 
        case 'I': _u=8;  _v=3;  break;
        case 'j': 
        case 'J': _u=9;  _v=3;  break;
        case 'k': 
        case 'K': _u=10; _v=3;  break;
        case 'l': 
        case 'L': _u=0;  _v=2;  break;
        case 'm': 
        case 'M': _u=1;  _v=2;  break;
        case 'n': 
        case 'N': _u=2;  _v=2;  break;
        case 'o': 
        case 'O': _u=3;  _v=2;  break;
        case 'p': 
        case 'P': _u=4;  _v=2;  break;
        case 'q': 
        case 'Q': _u=5;  _v=2;  break;
        case 'r': 
        case 'R': _u=6;  _v=2;  break;
        case 's': 
        case 'S': _u=7;  _v=2;  break;
        case 't': 
        case 'T': _u=8;  _v=2;  break;
        case 'u': 
        case 'U': _u=9;  _v=2;  break;
        case 'v': 
        case 'V': _u=10; _v=2;  break;
        case 'w': 
        case 'W': _u=0;  _v=1;  break;
        case 'x': 
        case 'X': _u=1;  _v=1;  break;
        case 'y': 
        case 'Y': _u=2;  _v=1;  break;
        case 'z': 
        case 'Z': _u=3;  _v=1;  break;
        case '.': _u=4;  _v=1;  break;
        case '!': _u=5;  _v=1;  break;
        case '?': _u=6;  _v=1;  break;
        case '/': _u=7;  _v=1;  break;
        case '=': _u=8;  _v=1;  break;
        case ':': _u=9;  _v=1;  break;
        case ',': _u=10; _v=1;  break;
        case '-': _u=10; _v=0;  break;
        case 0:
        case '0': _u=0;  _v=0;  break;
        case 1:
        case '1': _u=1;  _v=0;  break;
        case 2:
        case '2': _u=2;  _v=0;  break;
        case 3:
        case '3': _u=3;  _v=0;  break;
        case 4:
        case '4': _u=4;  _v=0;  break;
        case 5:
        case '5': _u=5;  _v=0;  break;
        case 6:
        case '6': _u=6;  _v=0;  break;
        case 7:
        case '7': _u=7;  _v=0;  break;
        case 8:
        case '8': _u=8;  _v=0;  break;
        case 9:
        case '9': _u=9;  _v=0;  break;
    }
    *u = _u;
    *v = _v;
    return;
/*}}}*/ }


static void text_to_glyph_vertices(GLuint vertex_buffer_id,
                                   const char* message,
                                   float x,
                                   float y,
                                   float font_size)
{ //{{{
    const unsigned int message_length = strlen(message);
    const size_t vertex_buffer_size = message_length*4;
    vertex_t vertex_buffer[vertex_buffer_size];
    unsigned int vertex_index = 0;
    float u, v;

    #define add_vertex(X, Y, U, V) \
        vertex_buffer[vertex_index++] = (vertex_t) { \
            (X)*((float)font_size*(float)GLYPH_WIDTH/(float)DISPLAY_WIDTH), \
            (Y)*((float)font_size*(float)GLYPH_HEIGHT/(float)DISPLAY_HEIGHT), \
            (U)/(float)BITMAP_FONT_WIDTH, \
            (V)/(float)BITMAP_FONT_HEIGHT \
        }

    float _x = x;
    for (unsigned int i = 0; i < message_length; ++i, _x += 1+GLYPH_KERNING) {
        char c = message[i];
        if (c == ' ') continue;
        else if (c == '\n') { y-=1, _x=x; continue; }
        get_glyph_bitmap_texcoords(c, &u, &v);
        // add_vertex(_x,   y,   u+GLYPH_BORDER_WIDTH,   v+GLYPH_BORDER_HEIGHT);
        // add_vertex(_x,   y+1, u+GLYPH_BORDER_WIDTH,   v+1-GLYPH_BORDER_HEIGHT);
        // add_vertex(_x+1, y+1, u+1-GLYPH_BORDER_WIDTH, v+1-GLYPH_BORDER_HEIGHT);
        // add_vertex(_x+1, y,   u+1-GLYPH_BORDER_WIDTH, v+GLYPH_BORDER_HEIGHT);
        add_vertex(_x,   y,   u,   v);
        add_vertex(_x,   y+1, u,   v+1);
        add_vertex(_x+1, y+1, u+1, v+1);
        add_vertex(_x+1, y,   u+1, v);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glBufferData(/* type */  GL_ARRAY_BUFFER,
                 /* size */  vertex_index*sizeof(vertex_t),
                 /* data */  vertex_buffer,
                 /* usage */ GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
/*}}}*/ }


static GLuint create_text_buffer_from_string(const textbox_t *t)
{ //{{{
    GLuint vertex_buffer_id;
    glGenBuffers(1, &vertex_buffer_id);
    text_to_glyph_vertices(vertex_buffer_id, t->message, t->x, t->y, t->font_size);
    return vertex_buffer_id;
/*}}}*/ }


void graphics_text_init(void)
{ //{{{
    program = glCreateProgram();
    load_shader("app0:" VERTEX_SHADER_PATH, &program);
    load_shader("app0:" FRAGMENT_SHADER_PATH, &program);

    glBindAttribLocation(program, VERTEX_ATTRIBUTE_POSITION_LOCATION, "position");
    glBindAttribLocation(program, VERTEX_ATTRIBUTE_TEXCOORD_LOCATION, "texture_coordinate");

    glLinkProgram(program);
    glUseProgram(program);

    load_texture();

    glUniform1i(glGetUniformLocation(program, "u_font_bitmap_texture"), 0);

    for (unsigned int i = 0; i < textboxes_size; ++i) {
        const textbox_t t = textboxes[i];
        vertex_buffer_ids[i] = create_text_buffer_from_string(&t);
    }

    graphics_text_update_score_number(0);
    graphics_text_update_cleared_lines_number(0);
    graphics_text_update_level_number(1);
/*}}}*/ }


void graphics_text_end(void)
{ //{{{
    glDeleteProgram(program);
    glDeleteTextures(/*texture_quantity=*/1, &texture_id);
    glDeleteBuffers(/*buffer_quantity=*/textboxes_size, vertex_buffer_ids);
/*}}}*/ }


static void draw_text_buffer(GLuint vertex_buffer_id) {
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    unsigned int vertex_buffer_size;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &vertex_buffer_size);
    vertex_buffer_size /= sizeof(vertex_t);

    glEnableVertexAttribArray(VERTEX_ATTRIBUTE_POSITION_LOCATION);
    glVertexAttribPointer(/* location */  VERTEX_ATTRIBUTE_POSITION_LOCATION, /* a.k.a. index */
                          /* dimension */ 2,                                  /* a.k.a. size */
                          /* type */      GL_FLOAT,
                          /* normalize */ GL_FALSE,
                          /* stride */    sizeof(vertex_t),
                          /* pointer */   (GLvoid*)offsetof(vertex_t,x));

    glEnableVertexAttribArray(VERTEX_ATTRIBUTE_TEXCOORD_LOCATION);
    glVertexAttribPointer(/* location */  VERTEX_ATTRIBUTE_TEXCOORD_LOCATION,
                          /* dimension */ 2,
                          /* type */      GL_FLOAT,
                          /* normalize */ GL_FALSE,
                          /* stride */    sizeof(vertex_t),
                          /* pointer */   (GLvoid*)offsetof(vertex_t,u));

    glEnable(GL_BLEND); // Enable glyph backgrounds to be rendered invisibly
    glDrawArrays(/*mode=*/GL_QUADS, /*first=*/0, /*count=*/vertex_buffer_size);
    glDisable(GL_BLEND);
    glDisableVertexAttribArray(VERTEX_ATTRIBUTE_POSITION_LOCATION);
    glDisableVertexAttribArray(VERTEX_ATTRIBUTE_TEXCOORD_LOCATION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void graphics_text_draw(void)
{ //{{{
    glUseProgram(program);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glActiveTexture(GL_TEXTURE0);
    for (int i = 0; i < textboxes_size; ++i) {
        draw_text_buffer(vertex_buffer_ids[i]);
    }
    glUseProgram(0);
/*}}}*/ }


void graphics_text_draw_ad_hoc(const char* message,
                               const float x,
                               const float y,
                               const float font_size) {
    /* Converts the given text to a temporary buffer and renders it one time. */
    const textbox_t t = {message, x, y, font_size};
    const GLuint vertex_buffer_id = create_text_buffer_from_string(&t);
    glUseProgram(program);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glActiveTexture(GL_TEXTURE0);
    draw_text_buffer(vertex_buffer_id);
    glUseProgram(0);
    glDeleteBuffers(/*buffer_quantity=*/1, &vertex_buffer_id);
}


void graphics_text_update_score_number(unsigned int score) {
    #define SCORE_TEXT_FORMAT "%06d"
    const textbox_t score_textbox = textboxes[TEXT_SCORE_NUMBER];
    const size_t score_string_size = snprintf(NULL, 0, SCORE_TEXT_FORMAT, score);
    char message[score_string_size];
    snprintf(message, score_string_size + 1, SCORE_TEXT_FORMAT, score);
    text_to_glyph_vertices(vertex_buffer_ids[TEXT_SCORE_NUMBER],
                           (const char*)message,
                           score_textbox.x,
                           score_textbox.y,
                           score_textbox.font_size);
}


void graphics_text_update_cleared_lines_number(unsigned short lines) {
    #define LINES_TEXT_FORMAT "%03d"
    const textbox_t lines_textbox = textboxes[TEXT_LINES_NUMBER];
    const size_t lines_string_size = snprintf(NULL, 0, LINES_TEXT_FORMAT, lines);
    char message[lines_string_size];
    snprintf(message, lines_string_size + 1, LINES_TEXT_FORMAT, lines);

    text_to_glyph_vertices(vertex_buffer_ids[TEXT_LINES_NUMBER],
                           (const char*)message,
                           lines_textbox.x,
                           lines_textbox.y,
                           lines_textbox.font_size);   
}


void graphics_text_update_level_number(unsigned char level) {
    #define LEVEL_TEXT_FORMAT "%02d"
    const textbox_t level_textbox = textboxes[TEXT_LEVEL_NUMBER];
    const size_t level_string_size = snprintf(NULL, 0, LEVEL_TEXT_FORMAT, level);
    char message[level_string_size];
    snprintf(message, level_string_size + 1, LEVEL_TEXT_FORMAT, level);
    text_to_glyph_vertices(vertex_buffer_ids[TEXT_LEVEL_NUMBER],
                           (const char*)message,
                           level_textbox.x,
                           level_textbox.y,
                           level_textbox.font_size);
}