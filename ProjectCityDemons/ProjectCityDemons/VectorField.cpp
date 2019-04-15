#include "VectorField.h"

VectorField::VectorField()
{
	range = 2;
	used = false;
}

VectorField::~VectorField()
{
}

void VectorField::init()
{
	//init vector fields
	p1Legs.active = false;
	p1Legs.weight = movementWeight;
	p1Legs.direction = glm::vec2(0.0f,0.0f);
	p1Legs.location = glm::vec2(0.0f, 0.0f);

	p2Legs.active = false;
	p2Legs.weight = movementWeight;
	p2Legs.direction = glm::vec2(0.0f, 0.0f);
	p2Legs.location = glm::vec2(0.0f, 0.0f);

	p1Head.active = false;
	p1Head.weight = movementWeight;
	p1Head.direction = glm::vec2(0.0f, 0.0f);
	p1Head.location = glm::vec2(0.0f, 0.0f);

	p2Head.active = false;
	p2Head.weight = movementWeight;
	p2Head.direction = glm::vec2(0.0f, 0.0f);
	p2Head.location = glm::vec2(0.0f, 0.0f);

	p1Body.active = false;
	p1Body.weight = movementWeight;
	p1Body.direction = glm::vec2(0.0f, 0.0f);
	p1Body.location = glm::vec2(0.0f, 0.0f);

	p2Body.active = false;
	p2Body.weight = movementWeight;
	p2Body.direction = glm::vec2(0.0f, 0.0f);
	p2Body.location = glm::vec2(0.0f, 0.0f);


	p1Swing.active = false;
	p1Swing.weight = 40;
	p1Swing.direction = glm::vec2(0.0f, 0.0f);
	p1Swing.location = p1Head.location;
	p2Swing.active = false;
	p2Swing.weight = 40;
	p2Swing.direction = glm::vec2(0.0f, 0.0f);
	p2Swing.location = p2Head.location;

	used = true;

}

//john-note: make this scaleable you idiot
glm::vec2 VectorField::totalWeight(glm::vec2 position)
{
	glm::vec2 totalWeight = glm::vec2(0.0f, 0.0f);
 	//adds weight of p1's movement
	if (p1Head.active && glm::distance(position, p1Head.location) < range ) {
		totalWeight += p1Head.getForce(position);
	}
	if (p1Legs.active && glm::distance(position, p1Legs.location) < range) {
		totalWeight += p1Legs.getForce(position);
	}
	if (p1Body.active && glm::distance(position, p1Body.location) < range) {
		totalWeight += p1Body.getForce(position);
	}

	//adds weight of p1's hits
	if (p1Swing.active && glm::distance(position, p1Swing.location) < range) {
		totalWeight += p1Swing.getForce(position);
	}

	//adds weight of p2's movement
	if (p2Head.active && glm::distance(position, p2Head.location) < range) {
		totalWeight += p2Head.getForce(position);
	}
	if (p2Legs.active && glm::distance(position, p2Legs.location) < range) {
		totalWeight += p2Legs.getForce(position);
	}
	if (p2Body.active && glm::distance(position, p2Body.location) < range) {
		totalWeight += p2Body.getForce(position);
	}


	//adds weight of p2's hits
	if (p2Swing.active && glm::distance(position, p2Swing.location) < range) {
		totalWeight += p2Swing.getForce(position);
	}

	return totalWeight;

}

void VectorField::addSwing(int player, glm::vec2 dir)
{
	//inpulse vector if players attack
	if (player == 1) {
		p1Swing.active = true;
		p1Swing.direction = dir;
	}
	else {
		p2Swing.active = true;
		p2Swing.direction = dir;
	}
}

void VectorField::update(glm::vec2 p1, glm::vec2 p1Dir, glm::vec2 p2, glm::vec2 p2Dir)
{
	
	//update player 1 attributes
	if (p1Dir != glm::vec2(0.0f, 0.0f)) {		
		//cout << "moved";
		//motors are activated on player movement
		p1Legs.active = p1Head.active = p1Body.active = true;
		//legs set to player location
		p1Legs.location = p1;
		//head offset applied
		p1Head.location = p1 + glm::vec2(0.0f, headOffSet);
		//body offset applied
		p1Body.location = p1Swing.location = p1 + glm::vec2(0.0f, bodyOffset);
		//direction of motor propogation updated
		p1Head.direction = p1Legs.direction = p1Body.direction = p1Dir;
	}
	else {
		p1Legs.active = p1Head.active = p1Body.active = false;
	}
	
	//update player 2 attributes
	if (p2Dir != glm::vec2(0.0f, 0.0f)) {
		//motors are activated on player movement
		p2Legs.active = p2Head.active = p2Body.active = true;
		//legs set to player location
		p2Legs.location = p2;
		//head offset applied
		p2Head.location = p2 + glm::vec2(0.0f, headOffSet);
		//body offset applied
		p2Body.location = p2Swing.location = p2 + glm::vec2(0.0f, bodyOffset);
		//direction of motor propogation updated
		p2Head.direction = p2Body.direction = p2Legs.direction = p2Dir;
	}
	else {
		p2Legs.active = p2Head.active = p2Body.active = false;
	}

}
