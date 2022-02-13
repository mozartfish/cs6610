#include<iostream>
#include <GL/freeglut.h>

/*
* This is an implementation of an animated background windows using FreeGlut and OpenGL.
* Author: Pranav Rajan
* Version: January 21, 2022
*/

// global variable for incrementing the red color value
float red = 0.0;
void myIdle() {
	// Example Animation 
	//std::cout << "red " << red << std::endl;
	glClearColor(red, 0, 0, 0);
	red += 0.02;
	Sleep(100);
	if (red > 1.0) {
		red = 0.0;
	}

	glutPostRedisplay();
}

void myDisplay() {
	// Clear the viewport
	glClear(GL_COLOR_BUFFER_BIT);

	// OpenGL draw calls here

	// Swap buffers
	glutSwapBuffers();

}

void myKeyboard(unsigned char key, int x, int y) {
	// Handle keyboard input here
	// GLUT uses ascii for key events 
	switch (key) {
	case 27:
		glutLeaveMainLoop();
		break;
	}
	return;
}

int main(int argc, char** argv) {
	// GLUT Initialization
	glutInit(&argc, argv);

	// Create a window 
	glutInitWindowSize(1920, 1080);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("Window Title");

	// OpenGL Initialization
	glClearColor(0, 0, 0, 0); // RGBA Values between 0-1

	// Register display callback function 
	glutDisplayFunc(myDisplay);

	// Register special keyboard callback
	glutKeyboardFunc(myKeyboard);

	// Register Idle Function 
	glutIdleFunc(myIdle);

	// Create buffers
	// Create textures
	// Compile Shaders

	// Other Initialization

	glutMainLoop(); // Call main loop
	return 0; // Exit when main loop is done 
}
