#include <stdint.h>
#include "core.h"
#include "border.h"
#include "../engine/playfield.h"

#define VERTEX_SHADER_PATH   "shader/border.vert.cg"
#define FRAGMENT_SHADER_PATH "shader/border.frag.cg"

enum vertex_attribute_location { VERTEX_ATTRIBUTE_POSITION_LOCATION };

typedef struct { const float x, y, width, height;
                 const uint8_t thickness;
                 const float r, g, b; } border_t;

typedef struct { const float x, y; } vertex_t;

static const border_t borders[] = {
    {
        // Playfield border
        #define PLAYFIELD_BORDER_WIDTH 0.56
        #define PLAYFIELD_BORDER_HEIGHT 1.9
        .x = -(PLAYFIELD_BORDER_WIDTH/2), .y = -(PLAYFIELD_BORDER_HEIGHT/2),
        .width = PLAYFIELD_BORDER_WIDTH, .height = PLAYFIELD_BORDER_HEIGHT,
        .r=0.85, .g=0.85, .b=0.85,
        .thickness = 2,
    },
    {
        // Tetromino queue border
        .x = 0.38, .y = -0.61,
        .width = 0.18, .height = 1.1,
        .r=0.85, .g=0.85, .b=0.85,
        .thickness = 2,
    },
    {
        // Held tetromino border
        .x = -0.56, .y = 0.375,
        .width = 0.18, .height = .2,
        .r=0.85, .g=0.85, .b=0.85,
        .thickness = 2,
    },
    {
        // Score border
        .x = -0.324, .y = -0.01,
        .width = -0.288, .height = .125,
        .r=0.85, .g=0.85, .b=0.85,
        .thickness = 2,
    },
    {
        // Level border
        .x = -0.415, .y = -0.405,
        .width = -0.102, .height = .125,
        .r=0.85, .g=0.85, .b=0.85,
        .thickness = 2,
    },
    {
        // Lines border
        .x = -0.392, .y = -0.765,
        .width = -0.148, .height = .125,
        .r=0.85, .g=0.85, .b=0.85,
        .thickness = 2,
    },
};

#define borders_size ARRAY_SIZE(borders)

static GLuint program;
static GLuint vertex_buffer_ids[borders_size];
static GLuint u_border_color_location;

void graphics_border_init(void) { //{{{
    program = glCreateProgram();
    load_shader("app0:" VERTEX_SHADER_PATH, &program);
    load_shader("app0:" FRAGMENT_SHADER_PATH, &program);

    glBindAttribLocation(program, VERTEX_ATTRIBUTE_POSITION_LOCATION, "position");

    glLinkProgram(program);
    glUseProgram(program);

    u_border_color_location = glGetUniformLocation(program, "u_border_color");

    for (int i = 0; i < borders_size; ++i) {
        const border_t border = borders[i];
        // Prepare mesh for GL_LINE_LOOP line primitive
        //    https://www.khronos.org/opengl/wiki/Primitive#Line_primitives
        const vertex_t vertex_buffer[4] = {
            {.x=border.x,                .y=border.y},
            {.x=border.x + border.width, .y=border.y},
            {.x=border.x + border.width, .y=border.y + border.height},
            {.x=border.x,                .y=border.y + border.height},
        };

        glGenBuffers(1, &(vertex_buffer_ids[i]));
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_ids[i]);
        glBufferData(/*type*/ GL_ARRAY_BUFFER,
                     /*size*/ sizeof(vertex_buffer),
                     /*data*/ vertex_buffer,
                     /*usage*/GL_STATIC_DRAW);
    }
/*}}}*/ }


void graphics_border_end(void) { //{{{
    glDeleteProgram(program);
    glDeleteBuffers(/*buffer_quantity=*/borders_size, vertex_buffer_ids);
/*}}}*/ }


void graphics_border_draw(void) { //{{{
    glUseProgram(program);

    for (int i = 0; i < borders_size; ++i) {
        const border_t border = borders[i];
        glUniform3fv(u_border_color_location, 1, (const GLfloat[3]){border.r, border.g, border.b});

        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_ids[i]);

        glEnableVertexAttribArray(VERTEX_ATTRIBUTE_POSITION_LOCATION);
        glVertexAttribPointer(/* location */  VERTEX_ATTRIBUTE_POSITION_LOCATION,
                              /* dimension */ 2,
                              /* type */      GL_FLOAT,
                              /* normalize */ GL_FALSE,
                              /* stride */    sizeof(vertex_t),
                              /* pointer */   (GLvoid*)offsetof(vertex_t,x));

        glLineWidth(border.thickness);
        glDrawArrays(/*mode=*/GL_LINE_LOOP, /*first=*/0, /*count=*/4);
    }

    glDisableVertexAttribArray(VERTEX_ATTRIBUTE_POSITION_LOCATION);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
/*}}}*/ }