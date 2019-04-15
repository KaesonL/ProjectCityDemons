#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <fstream>
#include <iostream>

using namespace std;

//basic motion motor for generating wind when moving
struct Motor {
	bool active = false;
	float weight = 1.0f;
	glm::vec2 direction = glm::vec2(0.0f, 0.0f);
	glm::vec2 location = glm::vec2(0.0f, 0.0f);

	//physics calculation
	glm::vec2 getForce(glm::vec2 position) {
		float dist = glm::distance(position, location) + 0.01f;
		return (direction * weight) /dist;
	};
};

//ill abstract this better later if i have time			-John

//Field of vectors for both players
class VectorField {
public:
	VectorField();
	~VectorField();

	void init();

	//returns the total weight of all fields, or 0
	glm::vec2 totalWeight(glm::vec2 position);

	void addSwing(int player, glm::vec2 dir);

	void update(glm::vec2 p1, glm::vec2 p1Dir, glm::vec2 p2, glm::vec2 p2Dir);

	float range;
	bool used;

private:
	Motor p1Legs;
	Motor p2Legs;
	Motor p1Body;
	Motor p2Body;
	Motor p1Head;
	Motor p2Head;
	Motor p1Swing;
	Motor p2Swing;

	float headOffSet = 4.0f;
	float bodyOffset = 2.0f;
	float movementWeight = 1.5f;

};