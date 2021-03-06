#pragma once
#include <glm/glm.hpp>
#include <GLM\gtx\transform.hpp>
#include "Mesh.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include <vector>
#include <iostream>
#include "Transform.h"

/*
Credit to:
Emilian Cioca
Gil Robern
Jessica Le
*/

class Object {

public:
	Object() {}
	Object(const std::string& body, const std::string& texture, std::string _name, bool _blend = false);
	~Object();

	virtual void update(int t, std::vector<bool> inputs);

	virtual void draw(ShaderProgram GBufferPass, float dt);

	glm::vec3 getPosition();
	void setPosition(glm::vec3 pos);
	void setScale(glm::vec3 scale);
	void setScale(float scale);
	void RotateY(float rot);
	void RotateX(float rot);


	std::string name;
	//model
	Transform transform;
	Mesh body;
	Texture texture;
	bool blending;

	bool hide = false;
protected:

	//physics
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 force;
	float mass;

	//scaling
	float scaleX;
	float scaleY;
	float scaleZ;
	//rotation
	float rotX;
	float rotY;
	float rotZ;

private:
};