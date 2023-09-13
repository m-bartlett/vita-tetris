#include <stdint.h>
#include <string.h>
#include "core.h"
#include "text.h"
#include "../lib/bmp.h"

#define BITMAP_FONT_WIDTH 11.0
#define BITMAP_FONT_HEIGHT 4.0
#define GLYPH_WIDTH 4.0
#define GLYPH_HEIGHT 6.0
#define GLYPH_KERNING 0.2
#define FONT_SIZE 10.0

#define TEXTURE_PATH         "texture/font.bmp"
#define VERTEX_SHADER_PATH   "shader/text.vert.cg"
#define FRAGMENT_SHADER_PATH "shader/text.frag.cg"

enum vertex_attribute_location { VERTEX_ATTRIBUTE_POSITION_LOCATION,
                                 VERTEX_ATTRIBUTE_TEXCOORD_LOCATION };

typedef struct { float x, y; float u, v; } vertex_t;
typedef struct {const char* message; const float x,y; } textbox_t;

const textbox_t textboxes[] = {
    {.message = "SCORE", .x=-15.0, .y=-0.61},
    {.message = "LINES", .x=-15.0, .y=-3.61},
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
    glGenTextures(1, &texture_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(/* target */ GL_TEXTURE_2D,
                 /* level */  0,
                 /* intfmt */ GL_RED,
                 /* width */  width,
                 /* height */ height,
                 /* border */ 0,
                 /* format */ GL_RED,
                 /* type */   GL_UNSIGNED_BYTE,
                 /* data */   pixels);
    free(pixels);
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


static GLuint text_to_glyph_vertices(const char* message, float x, float y)
{
    const unsigned int message_length = strlen(message);
    const size_t vertex_buffer_size = message_length*4;
    vertex_t vertex_buffer[vertex_buffer_size];
    unsigned int vertex_index = 0;
    float u, v;

    #define add_vertex(X, Y, U, V) \
        vertex_buffer[vertex_index++]=(vertex_t){(X)*(FONT_SIZE*GLYPH_WIDTH/DISPLAY_WIDTH),\
                                                 (Y)*(FONT_SIZE*GLYPH_HEIGHT/DISPLAY_HEIGHT),\
                                                 (U)/BITMAP_FONT_WIDTH,\
                                                 (V)/BITMAP_FONT_HEIGHT}
    float _x = x;
    for (unsigned int i = 0; i < message_length; ++i) {
        char c = message[i];
        // if (c == ' ') continue;
        // else if (c == '\n') { y+=1; continue; }
        // float _x = x + (i*(GLYPH_WIDTH+GLYPH_KERNING)*FONT_SIZE);
        // float _x = x + (i*GLYPH_WIDTH);
        get_glyph_bitmap_texcoords(c, &u, &v);
        add_vertex(_x,   y,   u,   v);
        add_vertex(_x,   y+1, u,   v+1);
        add_vertex(_x+1, y+1, u+1, v+1);
        add_vertex(_x+1, y,   u+1, v);
        _x += 1+GLYPH_KERNING;
    }

    GLuint vertex_buffer_id;
    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glBufferData(/* type */  GL_ARRAY_BUFFER,
                 /* size */  sizeof(vertex_buffer),
                 /* data */  vertex_buffer,
                 /* usage */ GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return vertex_buffer_id;
}


void graphics_text_init(void) {
    program = glCreateProgram();
    load_shader("app0:" VERTEX_SHADER_PATH, &program);
    load_shader("app0:" FRAGMENT_SHADER_PATH, &program);

    glBindAttribLocation(program, VERTEX_ATTRIBUTE_POSITION_LOCATION, "position");
    glBindAttribLocation(program, VERTEX_ATTRIBUTE_TEXCOORD_LOCATION, "texture_coordinate");

    glLinkProgram(program);
    glUseProgram(program);

    GLuint u_block_type_colors_location = glGetUniformLocation(program, "u_block_type_colors");
    // TO-DO: 3 bytes instead of 3 floats, maybe try normalize=true or /255.0 in shader
    const float u_block_type_colors[TETROMINO_TYPE_QUANTITY][3] = {
        [TETROMINO_TYPE_NULL] = {1.0, 1.0, 1.0},
        [TETROMINO_TYPE_I] = {0.2431, 0.8627, 1.0},
        [TETROMINO_TYPE_O] = {1.0,    0.7750, 0.1216},
        [TETROMINO_TYPE_T] = {0.7920, 0.1726, 0.6745},
        [TETROMINO_TYPE_J] = {0.1255, 0.3333, 0.8314},
        [TETROMINO_TYPE_L] = {1.0,    0.406,  0.029},
        [TETROMINO_TYPE_S] = {0.4980, 0.8275, 0.0824},
        [TETROMINO_TYPE_Z] = {0.9333, 0.102,  0.2118},
    };
    glUniform3fv(u_block_type_colors_location,
                 TETROMINO_TYPE_QUANTITY,
                 (const float*)u_block_type_colors);

    load_texture();

    glUniform1i(glGetUniformLocation(program, "u_font_bitmap_texture"), 0);

    for (unsigned int i = 0; i < textboxes_size; ++i) {
        const textbox_t t = textboxes[i];
        vertex_buffer_ids[i] = text_to_glyph_vertices(t.message, t.x, t.y);
    }
}


void graphics_text_end(void)
{ //{{{
    glDeleteProgram(program);
    glDeleteTextures(/*texture_quantity=*/1, &texture_id);
    glDeleteBuffers(/*buffer_quantity=*/textboxes_size, vertex_buffer_ids);
/*}}}*/ }


void graphics_text_draw(void) { //{{{
    glUseProgram(program);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glActiveTexture(GL_TEXTURE0);

    for (int i = 0; i < textboxes_size; ++i) {
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_ids[i]);
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
    }
    
    glDisableVertexAttribArray(VERTEX_ATTRIBUTE_POSITION_LOCATION);
    glDisableVertexAttribArray(VERTEX_ATTRIBUTE_TEXCOORD_LOCATION);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
/*}}}*/ }