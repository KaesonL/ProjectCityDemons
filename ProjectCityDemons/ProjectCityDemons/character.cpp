#include "character.h"

/*
Particall by John Wang
*/

/*
Keys:
$$$ - Particle Signal

*/

//new push
#define BASE_ANI_TOGGLE		true	//non-offensive animations
#define G_ATK_ANI_TOGGLE	true	//ground attacks
#define A_ATK_ANI_TOGGLE	true	//aerials
#define S_ATK_ANI_TOGGLE	false	//specials


#define HITBOX_TOGGLE		false	//visual hitboxes
#define HURTBOX_TOGGLE		false	//visual hurtboxes

Character::Character(const std::string& textureName){
	//Nothing Here.  DO NOT MAKE CHARACTER
}

void Character::update(int t, InputHandler* inputs, unsigned int playerNum) {

	//max health
	if (currentHealth > maxHealth) {
		currentHealth = maxHealth;
		greyHealth = maxHealth;
	}

	//hp over time
	if (currentHealth < greyHealth) {
		currentHealth += regenSpeed; // passive health regen
		greyHealth -= greyDrainSpeed; // grey health passive drain

		if (comboCount > 1) {//while comboing
			currentHealth += regenSpeed; // gain double hb regen
		}
	}

	if (comboTimer > comboMaxTime) {
		comboClear();
		resetTimer();
	}

	// if hp ever higher than grey, set to grey
	else if (currentHealth > greyHealth) {
		currentHealth = greyHealth;
	}

	//COMBO
	comboTick();

	//actual update
	force.x = 0;
	transform = actionHandler(inputs, playerNum);
	if (action != ACTION_A_DASHB && action != ACTION_A_DASHF && action != ACTION_G_DASHB && action != ACTION_G_DASHF) {

		//physics update
		force = glm::vec3(force.x, 0 - gravity, 0);
		acceleration = force / mass;
		velocity = velocity + (acceleration);

		////max speed
		//if (velocity.x > maxSpeed)
		//	velocity.x = maxSpeed;
		//if (velocity.x < (0 - maxSpeed))
		//	velocity.x = (0 - maxSpeed);

		if (dashTimer < 100)
			dashTimer++;
	}

	//Update Position
	position = position + (velocity);

	///Rotate the player to the correct way they should look
	if (facingRight == true)
		//glm::rotate(transform, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		transform.Scale(glm::vec3(1.0f, 1.0f, 1.0f));
	else
		//glm::rotate(transform, 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		transform.Scale(glm::vec3(-1.0f, 1.0f, 1.0f));

	//Fake Wall
	///Will be changed later
	if (position.x > rightWall) {
		//called on bounce
		position.x = rightWall;
		if (action == ACTION_A_HIT) {
			velocity.y *= 0.75f;
			velocity.x *= -0.75f;
			hitForce.x *= -0.9f;
		}
		else if ((currentFrame >= activeFrames || interuptable == true) && (action == ACTION_FALL)) {
			velocity.x *= 0.1f;
		}
	}
	else if (position.x < leftWall) {
		//called on bounce
		position.x = leftWall;
		if (action == ACTION_A_HIT) {
			velocity.y *= 0.75f;
			velocity.x *= -0.75f;
			hitForce.x *= -0.9f;
		}
		else if ((currentFrame >= activeFrames || interuptable == true) && (action == ACTION_FALL)) {
			velocity.x *= 0.1f;
		}
	}

	//Fake Floor Code
	///Will be changed later
	if (position.y < floor) {
		//called on landing
		position.y = floor;
		jumpsLeft = airJumps;
		usedAirDash1 = false;
		usedAirDash2 = false;

		if (action == ACTION_A_HIT && currentFrame < activeFrames) { // Bounce
			velocity.y *= -0.75f;
			velocity.x *= 0.5f;
			hitForce.y *= -0.9f;
		}
		else if (action == ACTION_A_HIT) //Glitch Fix
		{
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			Fall();
		}
		// Landing
		else if ((currentFrame >= activeFrames || interuptable == true) && (action == ACTION_FALL || action == ACTION_A_BASIC_ALT || action == ACTION_A_CLEAR || action == ACTION_A_METEOR || action == ACTION_A_BASIC)) {
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			// delete all hitboxes
			for (int i = 0; i < (int)activeHitboxes.size(); i++) {
				activeHitboxes[i]->setDone();
			}
			Idle();
			//velocity.x *= 0.7f;
			partiQueue.push(LANDDUST);						//$$$
		}
		else if (currentFrame < activeFrames - 5 && (action == ACTION_A_BASIC_ALT || action == ACTION_A_CLEAR || action == ACTION_A_METEOR || action == ACTION_A_BASIC)) {//landing lag
			currentFrame = activeFrames - 3;
			index = aniFrames[action].size() -1;
			//velocity.x *= 0.7f;
		}
	}
	else {
		//called after hitstun is up in the air
		if (currentFrame >= activeFrames && action == ACTION_A_HIT) {
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			Fall();
		}
	}
	
	//Testing Code for Spawning Hitboxes
	//Check Hitboxes
	for (unsigned int i = 0; i < activeHitboxes.size(); i++) {
		activeHitboxes[i]->update(t, position);

		if (activeHitboxes[i]->isDone())
			activeHitboxes.erase(activeHitboxes.begin() + i);
	}

	//Check Hurtboxes
	for (unsigned int i = 0; i < hurtbox.size(); i++) {
		hurtbox[i]->update(t, position);
	}

	transform.SetTranslation(glm::vec3(position.x, position.y, position.z));
	//glm::scale(transform, glm::vec3(scaleX, scaleY, scaleZ));
	transform.Scale(glm::vec3(scaleX, scaleY, scaleZ));

}

