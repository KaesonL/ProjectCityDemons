#include "FrameBuffer.h"

/*
Credit to:
Emilian Cioca
Gil Robern
Jessica Le
*/

//The int represents the number of colour attachments the class should expect to have, to allocate the right amount of space
FrameBuffer::FrameBuffer(unsigned numColorAttachments)
{
	_NumColorAttachments = numColorAttachments;

	glGenFramebuffers(1, &_FBO);

	_ColorAttachments = new GLuint[_NumColorAttachments];

	//_Bufs is required as a parameter for glDrawBuffers()
	_Bufs = new GLenum[_NumColorAttachments]; //_Bufs is dynamically allocating memory
	for (unsigned int i = 0; i < _NumColorAttachments; i++)
	{
		_Bufs[i] = GL_COLOR_ATTACHMENT0 + i;
	}
}

FrameBuffer::~FrameBuffer()
{
	Unload();
}

void FrameBuffer::InitDepthTexture(unsigned width, unsigned height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, _FBO);

	//Create depth texture
	glGenTextures(1, &_DepthAttachment);
	glBindTexture(GL_TEXTURE_2D, _DepthAttachment);
	//Inteded for use with FBOs
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, width, height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//Do not use GL_CLAMP it is deprecated
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//Bind texture to the FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _DepthAttachment, 0);
	
	//Unbind the frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
}

void FrameBuffer::InitColorTexture(unsigned index, unsigned width, unsigned height, GLint internalFormat, GLint filter, GLint wrap)
{
	glBindFramebuffer(GL_FRAMEBUFFER, _FBO);

	//Create depth texture
	glGenTextures(1, &_ColorAttachments[index]);
	glBindTexture(GL_TEXTURE_2D, _ColorAttachments[index]);
	//Inteded for use with FBOs
	glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, width, height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	//Do not use GL_CLAMP it is deprecated
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

	//Bind texture to the FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, _ColorAttachments[index], 0);

	//Unbind the frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
}

bool FrameBuffer::CheckFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _FBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Unload();
		return false;
	}

	return true;
}

//- Clears all OpenGL memory
void FrameBuffer::Unload()
{
	if (_Bufs != nullptr)
	{
		//Make sure to have delete[] on an array of data
		delete[] _Bufs;
		_Bufs = nullptr;
	}

	if (_ColorAttachments != nullptr)
	{
		for (unsigned int i = 0; i < _NumColorAttachments; i++)
		{
			glDeleteTextures(1, &_ColorAttachments[i]);
		}
		delete[] _ColorAttachments;
		_ColorAttachments = nullptr;
	}

	if (_DepthAttachment != GL_NONE)
	{
		glDeleteTextures(1, &_DepthAttachment);
		_DepthAttachment = GL_NONE;
	}

	_NumColorAttachments = 0;
}

//- Clears all attached textures
void FrameBuffer::Clear() // We are creating the clear function to be more specific for frame buffers
{
	GLbitfield temp = 0;

	if (_DepthAttachment != GL_NONE)
	{
		//Using the bitwise or operator the bits in temp will be "or'd" with the bits in the other variable, the result will be put back into temp
		temp = temp | GL_DEPTH_BUFFER_BIT; //We will not overlap or lose any of these bits, allows us to compact all the data in one memory space
	}

	if (_ColorAttachments != nullptr)
	{
		temp = temp | GL_COLOR_BUFFER_BIT;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
	glClear(temp);
	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
}

void FrameBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
	//Does not draw anything, it allows you to activate the colour buffers inside the FBO that you want to be valid for drawing
	glDrawBuffers(_NumColorAttachments, _Bufs);
}

void FrameBuffer::UnBind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
}

void FrameBuffer::MoveToBackBuffer(int windowWidth, int windowHeight)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _FBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GL_NONE);

	//You can specify depth or color
	glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	//This will unbind both at the same time
	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
}

//Very important
GLuint FrameBuffer::GetDepthHandle() const
{
	return _DepthAttachment;
}

//*****Make sure to add error checking for the array on our own and to make sure to do it in other functions*****
GLuint FrameBuffer::GetColorHandle(unsigned index) const
{
	return _ColorAttachments[index];
}