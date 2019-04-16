#pragma once
#include <GL\glew.h>

/*
Credit to:
Emilian Cioca
Gil Robern
Jessica Le
*/

//Frame bufferes can only have one depth texture but can have multiple colour textures
class FrameBuffer
{
public:
	//This give a compilation erro rif you use the default constructor
	FrameBuffer() = delete;
	//The int represents the number of colour attachments the class should expect to have, to allocate the right amount of space
	FrameBuffer(unsigned numColorAttachments);
	~FrameBuffer();

	void InitDepthTexture(unsigned width, unsigned height);
	void InitColorTexture(unsigned index, unsigned width, unsigned height, GLint internalFormat, GLint filter, GLint wrap);
	bool CheckFBO();

	//- Clears all OpenGL memory
	void Unload();

	//- Clears all attached textures
	void Clear();

	void Bind();
	void UnBind();

	void MoveToBackBuffer(int windowWidth, int windowHeight);

	//Very important
	GLuint GetDepthHandle() const;
	GLuint GetColorHandle(unsigned index) const;

private:
	GLuint _FBO = GL_NONE;
	GLuint _DepthAttachment = GL_NONE;
	GLuint *_ColorAttachments = nullptr;
	//A variabe to hold the parameters for afunction later on...?
	GLenum *_Bufs = nullptr;

	unsigned int _NumColorAttachments = 0;

};