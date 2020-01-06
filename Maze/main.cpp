#include <GL\glut.h>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
using namespace cv;
#include "define.h"

Color white, gray, green;
Light sunLight;
Map map;
Material wallMaterial, playerMaterial, endMaterial;
Texture wallTexture, endTexture;
Player player;
Camare fristPersonCamare, globalCamare;
ViewMode viewMode;
Vector2f mousePosition;
Wall playerCube;

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

void drawCube(Wall wall, bool isPlayer) {
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

	if (isPlayer) {
		glMaterialfv(GL_FRONT, GL_AMBIENT, playerMaterial.ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, playerMaterial.diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, playerMaterial.specular);
		glMaterialfv(GL_FRONT, GL_EMISSION, playerMaterial.emission);
		glMaterialfv(GL_FRONT, GL_SHININESS, &playerMaterial.shininess);

		glColor3f(green.r, green.g, green.b);

		for (int i = 0; i < 6; i++) {
			glBegin(GL_POLYGON);
			for (int j = 0; j < 4; j++) {
				glVertex3f(quads[i].vertexes[j].x, quads[i].vertexes[j].y, quads[i].vertexes[j].z);
			}
			glEnd();
		}
	}
	else {
		glColor3f(white.r, white.g, white.b);

		glMaterialfv(GL_FRONT, GL_AMBIENT, wallMaterial.ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, wallMaterial.diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, wallMaterial.specular);
		glMaterialfv(GL_FRONT, GL_EMISSION, wallMaterial.emission);
		glMaterialfv(GL_FRONT, GL_SHININESS, &wallMaterial.shininess);

		glEnable(GL_TEXTURE_2D);

		for (int i = 0; i < 6; i++) {

			glBindTexture(GL_TEXTURE_2D, wallTexture.id);
			glBegin(GL_POLYGON);

			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(quads[i].vertexes[0].x, quads[i].vertexes[0].y, quads[i].vertexes[0].z);

			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(quads[i].vertexes[1].x, quads[i].vertexes[1].y, quads[i].vertexes[1].z);

			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(quads[i].vertexes[2].x, quads[i].vertexes[2].y, quads[i].vertexes[2].z);

			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(quads[i].vertexes[3].x, quads[i].vertexes[3].y, quads[i].vertexes[3].z);

			glEnd();
		}
		glDisable(GL_TEXTURE_2D);
	}
}

void loadTexture(Texture &texture) {
	glGenTextures(1, &texture.id);
	glBindTexture(GL_TEXTURE_2D, texture.id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, texture.img.data);
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
				drawCube(wall, false);
			}
		}
	}

}

void drawCongratulation() {
	GLint viewPort[4];
	GLdouble modelView[16];
	GLdouble projection[16];

	glGetIntegerv(GL_VIEWPORT, viewPort);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	Vector2d pointOld[4];
	Vector3d pointNew[4];

	pointOld[0].x = WINDOW_SIZE_WIDTH / 4;
	pointOld[0].y = WINDOW_SIZE_HEIGHT / 4;
	pointOld[1].x = WINDOW_SIZE_WIDTH / 4;
	pointOld[1].y = WINDOW_SIZE_HEIGHT / 4 * 3;
	pointOld[2].x = WINDOW_SIZE_WIDTH / 4 * 3;
	pointOld[2].y = WINDOW_SIZE_HEIGHT / 4 * 3;
	pointOld[3].x = WINDOW_SIZE_WIDTH / 4 * 3;
	pointOld[3].y = WINDOW_SIZE_HEIGHT / 4;


	for (int i = 0; i < 4; i++) {
		gluUnProject(pointOld[i].x, pointOld[i].y, 0.1, modelView, projection, viewPort, &pointNew[i].x, &pointNew[i].y, &pointNew[i].z);
	}


	glColor3f(white.r, white.g, white.b);
	glMaterialfv(GL_FRONT, GL_AMBIENT, endMaterial.ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, endMaterial.diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, endMaterial.specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, endMaterial.emission);
	glMaterialfv(GL_FRONT, GL_SHININESS, &endMaterial.shininess);


	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, endTexture.id);


	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(pointNew[0].x, pointNew[0].y, pointNew[0].z);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(pointNew[1].x, pointNew[1].y, pointNew[1].z);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(pointNew[2].x, pointNew[2].y, pointNew[2].z);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(pointNew[3].x, pointNew[3].y, pointNew[3].z);
	glEnd();


	glDisable(GL_TEXTURE_2D);
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

	loadTexture(wallTexture);

	glLightfv(GL_LIGHT0, GL_POSITION, sunLight.position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, sunLight.ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sunLight.diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, sunLight.specular);

	glEnable(GL_LIGHT0);

	glEnable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);

	drawMaze(map);

	playerCube.x = player.y * MAP_BLOCK_LENGTH + MAP_BLOCK_LENGTH / 2 - PLAYER_CUBE_SIZE / 2;
	playerCube.y = map.height * MAP_BLOCK_LENGTH - player.x * MAP_BLOCK_LENGTH - MAP_BLOCK_LENGTH / 2 - PLAYER_CUBE_SIZE / 2;
	playerCube.z = 0;
	playerCube.size = PLAYER_CUBE_SIZE;
	drawCube(playerCube, true);

	if (player.x == endPosition.x && player.y == endPosition.y) {
		drawCongratulation();
	}

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

	playerMaterial.ambient[0] = 0.0f;
	playerMaterial.ambient[1] = 0.0f;
	playerMaterial.ambient[2] = 0.5f;
	playerMaterial.ambient[3] = 1.0f;
	playerMaterial.diffuse[0] = 0.0f;
	playerMaterial.diffuse[1] = 0.0f;
	playerMaterial.diffuse[2] = 0.5f;
	playerMaterial.diffuse[3] = 1.0f;
	playerMaterial.specular[0] = 0.0f;
	playerMaterial.specular[1] = 0.0f;
	playerMaterial.specular[2] = 0.8f;
	playerMaterial.specular[3] = 1.0f;
	playerMaterial.emission[0] = green.r;
	playerMaterial.emission[1] = green.g;
	playerMaterial.emission[2] = green.b;
	playerMaterial.emission[3] = 0;
	playerMaterial.shininess = 20;

	wallTexture.img = imread(".//texture.jpg");
	wallTexture.width = wallTexture.img.cols;
	wallTexture.height = wallTexture.img.rows;

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
}


