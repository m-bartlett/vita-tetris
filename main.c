#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES

#define _GNU_SOURCE

#include <sys/time.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <vitasdk.h>
#include <vitaGL.h>
#include "linalg.h"

#define DISPLAY_WIDTH 960
#define DISPLAY_HEIGHT 544

#define ARRAY_SIZE(A) (sizeof(A) / sizeof(A[0]))

#define PLAYFIELD_WIDTH 10
#define PLAYFIELD_HEIGHT 20
#define ROW_STRIP_VERTEX_COUNT_MAX (6*(PLAYFIELD_WIDTH/2) + 4*(PLAYFIELD_WIDTH%2) + 1)
#define STRIP_VERTEX_COUNT_MAX (ROW_STRIP_VERTEX_COUNT_MAX * PLAYFIELD_HEIGHT * 3)


// enum tetromino_type_enum { TETROMINO_TYPE_NULL = 0,
//                            TETROMINO_TYPE_I    = 1,
//                            TETROMINO_TYPE_O    = 2,
//                            TETROMINO_TYPE_T    = 3,
//                            TETROMINO_TYPE_J    = 4,
//                            TETROMINO_TYPE_L    = 5,
//                            TETROMINO_TYPE_S    = 6,
//                            TETROMINO_TYPE_Z    = 7,
//                            TETROMINO_TYPE_QUANTITY };


static const uint8_t PLAYFIELD[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH] = {
	{1,0,0,0,0,0,0,0,0,0},
	{2,2,0,0,0,0,0,0,0,0},
	{3,3,3,0,0,0,0,0,0,0},
	{4,4,4,4,0,0,0,0,0,0},
	{5,5,5,5,5,0,0,0,0,0},
	{6,6,6,6,6,6,0,0,0,0},
	{0,3,3,3,3,3,3,0,0,0},
	{0,0,4,4,4,4,4,4,0,0},
	{0,0,0,5,5,5,5,5,5,0},
	{0,0,0,0,6,6,6,6,6,6},
	{7,7,7,7,7,7,0,0,0,0},
	{0,3,3,3,3,3,3,0,0,0},
	{0,0,4,4,4,4,4,4,0,0},
	{0,0,0,5,5,5,5,5,5,0},
	{0,0,0,0,6,6,6,6,6,6},
	{0,0,0,0,0,5,5,5,5,5},
	{0,0,0,0,0,0,4,4,4,4},
	{0,0,0,0,0,0,0,3,3,3},
	{0,0,0,0,0,0,0,0,2,2},
	{0,0,0,0,0,0,0,0,0,1},
};

#define TEXTURE_HEIGHT 12
#define TEXTURE_WIDTH 12
static const uint8_t TEXTURE[TEXTURE_HEIGHT*TEXTURE_WIDTH] = {
	250,244,238,232,226,220,215,209,204,199,193,66,
	244,87,97,106,116,127,138,149,161,173,185,60,
	238,97,106,116,127,138,149,161,173,185,87,55,
	232,106,116,127,138,149,161,173,185,87,97,49,
	226,116,127,138,149,161,173,185,87,97,106,44,
	220,127,138,149,161,173,185,87,97,106,116,38,
	215,138,149,161,173,185,87,97,106,116,127,32,
	209,149,161,173,185,87,97,106,116,127,138,27,
	204,161,173,185,87,97,106,116,127,138,149,21,
	199,173,185,87,97,106,116,127,138,149,161,16,
	193,185,87,97,106,116,127,138,149,161,173,10,
	66,60,55,49,44,38,32,27,21,16,10,5,
};


#define POSITION_LOCATION 0
#define TEXCOORD_LOCATION 1
#define TYPE_LOCATION 2
// #define NORMAL_LOCATION 2

typedef struct {
    uint8_t x, y, z;
    uint8_t u, v;
    uint8_t type;
} vertex_t;