//Returns the Players Position
glm::vec3 Character::getPosition()
{
	return position;
}

void Character::draw(ShaderProgram shader, float dt) {
	if (action < ACTION_PLACEHOLDER) {
		aniTimer += dt / aniSpeeds[action];

		while (aniTimer > 1.0f)
		{
			aniTimer -= 1.0f;
			index = (index + 1) % (aniFrames[action].size());//9 total frames
		}
		if (index >= aniFrames[action].size())
			index = 0;
		// Ask for the handles identfying the uniform variables in our shader.
		shader.SendUniformMat4("uModel", transform.data, true);
		shader.SendUniform("interp", aniTimer);

		int modelLoc = glGetUniformLocation(shader.getProgram(), "uModel");
		glUniformMatrix4fv(modelLoc, 1, false, transform.data);

		activeTexture->Bind();
		glBindVertexArray(aniFrames[action][index]->VAO);

		// Adjust model matrix for next object's location
		glDrawArrays(GL_TRIANGLES, 0, aniFrames[action][index]->GetNumVertices());
		glUniformMatrix4fv(modelLoc, 1, false, Transform().data);
	}
	else {
		index = 0;
		shader.SendUniformMat4("uModel", transform.data, true);
		shader.SendUniform("interp", 0);

		int modelLoc = glGetUniformLocation(shader.getProgram(), "uModel");
		glUniformMatrix4fv(modelLoc, 1, false, transform.data);

		activeTexture->Bind();
		glBindVertexArray(aniFrames[ACTION_IDLE][index]->VAO);

		glDrawArrays(GL_TRIANGLES, 0, aniFrames[ACTION_IDLE][index]->GetNumVertices());
		glUniformMatrix4fv(modelLoc, 1, false, Transform().data);
	}
}


void Character::drawBoxes(ShaderProgram GBufferPass) {
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	//hurtboxes
	if (HURTBOX_TOGGLE) {
		for (unsigned int i = 0; i < hurtbox.size(); i++) {
			int modelLoc = glGetUniformLocation(GBufferPass.getProgram(), "uModel");
			glUniformMatrix4fv(modelLoc, 1, false, hurtbox[i]->getTransform().data);

			shieldTexture.Bind();
			glBindVertexArray(boxMesh.VAO);

			// Adjust model matrix for next object's location
			glDrawArrays(GL_TRIANGLES, 0, boxMesh.GetNumVertices());
			glUniformMatrix4fv(modelLoc, 1, false, Transform().data);
		}
		shieldTexture.UnBind();
	}
	//hitboxes
	if (HITBOX_TOGGLE) {
		for (unsigned int i = 0; i < activeHitboxes.size(); i++) {
			int modelLoc = glGetUniformLocation(GBufferPass.getProgram(), "uModel");
			glUniformMatrix4fv(modelLoc, 1, false, activeHitboxes[i]->getTransform().data);

			boxTexture.Bind();
			glBindVertexArray(boxMesh.VAO);

			// Adjust model matrix for next object's location
			glDrawArrays(GL_TRIANGLES, 0, boxMesh.GetNumVertices());
			glUniformMatrix4fv(modelLoc, 1, false, Transform().data);
		}
		boxTexture.UnBind();
	}
	glDisable(GL_BLEND);

}

