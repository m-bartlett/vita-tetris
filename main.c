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

static uint16_t VERTEX_BUFFER[STRIP_VERTEX_COUNT_MAX]={0};
static uint16_t VERTEX_BUFFER_SIZE=0;
static const char PLAYFIELD[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH] = {
   "J\0\0\0II\0\0ZS",
   "\0\0I\0Z\0L\0\0\0",
   "T\0\0O\0IZT\0L",
   "\0\0\0LZL\0\0JO",
   "\0\0\0\0\0\0I\0\0\0",
   "TLL\0\0\0\0J\0\0",
   "\0\0\0OILIZJI",
   "J\0TLLTSTOZ",
   "LL\0\0\0\0\0\0JZ",
   "JO\0J\0\0\0\0\0\0",
   "LLLIITLILL",
   "\0J\0\0\0\0\0\0O\0",
   "IL\0\0O\0\0ZLI",
   "T\0\0I\0LL\0OL",
   "\0LZTOTSJ\0J",
   "\0SI\0SJ\0\0\0\0",
   "STZO\0\0IT\0T",
   "\0\0O\0\0LT\0TI",
   "O\0SZ\0LS\0\0J",
   "JO\0\0TTOL\0L",
};


void parse_playfield_to_strip_vertices() {
	uint16_t vertex_index = 0;
	GLboolean current_is_populated, previous_was_populated;
	uint8_t y=0, x=0, y1=0, x1=0;

	while (y < PLAYFIELD_HEIGHT) {
		const char* row = PLAYFIELD[y];
		previous_was_populated = GL_FALSE;
		y1 = y+1;
		x = 0;

		while (x < PLAYFIELD_WIDTH-1) {
			current_is_populated = row[x] != '\0';
			x1 = x+1;

			// printf("(%d,%d) %c \n", x,y, row[x_1]);

			if (current_is_populated) {

				if (!previous_was_populated) {
					// Queue top left vertex twice to create a degenerate start point
					VERTEX_BUFFER[vertex_index++] = x;
					VERTEX_BUFFER[vertex_index++] = y1;
					VERTEX_BUFFER[vertex_index++] = 0;
					VERTEX_BUFFER[vertex_index++] = x;
					VERTEX_BUFFER[vertex_index++] = y1;
					VERTEX_BUFFER[vertex_index++] = 0;
					// Queue bottom left vertex
					VERTEX_BUFFER[vertex_index++] = x;
					VERTEX_BUFFER[vertex_index++] = y;
					VERTEX_BUFFER[vertex_index++] = 0;
				}

				// Queue top right & bottom right vertices. These are contiguous quads in the strip.
				VERTEX_BUFFER[vertex_index++] = x1;
				VERTEX_BUFFER[vertex_index++] = y1;
				VERTEX_BUFFER[vertex_index++] = 0;
				VERTEX_BUFFER[vertex_index++] = x1;
				VERTEX_BUFFER[vertex_index++] = y;
				VERTEX_BUFFER[vertex_index++] = 0;
			}

			else {  // Current is empty

				if (previous_was_populated) {
					// Queue bottom left vertex again to create a degenerate end point
					VERTEX_BUFFER[vertex_index++] = x;
					VERTEX_BUFFER[vertex_index++] = y;
					VERTEX_BUFFER[vertex_index++] = 0;
				}

				// Otherwise, previous empty & current empty so nothing to do
			}

			previous_was_populated = current_is_populated;
			x = x1;
		}

		if (previous_was_populated) {
		    VERTEX_BUFFER[vertex_index++] = VERTEX_BUFFER[vertex_index-3];
		    VERTEX_BUFFER[vertex_index++] = VERTEX_BUFFER[vertex_index-3];
		    VERTEX_BUFFER[vertex_index++] = VERTEX_BUFFER[vertex_index-3];
		}

		y = y1;
	}

	fflush(stdout);

	vertex_index -= 3; // last vertex is ignorable degenerate restart
	VERTEX_BUFFER_SIZE = vertex_index;
}


static GLint VBOglobal;
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

   glUniform4fv(MaterialColor_location, 1, color);

   glBindBuffer(GL_ARRAY_BUFFER, VBOglobal);

   glEnableVertexAttribArray(0);
   glVertexAttribPointer(/* location */   0,
                         /* dimensions */ 3,
                         /* type */       GL_SHORT,
                         /* normalized */ GL_FALSE,
                         /* stride */     3 * sizeof(int16_t),
                         /* pointer */    NULL); // NULL == use bound GL_ARRAY_BUFFER

   // glBindBuffer(GL_ARRAY_BUFFER, 0); // Not sure why this is needed, but it is.
   // glEnableVertexAttribArray(1);
   // glVertexAttribPointer(/* location */   1,
   //                       /* dimensions */ 3,
   //                       /* type */       GL_SHORT,
   //                       /* normalized */ GL_FALSE,
   //                       /* stride */     3 * sizeof(int16_t),
   //                       /* pointer */    CUBE_VERTEX_NORMALS);

   glBindBuffer(GL_ARRAY_BUFFER, VBOglobal);
   glDrawArrays(/* mode */  GL_TRIANGLE_STRIP,
                /* first */ 0,
                /* count */ (VERTEX_BUFFER_SIZE-1)/3);

   /* Disable the attributes */
   glDisableVertexAttribArray(0);
   // glDisableVertexAttribArray(1);
}

static void cube_draw(void) {
   const static GLfloat color[4] = { 1.0, 0.0, 0.4, 1.0 };
   GLfloat transform[16];
   identity(transform);

   glClearColor(0.5, 0.5, 0.5, 1.0);
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
   glEnable(GL_CULL_FACE);
   glFrontFace(GL_CCW); 
   glEnable(GL_DEPTH_TEST);

   GLuint program = glCreateProgram();
   load_shader("app0:vertex.cg", GL_VERTEX_SHADER, &program);
   load_shader("app0:fragment.cg", GL_FRAGMENT_SHADER, &program);

   glLinkProgram(program);
   glUseProgram(program);

   glBindAttribLocation(program, 0, "position");
   glBindAttribLocation(program, 1, "normal");

   /* Get the locations of the uniforms so we can access them */
   ModelViewProjectionMatrix_location = glGetUniformLocation(program, "ModelViewProjectionMatrix");
   NormalMatrix_location = glGetUniformLocation(program, "NormalMatrix");
   LightSourcePosition_location = glGetUniformLocation(program, "LightSourcePosition");
   MaterialColor_location = glGetUniformLocation(program, "MaterialColor");
   /* Set the LightSourcePosition uniform which is constant throught the program */
   glUniform4fv(LightSourcePosition_location, 1, LightSourcePosition);

   parse_playfield_to_strip_vertices();
   const uint16_t* vertex_buffer = &VERTEX_BUFFER[3]; // Discard first vertex, it's degenerate.
   
   glGenBuffers(1, &VBOglobal);
   glBindBuffer(GL_ARRAY_BUFFER, VBOglobal);
   glBufferData(/* type */  GL_ARRAY_BUFFER,
                /* size */  (VERTEX_BUFFER_SIZE-1) * sizeof(uint16_t),
                /* data */  vertex_buffer,
                /* usage */ GL_STATIC_DRAW);
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
