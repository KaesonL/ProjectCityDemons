#include "Utilities.h"
#include <GL\glew.h>

//A macro that takes i and adds it to a nullptr to get essentially the equivalent pointer of what that value would have been
#define BUFFER_OFFSET(i) ((char *)0 + (i))

GLuint _FullScreenQuadVAO = GL_NONE;
GLuint _FullScreenQuadVBO = GL_NONE;

void InitFullScreenQuad()
{
	float VBO_DATA[] = 
	{
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,

		1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,

		1.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f
	};

	int vertexSize = 6 * 3 * sizeof(float);
	int texCoordSize = 6 * 2 * sizeof(float);

	glGenVertexArrays(1, &_FullScreenQuadVAO);
	glBindVertexArray(_FullScreenQuadVAO);

	glEnableVertexAttribArray(0); //Vertices
	glEnableVertexAttribArray(1); //UV Coordinates

	glGenBuffers(1, &_FullScreenQuadVBO);

	glBindBuffer(GL_ARRAY_BUFFER, _FullScreenQuadVBO);
	glBufferData(GL_ARRAY_BUFFER, vertexSize + texCoordSize, VBO_DATA, GL_STATIC_DRAW);

	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertexSize));

	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	glBindVertexArray(GL_NONE);
}

void DrawFullScreenQuad()
{
	glBindVertexArray(_FullScreenQuadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(GL_NONE);
}