//Sets player position
void Character::setPosition(glm::vec3 pos){
	position = pos;
}

//Returns a list of all active hitboxes from this player
std::vector<Hitbox*> Character::getHitboxes()
{
	return activeHitboxes;
}

std::vector<Hitbox*> Character::getHurtboxes()
{
	return hurtbox;
}

//called on hit
void Character::onHit(Hitbox* hitBy, unsigned int comboCount) {

	float result;
	//calculate power w/ scaling
	result = hitBy->getPower() + ((hitBy->getScaling()*0.01f) * comboCount); ///power + (sclaing*combo/10)
	//if grounded and power below 5, stay grounded
	if (getPosition().y <= floor && result < 5)
		result = 0;

	facingRight = !(hitBy->facingRight);
	float xComp = cos((180.0f - hitBy->angle) * 3.14f / 180.0f);
	float yComp = sin((180.0f - hitBy->angle) * 3.14f / 180.0f);

	if (result <= 0) {
		action = ACTION_G_HIT;
		hitForce.x = 0;
		hitForce.y = 0;
	}
	else if (result <= 10) {
		action = ACTION_A_HIT;
		hitForce.y = 0.05f + (result * 0.01f);
		if (facingRight)
			hitForce.x = -0.1f - (result * 0.01f);
		else
			hitForce.x = 0.1f + (result * 0.01f);
	}
	else if (result <= 20) {
		action = ACTION_LAUNCHED_B;
		//hitForce.x = 5 * xComp;
		//hitForce.y = 5 * yComp;
		if (hitBy->angle > 0) {
			hitForce.y = 0.1f;
			if (facingRight)
				hitForce.x = -0.5f;
			else
				hitForce.x = 0.5f;
		}
		else {
			hitForce.y = -0.3f;
			if (facingRight)
				hitForce.x = -0.03f;
			else
				hitForce.x = 0.03f;
		}
	}
	else {//if a bouncer
		action = ACTION_LAUNCHED_K;
		//hitForce.x = 5 * xComp;
		//hitForce.y = 5 * yComp;
		if (hitBy->angle > 0) {
			hitForce.y = 0.1f;
			if (facingRight)
				hitForce.x = -0.5f;
			else
				hitForce.x = 0.5f;
		}
		else {
			hitForce.y = -0.3f;
			if (facingRight)
				hitForce.x = -0.03f;
			else
				hitForce.x = 0.03f;
		}
	}

	index = 0;
	aniTimer = 0;
	interuptable = false;
	activeFrames = unsigned int(hitframes + hitstun + hitBy->getPower());//
	currentFrame = 1;
	currentHealth -= hitBy->getDamage();

	if (currentHealth <= 0) {
		dead = true;
		greyHealth = 0;
		//set launched
	}
}

