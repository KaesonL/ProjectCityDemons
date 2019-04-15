#pragma once
#include <GL\glew.h>
#include <string>

class Texture
{
public:
	Texture() = default;
	~Texture();

	bool Load(const std::string &file);
	bool Unload();

	void SetNearestFilter();

	void Bind();
	void Bind(int texSlot);
	void UnBind();
	void UnBind(int texSlot);

	//- The handle to the texture object
	GLuint TextObj = 0;

private:
	GLuint _TexHandle;

	std::string filename;

	GLenum _InternalFormat;
	GLenum _Target;
	GLenum _FilterMag = GL_LINEAR; // Filter parameter
	GLenum _FilterMin = GL_LINEAR_MIPMAP_LINEAR;// GL_LINEAR;
	GLenum _WrapU = GL_REPEAT; // Per axis Wrap parameter
	GLenum _WrapV = GL_REPEAT;

	int sizeX;
	int sizeY;
	int channels;
};