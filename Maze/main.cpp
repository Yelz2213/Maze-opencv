#include <GL\glut.h>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
using namespace cv;
#include "define.h"

Color white, gray, green;
Map map;

void xyzToVertex(Vertex& vertex, float x, float y, float z) {
	// set position of vertex
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


void drawWall(Wall wall){
	//The 8 vertexs of a wall
	Vertex vertexes[8];
	xyzToVertex(vertexes[0], wall.x, wall.y, wall.z);
	xyzToVertex(vertexes[1], wall.x + wall.size, wall.y, wall.z);
	xyzToVertex(vertexes[2], wall.x + wall.size, wall.y + wall.size, wall.z);
	xyzToVertex(vertexes[3], wall.x, wall.y + wall.size, wall.z);
	xyzToVertex(vertexes[4], wall.x, wall.y, wall.z + wall.size);
	xyzToVertex(vertexes[5], wall.x + wall.size, wall.y, wall.z + wall.size);
	xyzToVertex(vertexes[6], wall.x + wall.size, wall.y + wall.size, wall.z + wall.size);
	xyzToVertex(vertexes[7], wall.x, wall.y + wall.size, wall.z + wall.size);

	//The surface of the wall
	Quad4 quads[6];
	vertexesToQuad(quads[0], vertexes[0], vertexes[1], vertexes[2], vertexes[3]);
	vertexesToQuad(quads[1], vertexes[0], vertexes[1], vertexes[5], vertexes[4]);
	vertexesToQuad(quads[2], vertexes[2], vertexes[3], vertexes[7], vertexes[6]);
	vertexesToQuad(quads[3], vertexes[1], vertexes[2], vertexes[6], vertexes[5]);
	vertexesToQuad(quads[4], vertexes[0], vertexes[3], vertexes[7], vertexes[4]);
	vertexesToQuad(quads[5], vertexes[4], vertexes[5], vertexes[6], vertexes[7]);
}

MapPosition startPosition, endPosition;

void drawMaze(Map map) {
	Wall wall;
	wall.size = MAP_BLOCK_LENGTH;
	for (int i = 0; i < map.width; i++) {
		for (int j = 0; j < map.height; j++) {
			if (map.blocks[i][j] == MAP_BLOCK_CUBE) {
				wall.x = j * wall.size;
				wall.y = map.height * wall.size - (i + 1) * wall.size;
				wall.z = 0;
				drawWall(wall);
			}
		}
	}

}

void render() {
	glClearColor(gray.r, gray.g, gray.b, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, 10000);

	drawMaze(map);
}

void init() {

	gray.r = 0.3f;
	gray.g = 0.3f;
	gray.b = 0.3f;

	map.width = MAP1_WIDTH;
	map.height = MAP1_HEIGHT;

	for (int i = 0; i < map.width; i++)
		for (int j = 0; j < map.height; j++)
			map.blocks[i][j] = MAP1_BLOCKS[i][j];

	for (int i = 0; i < map.width; i++) {
		for (int j = 0; j < map.height; j++) {
			if (map.blocks[i][j] == MAP_BLOCK_START) {
				startPosition.x = i;
				startPosition.y = j;
			}

			if (map.blocks[i][j] == MAP_BLOCK_END) {
				endPosition.x = i;
				endPosition.y = j;
			}
		}
	}
}

int main(int argc, char *argv[]) {
	init();

	// init glut
	glutInit(&argc, argv);

	// init window
	glutInitWindowPosition(WINDOW_POSITION_X, WINDOW_POSITION_Y);
	glutInitWindowSize(WINDOW_SIZE_WIDTH, WINDOW_SIZE_HEIGHT);
	glutCreateWindow("Maze");

	glutDisplayFunc(render);

	// Loop
	glutMainLoop();

	return 0;
}