Transform Character::actionHandler(InputHandler* inputs, unsigned int playerNum)
{
	Transform result;

	if (interuptable) {
		// AIR ATTACKS
		if (inputs->getButtonDown(playerNum, MyInputs::Basic) && inputs->getButton(playerNum, MyInputs::Shift) && position.y > floor) {
			result = aBasicAlt();
		}
		else if (inputs->getButtonDown(playerNum, MyInputs::Clear) && inputs->getButton(playerNum, MyInputs::Shift) && position.y > floor) {
			result = aClearAlt();
		}
		else if (inputs->getButtonDown(playerNum, MyInputs::Meteor) && inputs->getButton(playerNum, MyInputs::Shift) && position.y > floor) {
			result = aMeteorAlt();
		}
		else if (inputs->getButtonDown(playerNum, MyInputs::Meteor) && position.y > floor) {
			result = aMeteor();
		}
		else if (inputs->getButtonDown(playerNum, MyInputs::Clear) && position.y > floor) {
			result = aClear();
		}
		else if (inputs->getButtonDown(playerNum, MyInputs::Basic) && position.y > floor) {
			result = aBasic();
		}

		///jump
		else if (inputs->getButtonDown(playerNum, MyInputs::Jump) && position.y <= floor) {
			//result = PreJump();
			if ((inputs->getButton(playerNum, MyInputs::Right) && facingRight) || (inputs->getButton(playerNum, MyInputs::Left) && !facingRight))
				result = JumpF();
			else if ((inputs->getButton(playerNum, MyInputs::Right) && !facingRight) || (inputs->getButton(playerNum, MyInputs::Left) && facingRight))
				result = JumpB();
			else
				result = Jump();
		}
		///air jump
		else if (inputs->getButtonDown(playerNum, MyInputs::Jump) && position.y > floor) {
			if ((inputs->getButton(playerNum, MyInputs::Right) && facingRight) || (inputs->getButton(playerNum, MyInputs::Left) && !facingRight))
				result = JumpF();
			else if ((inputs->getButton(playerNum, MyInputs::Right) && !facingRight) || (inputs->getButton(playerNum, MyInputs::Left) && facingRight))
				result = JumpB();
			else
				result = Jump();
		}

		// GROUNDED ATTACKS
		else if (inputs->getButtonDown(playerNum, MyInputs::Basic) && inputs->getButton(playerNum, MyInputs::Shift) && position.y <= floor) {
			result = gBasicAlt();
		}
		else if (inputs->getButtonDown(playerNum, MyInputs::Clear) && inputs->getButton(playerNum, MyInputs::Shift) && position.y <= floor) {
			result = gClearAlt();
		}
		else if (inputs->getButtonDown(playerNum, MyInputs::Meteor) && inputs->getButton(playerNum, MyInputs::Shift) && position.y <= floor) {
			result = gMeteorAlt();
		}
		else if (inputs->getButtonDown(playerNum, MyInputs::Meteor) && position.y <= floor) {
			result = gMeteor();
		}
		else if (inputs->getButtonDown(playerNum, MyInputs::Clear) && position.y <= floor) {
			result = gClear();
		}
		else if (inputs->getButtonDown(playerNum, MyInputs::Basic) && position.y <= floor) {
			result = gBasic();
		}


		// DASH
		else if (inputs->getButtonDown(playerNum, MyInputs::Dash) && position.y <= floor) {
			if ((inputs->getButton(playerNum, MyInputs::Right) && !facingRight) || (inputs->getButton(playerNum, MyInputs::Left) && facingRight))
				result = gDashB();
			else
				result = gDashF();
		}
		//air dash
		else if (inputs->getButtonDown(playerNum, MyInputs::Dash) && position.y > floor) {
			if ((inputs->getButton(playerNum, MyInputs::Right) && !facingRight) || (inputs->getButton(playerNum, MyInputs::Left) && facingRight))
				result = aDashB();
			else
				result = aDashF();
		}

		//walk right
		else if (inputs->getButton(playerNum, MyInputs::Right) && position.y <= floor) {
			if (facingRight)
				result = WalkF(inputs->getButton(playerNum, MyInputs::Right));
			else
				result = WalkB(inputs->getButton(playerNum, MyInputs::Right));
		}
		//walk back
		else if (inputs->getButton(playerNum, MyInputs::Left) && position.y <= floor) {
			if (facingRight)
				result = WalkB(inputs->getButton(playerNum, MyInputs::Left));
			else
				result = WalkF(inputs->getButton(playerNum, MyInputs::Left));
		}

		else {
			//continue current action
			result = currentAction();
		}
	}
	else {
		//continue current action
		result = currentAction();
	}
	return result;
}

