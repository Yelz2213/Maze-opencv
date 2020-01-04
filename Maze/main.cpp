#include <GL\glut.h>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
using namespace cv;
#include "define.h"

void xyzToVertex(Vertex& vertex, float x, float y, float z) {
	vertex.x = x;
	vertex.y = y;
	vertex.z = z;
}

void vertexToVertex(Vertex& det, Vertex src) {
	det.x = src.x;
	det.y = src.y;
	det.z = src.z;
}

void vertexesToQuad(Quad4& quad, Vertex vertex0, Vertex vertex1, Vertex vertex2, Vertex vertex3) {
	vertexToVertex(quad.vertexes[0], vertex0);
	vertexToVertex(quad.vertexes[1], vertex1);
	vertexToVertex(quad.vertexes[2], vertex2);
	vertexToVertex(quad.vertexes[3], vertex3);
}

void printWall(Wall wall){
	//The 8 vertexs of a rectangle(wall)
	Vertex vertexes[8];
	xyzToVertex(vertexes[0], wall.x, wall.y, wall.z);
	xyzToVertex(vertexes[1], wall.x + wall.size, wall.y, wall.z);
	xyzToVertex(vertexes[2], wall.x + wall.size, wall.y + wall.size, wall.z);
	xyzToVertex(vertexes[3], wall.x, wall.y + wall.size, wall.z);
	xyzToVertex(vertexes[4], wall.x, wall.y, wall.z + wall.size);
	xyzToVertex(vertexes[5], wall.x + wall.size, wall.y, wall.z + wall.size);
	xyzToVertex(vertexes[6], wall.x + wall.size, wall.y + wall.size, wall.z + wall.size);
	xyzToVertex(vertexes[7], wall.x, wall.y + wall.size, wall.z + wall.size);

	//The surface of the rectangle(wall)
	Quad4 quads[6];
	vertexesToQuad(quads[0], vertexes[0], vertexes[1], vertexes[2], vertexes[3]);
	vertexesToQuad(quads[1], vertexes[0], vertexes[1], vertexes[5], vertexes[4]);
	vertexesToQuad(quads[2], vertexes[2], vertexes[3], vertexes[7], vertexes[6]);
	vertexesToQuad(quads[3], vertexes[1], vertexes[2], vertexes[6], vertexes[5]);
	vertexesToQuad(quads[4], vertexes[0], vertexes[3], vertexes[7], vertexes[4]);
	vertexesToQuad(quads[5], vertexes[4], vertexes[5], vertexes[6], vertexes[7]);
}

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