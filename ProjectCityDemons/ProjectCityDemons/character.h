#pragma once
#include <glm/glm.hpp>
#include <GLM\gtx\transform.hpp>
#include "Mesh.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include <vector>
#include "hitbox.h"
#include <iostream>
#include "Transform.h"
#include "InputHandler.h"
#include <queue>

/*
Particall by John Wang
*/

//Enum for tracking particle signals 
enum Particall
{
	NOACTION,
	LANDDUST,
	LDASHDUST,
	RDASHDUST,
	LAUNCHDUST,
	HITSPARKL,
	HITSPARKR,
	METERFLAME,
	ULTFX,
};

//Will be the parent class for all other charcaters
///Has all basic functions/data that charcaters need but lacks the unique passives and thas virtual functions for each attack type
class Character {

public:

	unsigned int type;

	Character() {}
	Character(const std::string& body, const std::string& texture);

	Character(const Character* copy) {

		this->aniTimer = 0.0f;
		this->index = 0;

		this->body = copy->body;
		this->bodyTexture = copy->bodyTexture;
		this->hurtTexture = copy->hurtTexture;
		activeTexture = &(this->bodyTexture);
		this->boxMesh = copy->boxMesh;
		this->boxTexture = copy->boxTexture;
		this->shieldTexture = copy->shieldTexture;

		//this->aniFrames = copy->aniFrames;
		for (int i = 0; i < (int)(copy->aniFrames->size()); i++) {//for each action
			//for (int j = 0; j < (int)(copy->aniFrames[i].size()); j++) {//for each pose
			//	this->aniFrames[i].push_back(copy->aniFrames[i][j]);//add pose to this character list
			//}
			this->aniFrames[i] = copy->aniFrames[i];
		}

		//Set Physics
		position = glm::vec3(0, 0, 0);
		velocity = glm::vec3(0, 0, 0);
		acceleration = glm::vec3(0, 0, 0);
		force = glm::vec3(0, 0, 0);
		facingRight = true;
		//scaling
		scaleX = copy->scaleX;
		scaleY = copy->scaleY;
		scaleZ = copy->scaleZ;

		transform.Scale(glm::vec3(scaleX, scaleY, scaleZ));
		//glm::rotate(transform, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		transform.RotateY(90);

		mass = copy->mass;
		gravity = copy->gravity;
		diMultiplier = copy->diMultiplier;
		dashMultiplier = copy->dashMultiplier;
		runSpeed = copy->runSpeed;
		runAccel = copy->runAccel;
		airAccel = copy->airAccel;
		jumpForce = copy->jumpForce;
		jumpFrames = copy->jumpFrames;
		dashLength = copy->dashLength;
		prejumpLength = copy->prejumpLength;
		airJumps = jumpsLeft = 1;
		hitstun = copy->hitstun;
		hitframes = copy->hitframes;

		//set combo stuff
		comboCount = 0;
		currentHealth = 0;
		comboTimer = 0;
		comboMaxTime = 60;//1 seconds times 60fps

		//Set Starting Action
		action = ACTION_FALL;//0 idle, 1 jumping
		idle();


		currentHealth = copy->currentHealth;

		Hitbox *hurt1 = new Hitbox(glm::vec3(0.0f, 3.0f, 0.0f), 3.0f);
		hurtbox.push_back(hurt1);
		Hitbox *hurt2 = new Hitbox(glm::vec3(0.0f, 1.0f, 0.0f), 3.0f);
		hurtbox.push_back(hurt2);
	}

	virtual void update(int t, InputHandler* inputs, unsigned int playerNum);
	void draw(ShaderProgram GBufferPass, float dt);
	void drawBoxes(ShaderProgram GBufferPass);
	glm::vec3 getPosition();
	void setPosition(glm::vec3 pos);

	glm::vec3 getVelocity() { return velocity; }
	void setVelocity(glm::vec3 vel) { velocity = vel; }
	glm::vec3 getAccel() { return acceleration; }
	void setAccel(glm::vec3 accel) { acceleration = accel; }

	std::vector<Hitbox*> getHitboxes();
	std::vector<Hitbox*> getHurtboxes();
	virtual Transform atkInputHandler(InputHandler* inputs, unsigned int playerNum);

	bool facingRight;

	//actions
	unsigned int action;
	unsigned int activeFrames;
	unsigned int currentFrame;
	bool interuptable;

	bool isHit() {
		return (action == ACTION_HIT);
	}

	//queue for particle signals
	std::queue<Particall> partiQueue;

	//Actions
	Transform walk(bool held);
	Transform run(bool held);
	Transform dash(bool left, bool right);
	Transform initialDash(bool left, bool right);
	Transform prejump();
	Transform jump();
	Transform jump2();
	Transform fall();
	virtual void hit(Hitbox* hitBy);
	virtual Transform idle();