Transform Character::currentAction()
{
	if (action == ACTION_IDLE) { return Idle(); }
	else if (action == ACTION_WALKF) { return WalkF(false); }
	else if (action == ACTION_WALKB) { return WalkB(false); }
	else if (action == ACTION_PREJUMP) { return PreJump(); }
	else if (action == ACTION_JUMP) { return Jump(); }
	else if (action == ACTION_JUMPF) { return JumpF(); }
	else if (action == ACTION_JUMPB) { return JumpB(); }
	else if (action == ACTION_FALL) { return Fall(); }
	else if (action == ACTION_G_BLOCK) {return gBlock();}
	else if (action == ACTION_A_BLOCK) {return aBlock();}
	else if (action == ACTION_G_DASHF) {return gDashF();}
	else if (action == ACTION_G_DASHB) {return gDashB();}
	else if (action == ACTION_A_DASHF) {return aDashF();}
	else if (action == ACTION_A_DASHB) {return aDashB();}
	else if (action == ACTION_G_HIT) { return gHit(); }
	else if (action == ACTION_A_HIT) { return aHit(); }
	else if (action == ACTION_LAUNCHED_B) { return Launched_Bounce(); }
	else if (action == ACTION_LAUNCHED_K) { return Launched_Knockdown(); }
	else if (action == ACTION_KNOCKDOWN) { return Knockdown(); }
	else if (action == ACTION_GETUP) { return Getup(); }
	else if (action == ACTION_BOUNCEG) {return Bounce_Ground();}
	else if (action == ACTION_BOUNCEW) {return Bounce_Wall();}
	else if (action == ACTION_G_BASIC) {return gBasic();}
	else if (action == ACTION_G_METEOR) { return gMeteor(); }
	else if (action == ACTION_G_CLEAR) { return gClear(); }
	else if (action == ACTION_G_BASIC_ALT) { return gBasicAlt(); }
	else if (action == ACTION_G_METEOR_ALT) { return gMeteorAlt(); }
	else if (action == ACTION_G_CLEAR_ALT) { return gClearAlt(); }
	else if (action == ACTION_A_BASIC) { return aBasic(); }
	else if (action == ACTION_A_METEOR) { return aMeteor(); }
	else if (action == ACTION_A_CLEAR) { return aClear(); }
	else if (action == ACTION_A_BASIC_ALT) { return aBasicAlt(); }
	else if (action == ACTION_A_METEOR_ALT) { return aMeteorAlt(); }
	else if (action == ACTION_A_CLEAR_ALT) { return aClearAlt(); }
	else { return Idle(); }
	return Transform();
}

Transform Character::Idle()
{
	Transform result;
	//Called First Frame
	if (interuptable == true && action != ACTION_IDLE) {
		interuptable = true;
		action = ACTION_IDLE;
		activeFrames = 40;
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
		velocity.x = 0;
		acceleration.x = 0;
		force.x = 0;
	}
	//Auto Finish
	if (action == ACTION_IDLE && currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		return Idle();
	}
	//Actions Per Frame
	switch (currentFrame) {
	case 1: 
	default:
		break;
	}
	currentFrame++;
	return result;
}


Transform Character::gBlock()
{
	Transform result;
	//Called First Frame
	if (interuptable == true && action != ACTION_G_BLOCK) {
		interuptable = false;
		action = ACTION_G_BLOCK;
		activeFrames = 6;
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
	}
	//Auto Finish
	if (currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		return Idle();
	}
	//Actions Per Frame
	switch (currentFrame) {
	case 1:
	default:
		break;
	}
	currentFrame++;
	return result;
}


Transform Character::aBlock()
{
	Transform result;
	//Called First Frame
	if (interuptable == true && action != ACTION_A_BLOCK) {
		interuptable = false;
		action = ACTION_A_BLOCK;
		activeFrames = 6;
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
	}
	//Auto Finish
	if (currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		if(position.y <= this->floor)return Idle();
		else return Fall();
	}
	//Actions Per Frame
	switch (currentFrame) {
	case 1:
	default:
		break;
	}
	currentFrame++;
	return result;
}

Transform Character::WalkF(bool held)//will always be true if called from press, false if called from current action (no press)
{
	Transform result;
	//Called First Frame
	if (interuptable == true && action != ACTION_WALKF) {
		interuptable = true;
		action = ACTION_WALKF;
		activeFrames = 30;
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
	}
	//Auto Finish
	if (!held) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		return Idle();
	}
	//Auto Loop
	if (held && currentFrame >= activeFrames) {
		interuptable = true;
		currentFrame = 1;
		return WalkF(held);
	}
	//Actions Per Frame
	switch (currentFrame) {
	case 1:
	default:
		break;
	}
	//Actions Every Frame
	if (facingRight) {
		velocity.x = walkSpeed;
	}
	else
		velocity.x = walkSpeed * -1.0f;
	currentFrame++;
	return result;
}

