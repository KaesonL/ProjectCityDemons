#include "hitbox.h"

Hitbox::Hitbox(glm::vec3 _position, float _size, float _power, float _scaling, float _angle, unsigned int _activeFrames, float _dmg, glm::vec3 _velocity)
{
	//set all values
	position = _position;
	size = _size;
	power = _power;
	scaling = _scaling;
	angle = _angle;
	velocity = _velocity;
	activeFrames = _activeFrames;
	currentFrame = 1;
	damage = _dmg;
	acceleration = glm::vec3(0, 0, 0);
	projectile = false;
	spline = false;
	facingRight = false;
	hurtbox = false;

}

Hitbox::Hitbox(glm::vec3 _position, float _size)
{
	//set all values
	position = _position;
	size = _size;
	power = 0;
	scaling = 0;
	velocity = glm::vec3();
	activeFrames = 0;
	currentFrame = 1;
	damage = 0;
	acceleration = glm::vec3(0, 0, 0);
	projectile = false;
	spline = false;
	facingRight = false;
	hurtbox = true;

}

Hitbox::~Hitbox() {

}
/*
//updates the position
void Hitbox::update(int t)
{
	if (currentFrame <= activeFrames) {

		//update position
		velocity = velocity + (acceleration);
		position = position + (velocity);
		currentFrame++;
	}
}*/

//updates the position
void Hitbox::update(int t, glm::vec3 parent = { 0,0,0 })
{
	if (currentFrame <= activeFrames || hurtbox == true) {
		//update position
		if (spline) {
			if (curve.size() <= 4)
				position = catmull(curve[0], curve[1], curve[2], curve[3], (float)currentFrame / activeFrames);
			else if(curve.size() == 5){
				float percent = (float)currentFrame / activeFrames;
				if (percent < 0.5f)
					position = catmull(curve[0], curve[1], curve[2], curve[3], percent - 0.25f);
				else
					position = catmull(curve[1], curve[2], curve[3], curve[4], percent - 0.75f);


			}

			if (projectile) {
				globalPosition = position;
			}
			else {
				globalPosition = position + parent;
			}
		}
		else {
			velocity = velocity + (acceleration);
			position = position + (velocity);
			if (projectile) {
				globalPosition = position;
			}
			else {
				globalPosition = position + parent;
			}
		}
		currentFrame++;
	}
}

//Returns the position of the hitbox
glm::vec3 Hitbox::getPosition()
{
	return globalPosition;
}

//Returns the position of the hitbox
Transform Hitbox::getTransform()
{
	Transform trans;
	trans.SetTranslation(globalPosition);
	//glm::scale(trans, glm::vec3(size, size, size) * 1.3f);
	trans.Scale(glm::vec3(size,size,size) * 1.3f);
	return trans;
}


//Returns the position of the hitbox
glm::vec3 Hitbox::getVelocity()
{
	return velocity;
}

//returns the size of the hitbox
float Hitbox::getSize()
{
	return size;
}

//sets position
void Hitbox::setPosition(glm::vec3 pos)
{
	globalPosition = pos;
}

//returns true if active frames are up
bool Hitbox::isDone()
{
	if (currentFrame <= activeFrames)
		return false;
	else
		return true;
}

void Hitbox::setDone()
{
	currentFrame = activeFrames + 1;
}

float Hitbox::getScaling()
{
	return scaling;
}

float Hitbox::getPower()
{
	return power;
}