	//3 atks
	virtual Transform gBasic() { return Transform(); }
	virtual Transform gMeteor() { return Transform(); }
	virtual Transform gClear() { return Transform(); }
	//3 atks + shift
	virtual Transform gBasicAlt() { return Transform(); }
	virtual Transform gMeteorAlt() { return Transform(); }
	virtual Transform gClearAlt() { return Transform(); }
	//3 airs
	virtual Transform aBasic() { return Transform(); }
	virtual Transform aMeteor() { return Transform(); }
	virtual Transform aClear() { return Transform(); }
	//3airs + shift
	virtual Transform aBasicAlt() { return Transform(); }
	virtual Transform aMeteorAlt() { return Transform(); }
	virtual Transform aClearAlt() { return Transform(); }

	//----------------------------------------------------------
	virtual void comboAdd() {
		if (comboTimer < comboMaxTime) {
			//add grey hp
			if (currentHealth < greyHealth) {
				currentHealth += regenSpeed; // passive health regen 2x as fast when u get hits
			}
			comboCount++;
			resetTimer();
		}
		else {
			comboClear();
			resetTimer();
		}
		//std::cout << "Count: " << comboCount << " Meter: " << currentHealth << std::endl;
	}
	void comboClear() { comboCount = 0; }
	void resetMeter() { greyHealth = currentHealth = maxHealth; comboClear(); }
	void comboTick() {
		if (currentHealth < 0) {
			currentHealth = 0;
		}
		comboTimer++;
	}
	void resetTimer() { comboTimer = 0; }

	void respawn() {
		comboTimer = 50;
		//hp
		currentHealth = 1000;
		greyHealth = 1000;

		position.y = 20;
		velocity = glm::vec3(0, 0, 0);
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		//fall();

		action = ACTION_RESPAWN;
	}

	//int getMeter() {
	//	return comboMeter;
	//}

	Transform transform;
	Mesh body;
	std::vector<Mesh*> aniFrames[40];//must equal placeholder
	Texture* activeTexture;
	Texture bodyTexture;
	Texture hurtTexture;
	float aniTimer;
	unsigned int index;

	bool ultFrame1;

	glm::vec2 pos2d;
	glm::vec2 lastPos;
	glm::vec2 movementDir;

	float currentHealth = 1000;//health
	float maxHealth = 1000;//total health for character
	float greyHealth = 1000;//health

	float regenSpeed = 0.05f;
	float greyDrainSpeed = 0.2f;

protected:
	//model

	//physics
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 force;

	//Attributes
	float mass;
	float gravity;
	float runSpeed;//max
	float runAccel;
	float airAccel;
	float jumpForce;
	float jumpForceX;
	float diMultiplier;
	float dashMultiplier;
	unsigned int jumpFrames;
	unsigned int dashLength;
	unsigned int prejumpLength;
	unsigned int airJumps;
	int jumpsLeft;
	int hitstun;
	int hitframes;
	bool usedAirDash1;
	bool usedAirDash2;

	//combo stuff
	unsigned int comboCount;//counts hits in a row, resets after x time
	unsigned int comboTimer;//counts time since last hit in combo
	unsigned int comboMaxTime;//2 seconds times 60fps

	unsigned int dashTimer;//counts time since last dash

	//scaling
	float scaleX;
	float scaleY;
	float scaleZ;

	//attacks
	std::vector<Hitbox*> activeHitboxes;
	std::vector<Hitbox*> hurtbox;//2


	//debug hitbox
	Mesh boxMesh;
	Texture boxTexture;
	Texture shieldTexture;
public:
	//===================================================//
		//ACTION ID's
	//===================================================//
	unsigned int ACTION_IDLE = 0;
	unsigned int ACTION_WALK = 1;
	unsigned int ACTION_RUN = 2;
	unsigned int ACTION_INTIAL_DASH = 3;
	unsigned int ACTION_PREJUMP = 4;
	unsigned int ACTION_JUMP = 5;
	unsigned int ACTION_JUMP2 = 6;
	unsigned int ACTION_FALL = 7;
	unsigned int ACTION_HIT = 8;
	unsigned int ACTION_HIT_G = 9;

	unsigned int ACTION_G_BASIC = 10;
	unsigned int ACTION_G_METEOR = 11;
	unsigned int ACTION_G_CLEAR = 12;

	unsigned int ACTION_G_BASIC_ALT = 13;
	unsigned int ACTION_G_METEOR_ALT = 14;
	unsigned int ACTION_G_CLEAR_ALT = 15;

	unsigned int ACTION_A_BASIC = 16;
	unsigned int ACTION_A_METEOR = 17;
	unsigned int ACTION_A_CLEAR = 18;

	unsigned int ACTION_A_BASIC_ALT = 19;
	unsigned int ACTION_A_METEOR_ALT = 20;
	unsigned int ACTION_A_CLEAR_ALT = 21;

	unsigned int ACTION_DASH = 22;
	unsigned int ACTION_BLOCK = 23;
	unsigned int ACTION_LAUNCHED = 24;
	unsigned int ACTION_KNOCKDOWN = 25;
	unsigned int ACTION_GETUP = 26;
	unsigned int ACTION_GETUP_SIDE = 27;

	unsigned int ACTION_PLACEHOLDER = 40;
	unsigned int ACTION_RESPAWN = 41;
	//===================================================//
	glm::vec3 hitForce;
	float aniSpeeds[40];//must equal placeholder

	//colisions
	float floor = 0;
	float leftWall = -25;
	float rightWall = 25;

private:
};