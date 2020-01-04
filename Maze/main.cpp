#include <GL\glut.h>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
using namespace cv;
#include "define.h"

void init() {

}

int main(int argc, char *argv[]) {
	init();

	// init glut
	glutInit(&argc, argv);

	// init window
	glutInitWindowPosition(WINDOW_POSITION_X, WINDOW_POSITION_Y);
	glutInitWindowSize(WINDOW_SIZE_WIDTH, WINDOW_SIZE_HEIGHT);
	glutCreateWindow("Maze");

	// Loop
	glutMainLoop();

	return 0;
}