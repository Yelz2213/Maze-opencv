#include <GL\glut.h>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
using namespace cv;
#include "define.h"

Color white, gray, green;
Light sunLight;
Map map;
Material wallMaterial, playerMaterial;
Texture wall;
Player player;
Camare fristPersonCamare, globalCamare;
ViewMode viewMode;
Vector2f mousePosition;

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

void setCamarePosition(Camare& camare, GLfloat x, GLfloat y, GLfloat z) {
	camare.position[0] = x;
	camare.position[1] = y;
	camare.position[2] = z;
}

void setCamareLookAt(Camare& camare, GLfloat x, GLfloat y, GLfloat z) {
	camare.lookAt[0] = x;
	camare.lookAt[1] = y;
	camare.lookAt[2] = z;
}

void setCamareDirection(Camare& camare, GLfloat x, GLfloat y, GLfloat z) {
	camare.direction[0] = x;
	camare.direction[1] = y;
	camare.direction[2] = z;
}

void setFristPersonCamareByPlayer() {

	setCamarePosition(
		fristPersonCamare,
		player.y * MAP_BLOCK_LENGTH + MAP_BLOCK_LENGTH / 2,
		map.height * MAP_BLOCK_LENGTH - player.x * MAP_BLOCK_LENGTH - MAP_BLOCK_LENGTH / 2,
		MAP_BLOCK_LENGTH / 2
	);

	switch (player.face) {
	case PLAYER_FACE_UP:
		setCamareLookAt(
			fristPersonCamare,
			fristPersonCamare.position[0],
			fristPersonCamare.position[1] + MAP_BLOCK_LENGTH * VIEW_STEP_LENGTH_XY,
			fristPersonCamare.position[2] - MAP_BLOCK_LENGTH * VIEW_STEP_LENGTH_Z * 2
		);
		break;
	case PLAYER_FACE_DOWN:
		setCamareLookAt(
			fristPersonCamare,
			fristPersonCamare.position[0],
			fristPersonCamare.position[1] - MAP_BLOCK_LENGTH * VIEW_STEP_LENGTH_XY,
			fristPersonCamare.position[2] - MAP_BLOCK_LENGTH * VIEW_STEP_LENGTH_Z * 2
		);
		break;
	case PLAYER_FACE_LEFT:
		setCamareLookAt(
			fristPersonCamare,
			fristPersonCamare.position[0] - MAP_BLOCK_LENGTH * VIEW_STEP_LENGTH_XY,
			fristPersonCamare.position[1],
			fristPersonCamare.position[2] - MAP_BLOCK_LENGTH * VIEW_STEP_LENGTH_Z * 2
		);
		break;
	case PLAYER_FACE_RIGHT:
		setCamareLookAt(
			fristPersonCamare,
			fristPersonCamare.position[0] + MAP_BLOCK_LENGTH * VIEW_STEP_LENGTH_XY,
			fristPersonCamare.position[1],
			fristPersonCamare.position[2] - MAP_BLOCK_LENGTH * VIEW_STEP_LENGTH_Z * 2
		);
		break;
	}

	setCamareDirection(fristPersonCamare, 0, 0, 1);
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

	//The edge of the wall
	Quad4 quads[6];
	vertexesToQuad(quads[0], vertexes[0], vertexes[1], vertexes[2], vertexes[3]);
	vertexesToQuad(quads[1], vertexes[0], vertexes[1], vertexes[5], vertexes[4]);
	vertexesToQuad(quads[2], vertexes[2], vertexes[3], vertexes[7], vertexes[6]);
	vertexesToQuad(quads[3], vertexes[1], vertexes[2], vertexes[6], vertexes[5]);
	vertexesToQuad(quads[4], vertexes[0], vertexes[3], vertexes[7], vertexes[4]);
	vertexesToQuad(quads[5], vertexes[4], vertexes[5], vertexes[6], vertexes[7]);

	glColor3f(white.r, white.g, white.b);

	glMaterialfv(GL_FRONT, GL_AMBIENT, wallMaterial.ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, wallMaterial.diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, wallMaterial.specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, wallMaterial.emission);
	glMaterialfv(GL_FRONT, GL_SHININESS, &wallMaterial.shininess);


	for (int i = 0; i < 6; i++) {
		glBegin(GL_QUADS);
		for (int j = 0; j < 4; j++) {
			glVertex3f(quads[i].vertexes[j].x, quads[i].vertexes[j].y, quads[i].vertexes[j].z);
		}
		glEnd();
	}

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

	switch (viewMode) {
	case VIEW_MODE_GLOBAL:
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(
			globalCamare.position[0], globalCamare.position[1], globalCamare.position[2],
			globalCamare.lookAt[0], globalCamare.lookAt[1], globalCamare.lookAt[2],
			globalCamare.direction[0], globalCamare.direction[1], globalCamare.direction[2]);
		break;

	case VIEW_MODE_FRIST_PERSON:
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(
			fristPersonCamare.position[0], fristPersonCamare.position[1], fristPersonCamare.position[2],
			fristPersonCamare.lookAt[0], fristPersonCamare.lookAt[1], fristPersonCamare.lookAt[2],
			fristPersonCamare.direction[0], fristPersonCamare.direction[1], fristPersonCamare.direction[2]);
		break;
	}


	glLightfv(GL_LIGHT0, GL_POSITION, sunLight.position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, sunLight.ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sunLight.diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, sunLight.specular);

	glEnable(GL_LIGHT0);

	glEnable(GL_LIGHTING);


	glEnable(GL_DEPTH_TEST);


	drawMaze(map);

	glutSwapBuffers();
}

