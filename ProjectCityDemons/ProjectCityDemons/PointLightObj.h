#pragma once
#include <glm/glm.hpp>
#include <GLM\gtx\transform.hpp>
#include "Mesh.h"
#include "ShaderProgram.h"
#include <vector>
#include <iostream>
#include "Transform.h"
#include "object.h"

#include <windows.h>

//Will be the parent class for all other objects
class PointLightObj {

public:
	PointLightObj() {}
	PointLightObj(glm::vec3 _position, glm::vec3 _color, std::string _name, bool _active = true);


	virtual void draw(ShaderProgram shader, Transform CameraTransform);

	glm::vec3 getPosition();
	void setPosition(glm::vec3 pos);
	Object& getParent();
	void setParent(Object&);

	bool active;
	std::string name;

protected:
	glm::vec3 position;
	glm::vec3 color;
	Object* parent;
private:
};