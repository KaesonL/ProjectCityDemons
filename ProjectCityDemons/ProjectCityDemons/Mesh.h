#pragma once
#include <string>
#include <vector>
#include <GL\glew.h>
#include <glm/glm.hpp>
#include <GLM\gtx\transform.hpp>

struct MeshFaceFloat;

class Mesh
{
public:
	Mesh();
	~Mesh();

	//- Load a mesh, and send it to OpenGL
	//bool LoadFromFile(const std::string &file);
	bool LoadFromFile(const std::vector<std::string> &files);

	//- Release data from OpenGL (VRAM)
	void Unload();

	unsigned int GetNumFaces() const;
	unsigned int GetNumVertices() const;
	bool CreateBinary(const std::string &file);


	//OpenGL buffers and objects
	GLuint* VBO_Vertices = 0;
	GLuint* VBO_UVs = 0;
	GLuint* VBO_Normals = 0;
	GLuint VAO = 0;

private:
	unsigned int _NumFaces = 0;
	unsigned int _NumVertices = 0;
	unsigned int _NumFrames = 0;

};