void init() {
	// define the color parameter
	white.r = 1.0f;
	white.g = 1.0f;
	white.b = 1.0f;

	gray.r = 0.3f;
	gray.g = 0.3f;
	gray.b = 0.3f;

	green.r = 0.4f;
	green.g = 1.0f;
	green.b = 0.6f;

	//The setting of light
	sunLight.position[0] = 0.0f;
	sunLight.position[1] = 0.0f;
	sunLight.position[2] = 0.0f;
	sunLight.position[3] = 1.0f;
	sunLight.ambient[0] = 0.0f;
	sunLight.ambient[1] = 0.0f;
	sunLight.ambient[2] = 0.0f;
	sunLight.ambient[3] = 1.0f;
	sunLight.diffuse[0] = 1.0f;
	sunLight.diffuse[1] = 1.0f;
	sunLight.diffuse[2] = 1.0f;
	sunLight.diffuse[3] = 1.0f;
	sunLight.specular[0] = 1.0f;
	sunLight.specular[1] = 1.0f;
	sunLight.specular[2] = 1.0f;
	sunLight.specular[3] = 1.0f;

	//reading the map from the "Map map" 's array.
	map.width = MAP1_WIDTH;
	map.height = MAP1_HEIGHT;
	for (int i = 0; i < map.width; i++)
		for (int j = 0; j < map.height; j++)
			map.blocks[i][j] = MAP1_BLOCKS[i][j];

	// define the material of wall
	wallMaterial.ambient[0] = 0.0f;
	wallMaterial.ambient[1] = 0.0f;
	wallMaterial.ambient[2] = 0.5f;
	wallMaterial.ambient[3] = 1.0f;
	wallMaterial.diffuse[0] = 0.0f;
	wallMaterial.diffuse[1] = 0.0f;
	wallMaterial.diffuse[2] = 0.5f;
	wallMaterial.diffuse[3] = 1.0f;
	wallMaterial.specular[0] = 0.0f;
	wallMaterial.specular[1] = 0.0f;
	wallMaterial.specular[2] = 0.8f;
	wallMaterial.specular[3] = 1.0f;
	wallMaterial.emission[0] = white.r;
	wallMaterial.emission[1] = white.g;
	wallMaterial.emission[2] = white.b;
	wallMaterial.emission[3] = 0;
	wallMaterial.shininess = 20;

	// Using loop to find out the start and end position from the map array
	for (int i = 0; i < map.width; i++) {
		for (int j = 0; j < map.height; j++) {
			//i = row, j = column
			//start position
			if (map.blocks[i][j] == MAP_BLOCK_START) {
				startPosition.x = i;
				startPosition.y = j;
			}

			//end position
			if (map.blocks[i][j] == MAP_BLOCK_END) {
				endPosition.x = i;
				endPosition.y = j;
			}
		}
	}

	player.x = startPosition.x;
	player.y = startPosition.y;

	player.face = PLAYER_FACE_UP;

	viewMode = VIEW_MODE_GLOBAL;

	setCamarePosition(globalCamare, GLOBAL_CAMARE_POSITION_X, GLOBAL_CAMARE_POSITION_Y, GLOBAL_CAMARE_POSITION_Z);
	setCamareLookAt(globalCamare, GLOBAL_CAMARE_LOOKAT_X, GLOBAL_CAMARE_LOOKAT_Y, GLOBAL_CAMARE_LOOKAT_Z);
	setCamareDirection(globalCamare, GLOBAL_CAMARE_DIRECTION_X, GLOBAL_CAMARE_DIRECTION_Y, GLOBAL_CAMARE_DIRECTION_Z);

	setFristPersonCamareByPlayer();

	mousePosition.x = 0.0f;
	mousePosition.y = 0.0f;

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