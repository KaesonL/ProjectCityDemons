#include "PointLightObj.h"

PointLightObj::PointLightObj(glm::vec3 _position, glm::vec3 _color, std::string _name, bool _active)
{
	position = _position;
	color = _color;
	name = _name;
	active = _active;
}

void PointLightObj::draw(ShaderProgram shader, Transform camera)
{
	glm::vec3 globalPosition;
	if (parent) {
		globalPosition = parent->getPosition() + position;
	}
	else {
		globalPosition = position;
	}
	glm::vec4 lightPos = camera.GetInverse().matData * glm::vec4(globalPosition.x, globalPosition.y, globalPosition.z, 1.0f);
	shader.SendUniform("uLightPosition", glm::vec3(lightPos));
	shader.SendUniform("uLightColor", color);

}

glm::vec3 PointLightObj::getPosition()
{
	return position;
}

void PointLightObj::setPosition(glm::vec3 pos)
{
	position = pos;
}

Object & PointLightObj::getParent()
{
	return *parent;
}

void PointLightObj::setParent(Object & _parent)
{
	parent = &_parent;
}
