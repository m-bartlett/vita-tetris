// Drawing a rotating cube
#include <vitasdk.h>
#include <vitaGL.h>
#include <math.h>

uint16_t indices[] = {
	 // 0, 2, 1, 1, 2, 3, // Back
	 4, 5, 6, 5, 7, 6, // Front
	 8, 10,9, 9,10,11, // Left
	12,13,14,13,15,14, // Right
	16,17,18,17,19,18, // Bottom
	20,22,21,21,22,23  // Top
};

int main(){
	// Initializing graphics device
	vglInit(0x800000);
	
	// Enabling V-Sync
	vglWaitVblankStart(GL_TRUE);
	
	float vertex_rgba[] = { 1,1,1,.5,     1,0,0,.5,     0,0,1,.5,     0,1,0,.5,     
                            1,1,1,.5,     1,0,0,.5,     0,0,1,.5,     0,1,0,.5,     
                            1,1,1,.5,     1,0,0,.5,     0,0,1,.5,     0,1,0,.5,     
                            1,1,1,.5,     1,0,0,.5,     0,0,1,.5,     0,1,0,.5,     
                            1,1,1,.5,     1,0,0,.5,     0,0,1,.5,     0,1,0,.5,     
                            1,1,1,.5,     1,0,0,.5,     0,0,1,.5,     0,1,0,.5   };


	float vertex_xyz[] = { -1,-1,-1,   1,-1,-1,  -1, 1,-1,   1, 1,-1,   // front
					       -1,-1, 1,   1,-1, 1,  -1, 1, 1,   1, 1, 1,   // back
					       -1,-1,-1,  -1, 1,-1,  -1,-1, 1,  -1, 1, 1,   // left
					        1,-1,-1,   1, 1,-1,   1,-1, 1,   1, 1, 1,   // right
					       -1,-1,-1,   1,-1,-1,  -1,-1, 1,   1,-1, 1,   // top
					       -1, 1,-1,   1, 1,-1,  -1, 1, 1,   1, 1, 1 }; // bottom
	
	// Setting clear color
	glClearColor (0.0f, 0.0f, 0.0f, 0.0f);

	// Initializing mvp matrix with a perspective full screen matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0f, 960.f/544.0f, 0.01f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -3.0f); // Centering the cube
	
	// Enabling depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);

	glEnable( GL_BLEND );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	for (int i = 0; i < 3*4*6; ++i) vertex_xyz[i] /= 2.0;

	
	// Main loop
	for (;;){
		// Clear color and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Drawing our cube with vertex arrays
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, vertex_xyz);
		glColorPointer(4, GL_FLOAT, 0, vertex_rgba);


		SceCtrlData pad;
		sceCtrlPeekBufferPositive(0, &pad, 1);
		if (pad.buttons & SCE_CTRL_LEFT)  glRotatef( 1, 0, 1, 0);
		if (pad.buttons & SCE_CTRL_RIGHT) glRotatef(-1, 0, 1, 0);
		if (pad.buttons & SCE_CTRL_UP)    glRotatef( 1, 1, 0, 0);
		if (pad.buttons & SCE_CTRL_DOWN)  glRotatef(-1, 1, 0, 0);

		glDrawElements(GL_TRIANGLES, 6*6, GL_UNSIGNED_SHORT, indices);
		// glDrawArrays(GL_TRIANGLES, 0, 6*6); 
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		
		// Performing buffer swap
		vglSwapBuffers(GL_FALSE);
	}
	
	// Terminating graphics device
	vglEnd();
}