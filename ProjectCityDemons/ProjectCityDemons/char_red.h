#pragma once
#include "character.h"

//Will be the parent class for all other charcaters
///Has all basic functions/data that charcaters need but lacks the unique passives and thas virtual functions for each attack type
class charRedDevil : public Character {

public:
	charRedDevil() {}
	charRedDevil(const std::string& body, const std::string& texture);

	charRedDevil(const charRedDevil* copy) {
		type = 2;

		this->aniTimer = 0.0f;
		this->index = 0;

		this->body = copy->body;
		this->bodyTexture = copy->bodyTexture;
		this->hurtTexture = copy->hurtTexture;
		activeTexture = &(this->bodyTexture);
		this->boxMesh = copy->boxMesh;
		this->boxTexture = copy->boxTexture;
		this->shieldTexture = copy->shieldTexture;

		//duplicate animations
		for (int i = 0; i < 23; i++) {//for each action
			for (int j = 0; j < (int)(copy->aniFrames[i].size()); j++) {//for each pose
				this->aniFrames[i].push_back(new Mesh(*copy->aniFrames[i][j]));//add pose to this character list
			}
		}
		for (int i = 0; i < (int)(aniFrames->size()); i++) {//for each action
			for (int j = 0; j < (int)(aniFrames[i].size()); j++) {//for each pose
				this->aniFrames[i][j]->VAO = copy->aniFrames[i][j]->VAO;
				this->aniFrames[i][j]->VBO_Normals = copy->aniFrames[i][j]->VBO_Normals;
				this->aniFrames[i][j]->VBO_UVs = copy->aniFrames[i][j]->VBO_UVs;
				this->aniFrames[i][j]->VBO_Vertices = copy->aniFrames[i][j]->VBO_Vertices;
			}
		}


		//Set Physics
		position = glm::vec3(0, 0, 0);
		lastPos = glm::vec3(0, 0, 0);
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
		currentHealth = 1000;
		maxHealth = 1000;
		greyHealth = 1000;
		comboTimer = 0;
		comboMaxTime = 120;//1 seconds times 60fps

		//Set Starting Action
		action = ACTION_FALL;//0 idle, 1 jumping
		idle();


		currentHealth = copy->currentHealth;

		Hitbox *hurt1 = new Hitbox(glm::vec3(0.0f, 3.0f, 0.0f), 3.0f);
		hurtbox.push_back(hurt1);
		Hitbox *hurt2 = new Hitbox(glm::vec3(0.0f, 1.0f, 0.0f), 3.0f);
		hurtbox.push_back(hurt2);
	}


	void update(int t, InputHandler* inputs, unsigned int playerNum) {
		
		Character::update(t, inputs, playerNum);
	}

	Transform gBasic();
	Transform gMeteor();
	Transform gClear();

	Transform gBasicAlt();
	Transform gMeteorAlt();
	Transform gClearAlt();

	Transform aBasic();
	Transform aMeteor();
	Transform aClear();

	Transform aBasicAlt();

	Transform nSpecial(bool charging);
	Transform uSpecial();


protected:
private:
};