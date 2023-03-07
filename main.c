// Drawing a rotating cube
#include <vitasdk.h>
#include <vitaGL.h>
#include <math.h>

#define ARRAY_SIZE(A) (sizeof(A) / sizeof(A[0]))

const int16_t CUBE_VERTICES[] = {
	 1, 1, 1,    1, 1,-1,    1,-1, 1,    1,-1,-1, 
	-1, 1, 1,   -1, 1,-1,   -1,-1, 1,   -1,-1,-1
};

const float CUBE_VERTEX_COLORS[] = {
	// 1,1,1,  1,1,0,  1,0,1,  1,0,0,
	// 0,1,1,  0,1,0,  0,0,1,  0,0,0
	0,0,0,   0,0,1,   0,1,0,   0,1,1,
	1,0,0,   1,0,1,   1,1,0,   1,1,1   
};

const uint16_t CUBE_FACE_VERTEX_INDICES[] = {
	// These must assemble each triangular face in clockwise orientation to properly cull
    0, 2, 1,   1, 2, 3,  /* right  */
    4, 7, 6,   4, 5, 7,  /* left   */
    2, 4, 6,   2, 0, 4,  /* front  */
    // 1, 3, 7,   1, 7, 5,  /* back   */
    3, 6, 7,   3, 2, 6,  /* top    */
	0, 5, 4,   0, 1, 5,  /* bottom */
};
const size_t CUBE_FACE_VERTEX_INDICES_SIZE = ARRAY_SIZE(CUBE_FACE_VERTEX_INDICES);


int main(){
	// Initializing graphics device
	vglInit(0x800000);
	
	// Enabling V-Sync
	vglWaitVblankStart(GL_TRUE);
	
	// Setting clear color
	glClearColor (0.0f, 0.0f, 0.0f, 0.0f);

	// Initializing mvp matrix with a perspective full screen matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0f, 960.f/544.0f, 0.01f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -4.0f); // Centering the cube
	
	// Enabling depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);

	glEnable( GL_BLEND );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	
	// Main loop
	for (;;){
		// Clear color and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Drawing our cube with vertex arrays
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(3, GL_SHORT, 0, CUBE_VERTICES);
		glColorPointer(3, GL_FLOAT, 0, CUBE_VERTEX_COLORS);


		SceCtrlData pad;
		sceCtrlPeekBufferPositive(0, &pad, 1);
		if (pad.buttons & SCE_CTRL_LEFT)   glRotatef( 1, 0, 1, 0);
		if (pad.buttons & SCE_CTRL_RIGHT)  glRotatef(-1, 0, 1, 0);
		if (pad.buttons & SCE_CTRL_UP)     glRotatef( 1, 1, 0, 0);
		if (pad.buttons & SCE_CTRL_DOWN)   glRotatef(-1, 1, 0, 0);
		if (pad.buttons & SCE_CTRL_CROSS)  glTranslatef(0, 0, -0.1);
		if (pad.buttons & SCE_CTRL_CIRCLE) glTranslatef(0, 0, 0.1);

		glDrawElements(GL_TRIANGLES, CUBE_FACE_VERTEX_INDICES_SIZE, GL_UNSIGNED_SHORT, CUBE_FACE_VERTEX_INDICES);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		
		// Performing buffer swap
		vglSwapBuffers(GL_FALSE);
	}
	
	// Terminating graphics device
	vglEnd();
}