Transform Character::WalkB(bool held)//will always be true if called from press, false if called from current action (no press)
{
	Transform result;
	//Called First Frame
	if (interuptable == true && action != ACTION_WALKB) {
		interuptable = true;
		action = ACTION_WALKB;
		activeFrames = 30;
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
	}
	//Auto Finish
	if (!held) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		return Idle();
	}
	//Auto Loop
	if (held && currentFrame >= activeFrames) {
		interuptable = true;
		currentFrame = 1;
		return WalkB(held);
	}
	//Actions Per Frame
	switch (currentFrame) {
	case 1:
	default:
		break;
	}
	//Actions Every Frame
	if (facingRight) {
		velocity.x = walkSpeed * -1.0f;
	}
	else
		velocity.x = walkSpeed;
	currentFrame++;
	return result;
}

Transform Character::gDashF()
{
	Transform result;
	//Called First Frame
	if (interuptable == true && action != ACTION_G_DASHF) {
		if (dashTimer < 10) return Idle();//10 frames between dashes
		interuptable = false;
		action = ACTION_G_DASHF;
		activeFrames = dashLength;
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
		dashTimer = 0;
	}
	//Auto Finish
	if (currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		return Idle();
	}
	//Actions Per Frame
	switch (currentFrame) {
	case 1:
	default:
		break;
	}
	//Actions Every Frame
	int direction = (int)facingRight;
	if (!facingRight)
		direction = -1;
	velocity.y = 0.0f;
	velocity.x = (float)direction * dashSpeed;
	currentFrame++;
	return result;
}


Transform Character::gDashB()
{
	Transform result;
	//Called First Frame
	if (interuptable == true && action != ACTION_G_DASHB) {
		if (dashTimer < 10) return Idle();//10 frames between dashes
		interuptable = false;
		action = ACTION_G_DASHB;
		activeFrames = dashLength;
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
		dashTimer = 0;
	}
	//Auto Finish
	if (currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		return Idle();
	}
	//Actions Per Frame
	switch (currentFrame) {
	case 1:
	default:
		break;
	}
	//Actions Every Frame
	int direction = (int)(!facingRight);
	if (facingRight)
		direction = -1;
	velocity.y = 0.0f;
	velocity.x = (float)direction * dashSpeed;
	currentFrame++;
	return result;
}

Transform Character::aDashF()
{
	Transform result;
	//Called First Frame
	if (interuptable == true && action != ACTION_A_DASHF) {
		if (usedAirDash1 && usedAirDash1) return Fall();
		if (!usedAirDash1) usedAirDash1 = true;
		else if (!usedAirDash2) usedAirDash2 = true;
		interuptable = false;
		action = ACTION_A_DASHF;
		activeFrames = dashLength;
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
		if (jumpsLeft == 1) {
			usedAirDash1 = true;
		}
		else if (jumpsLeft == 0) {
			usedAirDash2 = true;
			usedAirDash1 = true;
		}
	}
	//Auto Finish
	if (currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		return Fall();
	}
	//Actions Per Frame
	switch (currentFrame) {
	case 1:
	default:
		break;
	}
	//Actions Every Frame
	int direction = (int)facingRight;
	if (!facingRight)
		direction = -1;
	velocity.y = 0.0f;
	velocity.x = (float)direction * dashSpeed;
	currentFrame++;
	return result;
}

Transform Character::aDashB()
{
	Transform result;
	//Called First Frame
	if (interuptable == true && action != ACTION_A_DASHB) {
		if (usedAirDash1 && usedAirDash1) return Fall();
		if (!usedAirDash1) usedAirDash1 = true;
		else if (!usedAirDash2) usedAirDash2 = true;
		interuptable = false;
		action = ACTION_A_DASHB;
		activeFrames = dashLength;
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
		if (jumpsLeft == 1) {
			usedAirDash1 = true;
		}
		else if (jumpsLeft == 0) {
			usedAirDash2 = true;
			usedAirDash1 = true;
		}
	}
	//Auto Finish
	if (currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		return Fall();
	}
	//Actions Per Frame
	switch (currentFrame) {
	case 1:
	default:
		break;
	}
	//Actions Every Frame
	int direction = (int)(!facingRight);
	if (facingRight)
		direction = -1;
	velocity.y = 0.0f;
	velocity.x = (float)direction * dashSpeed;
	currentFrame++;
	return result;
}

