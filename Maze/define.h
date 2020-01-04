#pragma once

#define WINDOW_POSITION_X 100
#define WINDOW_POSITION_Y 100
#define WINDOW_SIZE_WIDTH 600
#define WINDOW_SIZE_HEIGHT 600

struct Vertex {
	GLfloat x;
	GLfloat y;
	GLfloat z;
};

struct Quad4 {
	Vertex vertexes[4];
};

struct Wall {
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLfloat size;
};