void onSpecialKeyDown(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_F1:
		viewMode = VIEW_MODE_FRIST_PERSON;
		glutPostRedisplay();
		break;
	case GLUT_KEY_F2:
		viewMode = VIEW_MODE_GLOBAL;
		glutPostRedisplay();
		break;
	}

	if (viewMode == VIEW_MODE_FRIST_PERSON) {
		switch (key) {
		case GLUT_KEY_UP:
			switch (player.face) {
			case PLAYER_FACE_UP:
				if (player.x > 0) {
					if (map.blocks[player.x - 1][player.y] != MAP_BLOCK_CUBE) {
						player.x -= 1;
					}
				}
				break;
			case PLAYER_FACE_DOWN:
				if (player.x < map.height - 1) {
					if (map.blocks[player.x + 1][player.y] != MAP_BLOCK_CUBE) {
						player.x += 1;
					}
				}
				break;
			case PLAYER_FACE_LEFT:
				if (player.y > 0) {
					if (map.blocks[player.x][player.y - 1] != MAP_BLOCK_CUBE) {
						player.y -= 1;
					}
				}
				break;
			case PLAYER_FACE_RIGHT:
				if (player.y < map.width - 1) {
					if (map.blocks[player.x][player.y + 1] != MAP_BLOCK_CUBE) {
						player.y += 1;
					}
				}
				break;
			}
			break;
		case GLUT_KEY_DOWN:
			switch (player.face) {
			case PLAYER_FACE_UP:
				if (player.x < map.height - 1) {
					if (map.blocks[player.x + 1][player.y] != MAP_BLOCK_CUBE) {
						player.x += 1;
					}
				}
				break;
			case PLAYER_FACE_DOWN:
				if (player.x > 0) {
					if (map.blocks[player.x - 1][player.y] != MAP_BLOCK_CUBE) {
						player.x -= 1;
					}
				}
				break;
			case PLAYER_FACE_LEFT:
				if (player.y < map.width - 1) {
					if (map.blocks[player.x][player.y + 1] != MAP_BLOCK_CUBE) {
						player.y += 1;
					}
				}
				break;
			case PLAYER_FACE_RIGHT:
				if (player.y > 0) {
					if (map.blocks[player.x][player.y - 1] != MAP_BLOCK_CUBE) {
						player.y -= 1;
					}
				}
				break;
			}
			break;
		case GLUT_KEY_LEFT:
			switch (player.face) {
			case PLAYER_FACE_UP:
				player.face = PLAYER_FACE_LEFT;
				break;
			case PLAYER_FACE_DOWN:
				player.face = PLAYER_FACE_RIGHT;
				break;
			case PLAYER_FACE_LEFT:
				player.face = PLAYER_FACE_DOWN;
				break;
			case PLAYER_FACE_RIGHT:
				player.face = PLAYER_FACE_UP;
				break;
			}
			break;
		case GLUT_KEY_RIGHT:
			switch (player.face) {
			case PLAYER_FACE_UP:
				player.face = PLAYER_FACE_RIGHT;
				break;
			case PLAYER_FACE_DOWN:
				player.face = PLAYER_FACE_LEFT;
				break;
			case PLAYER_FACE_LEFT:
				player.face = PLAYER_FACE_UP;
				break;
			case PLAYER_FACE_RIGHT:
				player.face = PLAYER_FACE_DOWN;
				break;
			}
			break;
		}
		setFristPersonCamareByPlayer();
		glutPostRedisplay();
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

	glutSpecialFunc(onSpecialKeyDown);

	// Loop
	glutMainLoop();

	return 0;
}