Transform Character::gHit()
{
	Transform result;
	//Called First Frame
	if (interuptable == true && action != ACTION_G_HIT) {
		interuptable = false;
		action = ACTION_G_HIT;
		activeFrames = hitstun + hitframes;
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
	}
	//Auto Finish
	if (currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		return Idle();
	}
	//Actions Per Frame
	switch (currentFrame) {
	case 1:
	default:
		break;
	}
	currentFrame++;
	return result;
}


Transform Character::aHit()
{
	Transform result;
	//Called First Frame
	if (interuptable == true && action != ACTION_A_HIT) {
		interuptable = false;
		action = ACTION_A_HIT;
		activeFrames = hitstun + hitframes;
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
		position.y += 0.01f;
	}
	//Auto Finish
	if (currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		if (position.y <= this->floor) return Idle();
		else return Fall();
	}
	//Actions Per Frame
	if (currentFrame < hitframes) {
		velocity.y = hitForce.y;
	}
	velocity.x = hitForce.x;
	currentFrame++;
	return result;
}

Transform Character::Launched_Bounce()
{
	Transform result;
	//Called First Frame
	if (action != ACTION_LAUNCHED_B) {
		interuptable = false;
		action = ACTION_LAUNCHED_B;
		activeFrames = hitstun + hitframes;
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
		position.y += 0.05f;
	}
	//Auto Finish
	if (currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		if (position.y <= this->floor) return Idle();
		else return Fall();
	}
	else if (position.y <= this->floor && (abs(velocity.x) <= 0.05f)) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		return Bounce_Ground();
	}
	else if ((position.x <= this->leftWall || position.x >= this->rightWall) && (abs(velocity.y) <= 0.1f)) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		return Bounce_Wall();
	}
	//Actions Per Frame
	if (currentFrame < hitframes) {
		velocity.y = hitForce.y;
	}
	velocity.x = hitForce.x;
	currentFrame++;
	return result;
}

Transform Character::Bounce_Wall()
{
	Transform result;
	//Called First Frame
	if (action != ACTION_BOUNCEW) {
		interuptable = false;
		action = ACTION_BOUNCEW;
		activeFrames = bounceframes;
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
	}
	//Auto Finish
	if (currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		if (position.y <= this->floor) return Idle();
		else return Fall();
	}
	//Actions Per Frame
	velocity.x = hitForce.x * -1.0f;
	currentFrame++;
	return result;
}

Transform Character::Bounce_Ground()
{
	Transform result;
	//Called First Frame
	if (action != ACTION_BOUNCEG) {
		interuptable = false;
		action = ACTION_BOUNCEG;
		activeFrames = bounceframes;
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
	}
	//Auto Finish
	if (currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		if (position.y <= this->floor) return Idle();
		else return Fall();
	}
	//Actions Per Frame
	velocity.y = hitForce.y * -1.0f;
	currentFrame++;
	return result;
}

Transform Character::Launched_Knockdown()
{
	Transform result;
	//Called First Frame
	if (action != ACTION_LAUNCHED_K) {
		interuptable = false;
		action = ACTION_LAUNCHED_K;
		activeFrames = hitstun + hitframes *2;
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
		position.y += 0.05f;
	}
	//Auto Finish
	if (currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		if (position.y <= this->floor) return Idle();
		else return Fall();
	}
	else if (position.y <= this->floor) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		return Knockdown();
	}
	//Actions Per Frame
	if (currentFrame < hitframes) {
		velocity.y = hitForce.y;
	}
	velocity.x = hitForce.x;
	currentFrame++;
	return result;
}


Transform Character::PreJump()
{
	Transform result = Transform();
	if (interuptable == true && action != ACTION_PREJUMP) {
		action = ACTION_PREJUMP;
		activeFrames = prejumpLength;
		currentFrame = 1;
		interuptable = false;
	}
	else if (action == ACTION_PREJUMP && currentFrame <= activeFrames) {

		if (currentFrame == activeFrames) {
			//if action over, goto jump
			action = ACTION_PLACEHOLDER;
			interuptable = true;
			return Jump();
		}

		//stuff goes here

		currentFrame++;
	}
	return result;
}