static vertex_t VERTEX_BUFFER[STRIP_VERTEX_COUNT_MAX];
static uint16_t VERTEX_BUFFER_SIZE=0;


void parse_playfield_to_strip_vertices() {
	uint16_t vertex_index = 0;
	GLboolean current_is_populated, previous_was_populated;
	uint8_t y=0, x=0, y1=0, x1=0, r=PLAYFIELD_HEIGHT-1;

	while (y < PLAYFIELD_HEIGHT) {
		previous_was_populated = GL_FALSE;
		y1 = y+1;
		x = 0;
		const char* row = PLAYFIELD[r--];

		while (x < PLAYFIELD_WIDTH) {
			const uint8_t block = row[x];
			current_is_populated = block != '\0';
			x1 = x+1;

			if (current_is_populated) {

				if (!previous_was_populated) {
					// Queue top left vertex twice to create a degenerate start point
					VERTEX_BUFFER[vertex_index++] = (vertex_t){.x=x, .y=y1, .z=0, .u=0, .v=1, .type=block};
					VERTEX_BUFFER[vertex_index++] = VERTEX_BUFFER[vertex_index-1];
					// Queue bottom left vertex
					VERTEX_BUFFER[vertex_index++] = (vertex_t){.x=x, .y=y, .z=0, .u=0, .v=0, .type=block};
				}

				// Queue top right & bottom right vertices, these are contiguous quads in the strip
				VERTEX_BUFFER[vertex_index++] = (vertex_t){.x=x1, .y=y1, .z=0, .u=1, .v=1, .type=block};
				VERTEX_BUFFER[vertex_index++] = (vertex_t){.x=x1, .y=y, .z=0, .u=0, .v=1, .type=block};
			}

			else {  // Current is empty

				if (previous_was_populated) {
					// Queue bottom left vertex again to create a degenerate end point
					VERTEX_BUFFER[vertex_index++] = (vertex_t){.x=x, .y=y, .z=0, .u=0, .v=0, .type=block};
				}

				// Otherwise, previous empty & current empty so nothing to do
			}

			previous_was_populated = current_is_populated;
			x = x1;
		}

		if (previous_was_populated) {
			// If the last square in the row was populated, manually add a final degenerate vertex
		    VERTEX_BUFFER[vertex_index++] = VERTEX_BUFFER[vertex_index-1];
		}

		y = y1;
	}

	vertex_index--; // last vertex is ignorable degenerate restart
	VERTEX_BUFFER_SIZE = vertex_index;
}


static GLuint VertexBufferID_g, TextureID_g;
static GLfloat view_rot[3] = { 20.0, 30.0, 0.0 };
static GLfloat angle = 0.0;
static GLuint ModelViewProjectionMatrix_location,
              NormalMatrix_location,
              LightSourcePosition_location,
              MaterialColor_location;
static GLfloat ProjectionMatrix[16];
static const GLfloat LightSourcePosition[4] = { 5.0, 5.0, 10.0, 1.0};


void load_shader(const char *shader_path, GLuint shader_type, GLuint *program) {
   FILE *f = fopen(shader_path, "r");
   if (!f) {printf("Shader error\n\n"); return; }
   fseek(f, 0, SEEK_END);
   uint32_t shader_size = ftell(f);
   fseek(f, 0, SEEK_SET);
   char shader_body[shader_size];
   fread(shader_body, 1, shader_size, f);
   fclose(f);
   printf("%s\n\n", shader_body);

   GLuint shader_ref = glCreateShader(shader_type);
   glShaderSource(/* shader */ shader_ref,
                  /* sources count */ 1,
                  /* array of string arrays */ (const char*[]){(const char*)&shader_body},
                  /* string length(s) */ &shader_size);
   glCompileShader(shader_ref);
   glAttachShader(*program, shader_ref);
}


