#pragma once
#include "character.h"

//Will be the parent class for all other charcaters
///Has all basic functions/data that charcaters need but lacks the unique passives and thas virtual functions for each attack type
class charBlueDragon : public Character {

public:
	charBlueDragon() {}
	charBlueDragon(const std::string& body, const std::string& texture);

	charBlueDragon(const charBlueDragon* copy) {
		type = 1;

		for (int i = 0; i < 40; i++)//must equal placeholder
			this->aniSpeeds[i] = copy->aniSpeeds[i];

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
		for (int i = 0; i < 40; i++) {//for each action //must equal placeholder
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
		velocity = glm::vec3(0, 0, 0);
		lastPos = glm::vec3(0, 0, 0);
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

	void comboAdd() {

		Character::comboAdd();
		if (action == ACTION_A_METEOR_ALT) {
			// delete all hitboxes
			for (int i = 0; i < (int)activeHitboxes.size(); i++) {
				activeHitboxes[i]->setDone();
			}
			//bounce
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			cancel = false;
			velocity.x = 0;
			velocity.y = 0;
			aMeteorAlt2();
		}
	}

	void hit(Hitbox* hitBy) {
		if (action == ACTION_G_METEOR_ALT && cancel) {
			// delete all hitboxes
			for (int i = 0; i < (int)activeHitboxes.size(); i++) {
				activeHitboxes[i]->setDone();
			}
			//bounce
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			cancel = false;
			gMeteorAlt2();
		}
		else
			Character::hit(hitBy);
	}

	void update(int t, InputHandler* inputs, unsigned int playerNum) {
		
		Character::update(t, inputs, playerNum);
	}

	virtual Transform atkInputHandler(InputHandler* inputs, unsigned int playerNum) {

		if (action == ACTION_A_METEOR_ALT_2) {
			//interuptable = true;
			force.x = (inputs->getButton(playerNum, MyInputs::Right) - inputs->getButton(playerNum, MyInputs::Left))* 0.5f *  airAccel;
			return aMeteorAlt2();
		}
		else if (action == ACTION_G_METEOR_ALT && inputs->getButtonDown(playerNum, MyInputs::Dash) && cancel == true) {
				interuptable = true;
				action = ACTION_PLACEHOLDER;
				cancel = false;
				return dash(inputs->getButtonDown(playerNum, MyInputs::Left), inputs->getButtonDown(playerNum, MyInputs::Right));
		}
		else if (action == ACTION_G_METEOR_ALT_2){// || (action == ACTION_G_METEOR_ALT && inputs->getButtonDown(playerNum, MyInputs::Meteor) && cancel == true)) {
			////debug press again to activate counter
			//if (action == ACTION_G_METEOR_ALT)
			//{
			//	// delete all hitboxes
			//	for (int i = 0; i < (int)activeHitboxes.size(); i++) {
			//		activeHitboxes[i]->setDone();
			//	}
			//	//bounce
			//	interuptable = true;
			//	action = ACTION_PLACEHOLDER;
			//	cancel = false;
			//}
			return gMeteorAlt2();
		}
		else if ((action == ACTION_G_BASIC_2 && inputs->getButtonDown(playerNum, MyInputs::Basic) && cancel == true) || action == ACTION_G_BASIC_3) {
			interuptable = true;
			return gBasic3();
		}
		else if ((action == ACTION_G_BASIC && inputs->getButtonDown(playerNum, MyInputs::Basic) && cancel == true) || action == ACTION_G_BASIC_2) {
			interuptable = true;
			return gBasic2();
		}
		else
			return Character::atkInputHandler(inputs, playerNum);
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
	Transform aMeteorAlt();
	Transform aClearAlt();

	//extra moves
	Transform gBasic2();
	Transform gBasic3();
	Transform gMeteorAlt2();
	Transform aMeteorAlt2();

	unsigned int ACTION_G_BASIC_2 = 30;
	unsigned int ACTION_G_BASIC_3 = 31;
	unsigned int ACTION_G_METEOR_ALT_2 = 32;
	unsigned int ACTION_A_METEOR_ALT_2 = 33;
	bool cancel = false;

protected:

private:
};