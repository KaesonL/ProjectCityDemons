#pragma once
#include <glm/glm.hpp>
#include <GLM\gtx\transform.hpp>
#include "mesh.h"
#include "texture.h"
#include <vector>
#include "Transform.h"


class Hitbox {

public:
	Hitbox() {}
	Hitbox(glm::vec3 _position, float _size, float _power, float _scaling, float _angle, unsigned int _activeFrames, float dmg, glm::vec3 _velocity);
	Hitbox(glm::vec3 _position, float _size);
	~Hitbox();

	void update(int t, glm::vec3 parent);
	glm::vec3 getPosition();
	glm::vec3 getVelocity();
	float getSize();
	void setPosition(glm::vec3 pos);
	bool isDone();
	float getScaling();
	float getPower();
	void setDone();
	Transform getTransform();
	bool spline;
	bool facingRight;
	bool projectile;
	glm::vec3 acceleration;
	std::vector<glm::vec3> curve;


	glm::vec3 catmull(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t)
	{
		return 0.5f * (
			t * t * t * (-p0 + 3.0f * p1 - 3.0f * p2 + p3) +
			t * t * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) +
			t * (-p0 + p2) +
			(2.0f * p1));
	}

	glm::vec4 catmull(glm::vec4 p0, glm::vec4 p1, glm::vec4 p2, glm::vec4 p3, float t)
	{
		return 0.5f * (
			t * t * t * (-p0 + 3.0f * p1 - 3.0f * p2 + p3) +
			t * t * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) +
			t * (-p0 + p2) +
			(2.0f * p1));
	}

	float getDamage() {
		return damage;
	}
	float angle;

private:
	//physics
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 globalPosition;

	//Hitbox Tings
	float power;
	float scaling;
	float size;
	float damage;
	bool hurtbox;
	//actions
	unsigned int activeFrames;
	unsigned int currentFrame;

};