static void draw_cube(GLfloat *transform,
                      GLfloat x,
                      GLfloat y,
                      GLfloat angle,
                      const GLfloat color[4]) {
   GLfloat model_view[16];
   GLfloat normal_matrix[16];
   GLfloat model_view_projection[16];

   /* Translate and rotate the cube */
                 
   memcpy(model_view, transform, sizeof (model_view));
   translate(model_view, x, y, 0);
   rotate(model_view, 2 * M_PI * angle / 360.0, 0, 0, 1);

   /* Create and set the ModelViewProjectionMatrix */
   memcpy(model_view_projection, ProjectionMatrix, sizeof(model_view_projection));
   multiply(model_view_projection, model_view);

   glUniformMatrix4fv(ModelViewProjectionMatrix_location, 1, GL_FALSE,
                      model_view_projection);

   memcpy(normal_matrix, model_view, sizeof (normal_matrix));
   invert(normal_matrix);
   transpose(normal_matrix);

   glUniformMatrix4fv(NormalMatrix_location, 1, GL_FALSE, normal_matrix);

   glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID_g);

   glEnableVertexAttribArray(POSITION_LOCATION);
   glVertexAttribPointer(/* location */  POSITION_LOCATION,
                         /* dimension */ 3,
                         /* type */      GL_UNSIGNED_BYTE,
                         /* normalize */ GL_FALSE,
                         /* stride */    sizeof(vertex_t),
                         /* pointer */   offsetof(vertex_t,x));
                         	  // If GL_ARRAY_BUFFER is bound, *pointer* is an offset into it

   glEnableVertexAttribArray(TEXCOORD_LOCATION);
   glVertexAttribPointer(/* location */  TEXCOORD_LOCATION,
                         /* dimension */ 2,
                         /* type */      GL_UNSIGNED_BYTE,
                         /* normalize */ GL_FALSE,
                         /* stride */    sizeof(vertex_t),
                         /* pointer */   (GLvoid*)offsetof(vertex_t,u));

   glEnableVertexAttribArray(TYPE_LOCATION);
   glVertexAttribPointer(/* location */  TYPE_LOCATION,
                         /* dimension */ 1,
                         /* type */      GL_UNSIGNED_BYTE,
                         /* normalize */ GL_FALSE,
                         /* stride */    sizeof(vertex_t),
                         /* pointer */   (GLvoid*)offsetof(vertex_t,type));

   glDrawArrays(/* mode */  GL_TRIANGLE_STRIP,
                /* first */ 0,
                /* count */ (VERTEX_BUFFER_SIZE-1));

   glDisableVertexAttribArray(POSITION_LOCATION);
   glDisableVertexAttribArray(TEXCOORD_LOCATION);
   glDisableVertexAttribArray(TYPE_LOCATION);
}