Transform Character::Jump()
{
	Transform result = Transform();
	if (interuptable == true && action != ACTION_JUMP) {
		if (jumpsLeft == 0 && position.y > this->floor) return Fall();//10 frames between dashes
		if (position.y > this->floor) jumpsLeft--;
		action = ACTION_JUMP;
		activeFrames = jumpFrames;
		currentFrame = 1;
		interuptable = false;
		partiQueue.push(LANDDUST);						//$$$
	}
	else if (action == ACTION_JUMP && currentFrame <= activeFrames) {

		if (currentFrame == activeFrames) {
			//if action over, goto fall
			action = ACTION_PLACEHOLDER;
			interuptable = true;
			return Fall();
		}

		//stuff goes here
		velocity.y = jumpForce;
		velocity.x = 0;
		currentFrame++;
	}
	return result;
}

Transform Character::JumpF()
{
	Transform result = Transform();
	if (interuptable == true && action != ACTION_JUMPF) {
		if (jumpsLeft == 0 && position.y > this->floor) return Fall();//10 frames between dashes
		if (position.y > this->floor) jumpsLeft--;
		action = ACTION_JUMPF;
		activeFrames = jumpFrames;
		currentFrame = 1;
		interuptable = false;
		partiQueue.push(LANDDUST);						//$$$
	}
	else if (action == ACTION_JUMPF && currentFrame <= activeFrames) {

		if (currentFrame == activeFrames) {
			//if action over, goto fall
			action = ACTION_PLACEHOLDER;
			interuptable = true;
			return Fall();
		}

		if (facingRight)
			velocity.x = jumpForceX;
		else 
			velocity.x = jumpForceX * -1.0f;
		//stuff goes here
		velocity.y = jumpForce;
		currentFrame++;
	}
	return result;
}

Transform Character::JumpB()
{
	Transform result = Transform();
	if (interuptable == true && action != ACTION_JUMPB) {
		if (jumpsLeft == 0 && position.y > this->floor) return Fall();//10 frames between dashes
		if (position.y > this->floor) jumpsLeft--;
		action = ACTION_JUMPB;
		activeFrames = jumpFrames;
		currentFrame = 1;
		interuptable = false;
		partiQueue.push(LANDDUST);						//$$$
	}
	else if (action == ACTION_JUMPB && currentFrame <= activeFrames) {

		if (currentFrame == activeFrames) {
			//if action over, goto fall
			action = ACTION_PLACEHOLDER;
			interuptable = true;
			return Fall();
		}

		if (!facingRight)
			velocity.x = jumpForceX;
		else
			velocity.x = jumpForceX * -1.0f;
		//stuff goes here
		velocity.y = jumpForce;
		currentFrame++;
	}
	return result;
}

Transform Character::Fall()
{
	Transform result = Transform();
	if (interuptable == true && action != ACTION_FALL) {
		action = ACTION_FALL;
		activeFrames = 9;
		currentFrame = 1;
		interuptable = true;
	}
	else if (action == ACTION_FALL && currentFrame <= activeFrames) {

		if (currentFrame == activeFrames) {
			//if action over, goto idle
			action = ACTION_PLACEHOLDER;
			interuptable = true;
			return Fall();
		}

		//stuff goes here

		currentFrame++;
	}
	return result;
}

Transform Character::Knockdown()
{
	Transform result;
	//Called First Frame
	if (action != ACTION_KNOCKDOWN) {
		interuptable = false;
		action = ACTION_KNOCKDOWN;
		activeFrames = 100;
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
		velocity.y = 0;
		acceleration.y = 0;
		force.y = 0;
	}
	//Auto Finish
	if (currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		return Getup();
	}
	//Actions Per Frame
	switch (currentFrame) {
	case 1:
	default:
		break;
	}
	velocity.x *= 0.95f;
	currentFrame++;
	return result;
}


Transform Character::Getup()
{
	Transform result;
	//Called First Frame
	if (action != ACTION_GETUP) {
		interuptable = false;
		action = ACTION_GETUP;
		activeFrames = 20;
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
		velocity.x = 0;
		acceleration.x = 0;
		force.x = 0;
	}
	//Auto Finish
	if (currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		return Idle();
	}
	//Actions Per Frame
	switch (currentFrame) {
	case 1:
	default:
		break;
	}
	currentFrame++;
	return result;
}