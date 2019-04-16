#include "Texture.h"
#include "SOIL\SOIL.h"
#include <iostream>

/*
Credit to:
Emilian Cioca
Gil Robern
Jessica Le
*/

Texture::~Texture()
{
	Unload();
}

bool Texture::Load(const std::string &file)
{
	/*TextObj = SOIL_load_OGL_texture(file.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (TextObj == 0)
	{
		std::cout << "Texture failed to load.\n" << SOIL_last_result() << "\n";
		return false;
	}

	//Send the data to OpenGL
	glBindTexture(GL_TEXTURE_2D, TextObj); //BInd the new texture

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//S and T are some sort of axes?
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	return true;*/
	
	//this->filename = "./Assets/Textures/" + file; //We should use this later
	this->filename = file;

	unsigned char* textureData;
	textureData = SOIL_load_image((this->filename).c_str(),
		&(this->sizeX), &(this->sizeY), &(this->channels), SOIL_LOAD_RGBA);

	if (this->sizeX == 0 || this->sizeY == 0 || this->channels == 0)
	{
		//SAT_DEBUG_LOG_ERROR("TEXTURE BROKE: %s", this->filename.c_str());
		return false;
	}

	// If the texture is 2D, set it to be a 2D texture;
	_Target = GL_TEXTURE_2D;
	_InternalFormat = GL_RGBA8;

	glGenTextures(1, &this->_TexHandle);
	this->Bind();
	glTextureStorage2D(this->_TexHandle, 1, this->_InternalFormat, this->sizeX, this->sizeY);

	glTextureSubImage2D(this->_TexHandle, 0, // We are editing the first and only layer in memory
		0, 0, // No offset
		this->sizeX, this->sizeY, // the dimensions of our image loaded
		GL_RGBA, GL_UNSIGNED_BYTE, // Data format and type
		textureData); // Pointer to the texture data

	glTextureParameteri(this->_TexHandle, GL_TEXTURE_MAG_FILTER, this->_FilterMag);
	glTextureParameteri(this->_TexHandle, GL_TEXTURE_MIN_FILTER, this->_FilterMin);
	glTextureParameteri(this->_TexHandle, GL_TEXTURE_WRAP_S, this->_WrapU);
	glTextureParameteri(this->_TexHandle, GL_TEXTURE_WRAP_T, this->_WrapV);

	this->UnBind();
	SOIL_free_image_data(textureData);
	return true;
}

bool Texture::Unload()
{
	if (this->_TexHandle != 0)
	{
		//Remove date from GPU
		glDeleteTextures(1, &this->_TexHandle);
		return true;
	}
	return false;
}

void Texture::SetNearestFilter()
{
	glBindTexture(GL_TEXTURE_2D, this->_TexHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

void Texture::Bind()
{
	glBindTexture(this->_Target, this->_TexHandle);
}

void Texture::Bind(int texSlot)
{
	glActiveTexture(GL_TEXTURE0 + texSlot);
	this->Bind();
}

void Texture::UnBind()
{
	glBindTexture(this->_Target, GL_NONE);
}

void Texture::UnBind(int texSlot)
{
	glActiveTexture(GL_TEXTURE0 + texSlot);
	this->UnBind();
}