static void cube_draw(void) {
   const static GLfloat color[4] = { 1.0, 0.0, 0.4, 1.0 };
   GLfloat transform[16];
   identity(transform);

   glClearColor(0.1, 0.1, 0.1, 1.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   /* Translate and rotate the view */
   translate(transform, -PLAYFIELD_WIDTH/2, -PLAYFIELD_HEIGHT/2, -20);
   rotate(transform, 2 * M_PI * view_rot[0] / 360.0, 1, 0, 0);
   rotate(transform, 2 * M_PI * view_rot[1] / 360.0, 0, 1, 0);
   rotate(transform, 2 * M_PI * view_rot[2] / 360.0, 0, 0, 1);

   /* Draw the cube */
   draw_cube(transform, 0, 0, angle, color);

   vglSwapBuffers(GL_FALSE);
}

static void cube_reshape(int width, int height) {
   perspective(ProjectionMatrix, 60.0, width / (float)height, 1.0, 1024.0);
   glViewport(0, 0, (GLint) width, (GLint) height);
}

static void cube_special(int special, int crap, int morecrap) {
   SceCtrlData pad;
   sceCtrlPeekBufferPositive(0, &pad, 1);
   if (pad.buttons & SCE_CTRL_LEFT)  view_rot[1] += 5.0;
   if (pad.buttons & SCE_CTRL_RIGHT) view_rot[1] -= 5.0;
   if (pad.buttons & SCE_CTRL_UP)    view_rot[0] += 5.0;
   if (pad.buttons & SCE_CTRL_DOWN)  view_rot[0] -= 5.0;
}

static void cube_idle(void) {
   static int frames = 0;
   static double tRot0 = -1.0, tRate0 = -1.0;
   double dt, t = (double)sceKernelGetProcessTimeWide() / 1000000.0;

   if (tRot0 < 0.0) tRot0 = t;
   dt = t - tRot0;
   tRot0 = t;

   /* advance rotation for next frame */
   angle += 70.0 * dt;  /* 70 degrees per second */
   if (angle > 3600.0) angle -= 3600.0;

   cube_special(0, 0, 0);
   cube_reshape(DISPLAY_WIDTH, DISPLAY_HEIGHT);
   cube_draw();
   frames++;

   if (tRate0 < 0.0)
      tRate0 = t;
   if (t - tRate0 >= 5.0) {
      GLfloat seconds = t - tRate0;
      GLfloat fps = frames / seconds;
      printf("%d frames in %3.1f seconds = %6.3f FPS\n", frames, seconds,
            fps);
      tRate0 = t;
      frames = 0;
   }
}


static void cube_init(void) {
   // glEnable(GL_CULL_FACE);
   glFrontFace(GL_CCW); 
   glEnable(GL_DEPTH_TEST);

   GLuint program = glCreateProgram();
   load_shader("app0:vertex.cg", GL_VERTEX_SHADER, &program);
   load_shader("app0:fragment.cg", GL_FRAGMENT_SHADER, &program);

   glBindAttribLocation(program, POSITION_LOCATION, "position");
   glBindAttribLocation(program, TEXCOORD_LOCATION, "texcoord");
   glBindAttribLocation(program, TYPE_LOCATION, "type");
   // glBindAttribLocation(program, NORMAL_LOCATION, "normal");

   glLinkProgram(program);
   glUseProgram(program);

   /* Get the locations of the uniforms so we can access them */
   ModelViewProjectionMatrix_location = glGetUniformLocation(program, "ModelViewProjectionMatrix");
   NormalMatrix_location = glGetUniformLocation(program, "NormalMatrix");
   // LightSourcePosition_location = glGetUniformLocation(program, "LightSourcePosition");

   glUniform4fv(LightSourcePosition_location, 1, LightSourcePosition);

   parse_playfield_to_strip_vertices();


   glGenBuffers(1, &VertexBufferID_g);
   glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID_g);
   glBufferData(/* type */  GL_ARRAY_BUFFER,
                /* size */  (VERTEX_BUFFER_SIZE-1) * sizeof(vertex_t),
                /* data */  &VERTEX_BUFFER[1], // Discard first vertex, it's degenerate.
                /* usage */ GL_STATIC_DRAW);

   glGenTextures(1, &TextureID_g);
   glBindTexture(GL_TEXTURE_2D, TextureID_g);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexImage2D(/* target */ GL_TEXTURE_2D,
                /* level */  0,
                /* intfmt */ GL_RED,
                /* width */  TEXTURE_WIDTH,
                /* height */ TEXTURE_HEIGHT,
                /* border */ 0,
                /* format */ GL_RED,
                /* type */   GL_UNSIGNED_BYTE,
                /* data */   TEXTURE);

   glUniform1i(glGetUniformLocation(program, "gTexture"), 0);
   glActiveTexture(GL_TEXTURE0);

}

int main(int argc, char *argv[]) {
   /* Initialize the window */
   vglInitExtended(0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0x800000, SCE_GXM_MULTISAMPLE_4X);

   cube_init();

   cube_reshape(DISPLAY_WIDTH, DISPLAY_HEIGHT);

   while(1) {
   	cube_special(0,0,0);
   	cube_draw();
   	// cube_idle();
   }

   return 0;
}
