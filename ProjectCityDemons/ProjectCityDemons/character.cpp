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

Character::Character(const std::string& bodyName, const std::string& textureName){
	//Nothing Here.  DO NOT MAKE CHARACTER
}

void Character::update(int t, InputHandler* inputs, unsigned int playerNum) {

	//Change Player Facing only during these actinos.
	if (action <= 7 && action != ACTION_INTIAL_DASH) {
		///check the way it should face
		if (inputs->getButtonDown(playerNum, MyInputs::Right)) {
			facingRight = true;
		}
		else if (inputs->getButtonDown(playerNum, MyInputs::Left)) {
			facingRight = false;
		}
	}

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

	////meter flame
	//if ((currentHealth <= (maxHealth * 0.25f) && action != ACTION_HIT && action != ACTION_IN_NET) || action == ACTION_RESPAWN) {
	//	partiQueue.push(METERFLAME);						//$$$
	//}

	//actual update
	force.x = 0;
	transform = atkInputHandler(inputs, playerNum);
	if (action != ACTION_DASH && action != ACTION_RESPAWN) {

		//physics update
		force = glm::vec3(force.x, 0 - gravity, 0);
		acceleration = force / mass;
		velocity = velocity + (acceleration);

		//max speed
		if (velocity.x > runSpeed)
			velocity.x = runSpeed;
		if (velocity.x < (0 - runSpeed))
			velocity.x = (0 - runSpeed);

		//friction
		if (position.y <= 0.0f && ((!inputs->getButton(playerNum, MyInputs::Right) && !inputs->getButton(playerNum, MyInputs::Left)) || (action != ACTION_DASH && action != ACTION_WALK && action != ACTION_RUN && action != ACTION_INTIAL_DASH && action != ACTION_PREJUMP && action != ACTION_JUMP))) {
			if (action != ACTION_G_BASIC_ALT)
				velocity.x = velocity.x * 0.7f;
			else
				velocity.x = velocity.x * 0.95f;

		}
		if (position.y > 0.0f && (!inputs->getButtonDown(playerNum, MyInputs::Right) && !inputs->getButtonDown(playerNum, MyInputs::Left))) {
			velocity.x = velocity.x * 0.95f;
		}
		
		if (dashTimer < 100)
			dashTimer++;
	}

	//Update Position
	position = position + (velocity);

	//update movement dir

	//convert into 2d space
	pos2d = glm::vec2(position.x, position.y);
	//calculate direction of movement
	movementDir = (pos2d - lastPos);
	//Thresholds for non-movement
	if (movementDir.x < 0.3f && movementDir.x > -0.3f) {
		movementDir.x = 0.0f;
	}
	if(movementDir.y < 0.3f && movementDir.y > -0.3f)
	{
		movementDir.y = 0.0f;
	}

	//normalize
	glm::normalize(movementDir);
	//update lastpos
	lastPos = pos2d;

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
		if (action == ACTION_HIT) {
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
		if (action == ACTION_HIT) {
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

		if (action == ACTION_HIT && currentFrame < activeFrames) { // Bounce
			velocity.y *= -0.75f;
			velocity.x *= 0.5f;
			hitForce.y *= -0.9f;
		}
		else if (action == ACTION_HIT) //Glitch Fix
		{
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			fall();
		}
		// Landing
		else if ((currentFrame >= activeFrames || interuptable == true) && (action == ACTION_FALL || action == ACTION_A_BASIC_ALT || action == ACTION_A_CLEAR || action == ACTION_A_METEOR || action == ACTION_A_BASIC)) {
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			// delete all hitboxes
			for (int i = 0; i < (int)activeHitboxes.size(); i++) {
				activeHitboxes[i]->setDone();
			}
			idle();
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
		if (currentFrame >= activeFrames && action == ACTION_HIT) {
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			fall();
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
void Character::hit(Hitbox* hitBy) {
	float result;
	if (getPosition().y == floor && hitBy->getKnockback() < 7)
		result = 0;
	else if (!hitBy->facingRight) 
		result = 0.035f;//base launch multiplier (kb = 1)
	else 
		result = -0.035f;

	facingRight = !(hitBy->facingRight);
	float xComp = hitBy->getKnockback() * cos((180.0f - hitBy->getAngle()) * 3.14f / 180.0f);//*0.0174772222222222f);
	float yComp = hitBy->getKnockback() * sin((180.0f - hitBy->getAngle()) * 3.14f / 180.0f);//*0.0174772222222222f);

	glm::vec3 add(result * xComp, abs(result) * yComp, 0);
	hitForce = add;
	if (result == 0)
		action = ACTION_HIT_G;
	else
		action = ACTION_HIT;

	interuptable = false;
	activeFrames = unsigned int(hitframes + hitstun + hitBy->getKnockback());//
	currentFrame = 1;
	currentHealth -= hitBy->getDamage();
}

///0-up, 1-left, 2-down, 3-right, 4-A, 5-B, 6-jump
///7-hardLeft, 8-hardRight
///9-shield
Transform Character::atkInputHandler(InputHandler* inputs, unsigned int playerNum)
{
	Transform result;
	///die
	if (action == ACTION_RESPAWN) {
		if (currentFrame == 1) {//on first frame pause
			comboTimer = 50;
			currentHealth = maxHealth;
			activeTexture = &(bodyTexture);
			force = glm::vec3(0, 0, 0);
			acceleration = glm::vec3(0, 0, 0);
			velocity = glm::vec3(0, 0, 0);
			//position.x = 0;
			position.y = 20;
			currentFrame = 1;
		}
		//If in Hitstun reduce directional influence
		if (currentFrame != 1 && currentFrame < 50) {
			force = glm::vec3(0, 0, 0);
			acceleration = glm::vec3(0, 0, 0);
			velocity = glm::vec3(0, 0, 0);
			//position.x = 0;
			position.y -= 0.2f;
			if((int)(currentFrame * 0.2f) % 2)
				activeTexture = &(bodyTexture);
			else
				activeTexture = &(hurtTexture);
		}
		else if (currentFrame >= 50) {
			force = glm::vec3(0, 0, 0);
			acceleration = glm::vec3(0, 0, 0);
			velocity = glm::vec3(0, 0, 0);
			activeTexture = &(bodyTexture);
			//respawn();
			comboTimer = 50;
			currentHealth = maxHealth;
			//position = glm::vec3(0, 10, 0);
			velocity = glm::vec3(0, 0, 0);
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			fall();
		}

		if (facingRight)
			result.RotateY(-45.0f + currentFrame * 6.0f);
		else
			result.RotateY(-45.0f - currentFrame * 6.0f);

		currentFrame++;
	}
	///hit
	else if (action == ACTION_HIT_G) {
		//If in Hitstun reduce directional influence
		if (currentFrame >= activeFrames) {//called on last frame
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			idle();
		}
		else if (currentFrame == 1) {//on first frame pause
			force = glm::vec3(0, 0, 0);
			acceleration = glm::vec3(0, 0, 0);
			velocity = glm::vec3(0, 0, 0);
			if (facingRight)
				partiQueue.push(HITSPARKL);				//$$$
			else
				partiQueue.push(HITSPARKR);				//$$$
		}

		currentFrame++;
	}
	///hit
	else if (action == ACTION_HIT) {
		//If in Hitstun reduce directional influence
		if (currentFrame != 1) {
			force.x = (inputs->getButton(playerNum, MyInputs::Right) - inputs->getButton(playerNum, MyInputs::Left)) *  diMultiplier;
			if (currentFrame < (unsigned int)hitframes) {//only launched for hitframes, character will just be stunned for the remaining frames (hitstun + moves kb)
				velocity = hitForce = hitForce * 0.99f;			
				partiQueue.push(LAUNCHDUST);				//$$$
			}
			else
				result.RotateZ((float)(currentFrame - hitframes)*(-0.5f + (int)facingRight));
		}
		else {//on first frame pause
			force = glm::vec3(0, 0, 0);
			acceleration = glm::vec3(0, 0, 0);
			velocity = glm::vec3(0, 0, 0);
			if (facingRight) 
				partiQueue.push(HITSPARKL);				//$$$
			else
				partiQueue.push(HITSPARKR);				//$$$
		}

		currentFrame++;
	}
	// AIR ATTACKS
	else if (((inputs->getButtonDown(playerNum, MyInputs::Basic) && inputs->getButton(playerNum, MyInputs::Shift)) && (action == ACTION_FALL || action == ACTION_JUMP || action == ACTION_JUMP2 || (action == ACTION_DASH && position.y > floor))) || action == ACTION_A_BASIC_ALT) {//left or right & A in air = fair
	result = aBasicAlt();
	}
	else if (((inputs->getButtonDown(playerNum, MyInputs::Clear) && inputs->getButton(playerNum, MyInputs::Shift)) && (action == ACTION_FALL || action == ACTION_JUMP || action == ACTION_JUMP2 || (action == ACTION_DASH && position.y > floor))) || action == ACTION_A_CLEAR_ALT) {//left or right & A in air = fair
	result = aClearAlt();
	}
	else if (((inputs->getButtonDown(playerNum, MyInputs::Meteor) && inputs->getButton(playerNum, MyInputs::Shift)) && (action == ACTION_FALL || action == ACTION_JUMP || action == ACTION_JUMP2 || (action == ACTION_DASH && position.y > floor))) || action == ACTION_A_METEOR_ALT) {//left or right & A in air = fair
	result = aMeteorAlt();
	}
	else if ((inputs->getButtonDown(playerNum, MyInputs::Meteor) && (action == ACTION_FALL || (action == ACTION_DASH && position.y > floor))) || action == ACTION_A_METEOR) {//down & A and in air = Dair
		result = aMeteor();
	}
	else if ((inputs->getButtonDown(playerNum, MyInputs::Clear) && (action == ACTION_FALL || (action == ACTION_DASH && position.y > floor))) || action == ACTION_A_CLEAR) {//up & A and in air = Uair
		result = aClear();
	}
	else if ((inputs->getButtonDown(playerNum, MyInputs::Basic) && (action == ACTION_FALL || (action == ACTION_DASH && position.y > floor))) || action == ACTION_A_BASIC) {//just A in air = nair
		result = aBasic();
	}

	///prejump
	else if ((inputs->getButtonDown(playerNum, MyInputs::Jump) && (action == ACTION_IDLE || action == ACTION_RUN || action == ACTION_WALK || action == ACTION_INTIAL_DASH || (action == ACTION_DASH && position.y <= floor))) || action == ACTION_PREJUMP) {
		result = prejump();
		if (inputs->getButtonDown(playerNum, MyInputs::Left))
			jumpForceX = -0.5f * jumpForce;
		else if (inputs->getButtonDown(playerNum, MyInputs::Right))
			jumpForceX = 0.5f * jumpForce;
		else jumpForceX = 0;

	}

	// GROUNDED ATTACKS
	else if ((inputs->getButtonDown(playerNum, MyInputs::Meteor) && inputs->getButton(playerNum, MyInputs::Shift) && !(action == ACTION_FALL || action == ACTION_JUMP || action == ACTION_JUMP2 || (action == ACTION_DASH && position.y > floor))) || action == ACTION_G_METEOR_ALT) {//left or right + B = side Special
		result = gMeteorAlt();
	}
	else if ((inputs->getButtonDown(playerNum, MyInputs::Clear) && inputs->getButton(playerNum, MyInputs::Shift) && !(action == ACTION_FALL || action == ACTION_JUMP || action == ACTION_JUMP2 || (action == ACTION_DASH && position.y > floor))) || action == ACTION_G_CLEAR_ALT) {//down + B = down Special
		result = gClearAlt();
	}
	else if ((inputs->getButtonDown(playerNum, MyInputs::Basic) && inputs->getButton(playerNum, MyInputs::Shift) && !(action == ACTION_FALL || action == ACTION_JUMP || action == ACTION_JUMP2 || (action == ACTION_DASH && position.y > floor))) || action == ACTION_G_BASIC_ALT) {//left or right & A = Ftilt
		result = gBasicAlt();
	}
	else if ((inputs->getButtonDown(playerNum, MyInputs::Meteor) && !(action == ACTION_FALL || action == ACTION_JUMP || action == ACTION_JUMP2 || (action == ACTION_DASH && position.y > floor))) || action == ACTION_G_METEOR) {//down & A = Dtilt
		result = gMeteor();
	}
	else if ((inputs->getButtonDown(playerNum, MyInputs::Clear) && !(action == ACTION_FALL || action == ACTION_JUMP || action == ACTION_JUMP2 || (action == ACTION_DASH && position.y > floor))) || action == ACTION_G_CLEAR) {//down & A = Dtilt
		result = gClear();
	}
	else if ((inputs->getButtonDown(playerNum, MyInputs::Basic) && !(action == ACTION_FALL || action == ACTION_JUMP || action == ACTION_JUMP2 || (action == ACTION_DASH && position.y > floor))) || action == ACTION_G_BASIC) {//just A = jab
		result = gBasic();
	}

	///dash
	else if ((inputs->getButtonDown(playerNum, MyInputs::Dash) && (action == ACTION_IDLE || action == ACTION_WALK || action == ACTION_RUN || action == ACTION_INTIAL_DASH || action == ACTION_FALL || action == ACTION_JUMP || action == ACTION_JUMP2)) || action == ACTION_DASH) {

		result = dash(inputs->getButton(playerNum, MyInputs::Right), inputs->getButton(playerNum, MyInputs::Left));
	}
	///jump
	else if (action == ACTION_JUMP) {
		result = jump();
		force.x = (inputs->getButton(playerNum, MyInputs::Right) - inputs->getButton(playerNum, MyInputs::Left))* 0.5f *  airAccel;
		if (inputs->getButton(playerNum, MyInputs::Left) || inputs->getButton(playerNum, MyInputs::Right))
			force.x *= 2.0f;
	}
	///jump2
	else if ((inputs->getButtonDown(playerNum, MyInputs::Jump) && action == ACTION_FALL) || action == ACTION_JUMP2) {
		result = jump2();
		force.x = (inputs->getButton(playerNum, MyInputs::Right) - inputs->getButton(playerNum, MyInputs::Left))* 0.5f *  airAccel;
		if (inputs->getButton(playerNum, MyInputs::Left) || inputs->getButton(playerNum, MyInputs::Right))
			force.x *= 2.0f;
	}
	///fall
	else if (action == ACTION_FALL) {
		result = fall();
		force.x = (inputs->getButton(playerNum, MyInputs::Right) - inputs->getButton(playerNum, MyInputs::Left))* 0.5f *  airAccel;
		if (inputs->getButton(playerNum, MyInputs::Left) || inputs->getButton(playerNum, MyInputs::Right))
			force.x *= 2.0f;
	}
	
	//NON-OFFENSIVE
	///intial dash
	else if ((((inputs->getButton(playerNum, MyInputs::Left) || inputs->getButton(playerNum, MyInputs::Right) && (action == ACTION_IDLE || (action == ACTION_WALK && currentFrame < activeFrames*0.5f))) || action == ACTION_INTIAL_DASH) && action != ACTION_RUN)) {
		
		result = initialDash(inputs->getButton(playerNum, MyInputs::Right), inputs->getButton(playerNum, MyInputs::Left));
	}
	///run
	else if (inputs->getButton(playerNum, MyInputs::Left) || inputs->getButton(playerNum, MyInputs::Right) && (action == ACTION_WALK || action == ACTION_RUN)) {

		//dashdancing
		if (facingRight && inputs->getButton(playerNum, MyInputs::Left)) {
			velocity.x *= -0.75f;
			facingRight = false;
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			result = initialDash(inputs->getButton(playerNum, MyInputs::Right), inputs->getButton(playerNum, MyInputs::Left));
		}
		if (!facingRight && inputs->getButton(playerNum, MyInputs::Right)) {
			velocity.x *= -0.75f;
			facingRight = true;
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			result = initialDash(inputs->getButton(playerNum, MyInputs::Right), inputs->getButton(playerNum, MyInputs::Left));
		}
		else
			result = run(inputs->getButton(playerNum, MyInputs::Left) || inputs->getButton(playerNum, MyInputs::Right));
	}
	///walk
	else if ((inputs->getButton(playerNum, MyInputs::Left) || inputs->getButton(playerNum, MyInputs::Right)) && (action == ACTION_WALK || action == ACTION_IDLE)) {
		result = walk(inputs->getButton(playerNum, MyInputs::Left) || inputs->getButton(playerNum, MyInputs::Right));
	}
	///idle
	else if (!(inputs->getButton(playerNum, MyInputs::Left) || inputs->getButton(playerNum, MyInputs::Right)) || action == ACTION_IDLE) {
		result = idle();
	}
	else {
		result = idle();
	}

	return result;

}

Transform Character::idle()
{
	Transform result = Transform();
	if (interuptable == true && action != ACTION_IDLE) {
		action = ACTION_IDLE;
		activeFrames = 27;
		currentFrame = 1;
		interuptable = true;
	}
	else if (action == ACTION_IDLE && currentFrame <= activeFrames) {

		if (currentFrame >= activeFrames) {
			//if action over, goto idle
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return idle();
		}

		//stuff goes here

		//std::cout << "idle" << std::endl;
		
		currentFrame++;
	}
	return result;
}

Transform Character::walk(bool held)
{
	Transform result = Transform();
	if (interuptable == true && action != ACTION_WALK) {
		action = ACTION_WALK;
		activeFrames = 42;
		currentFrame = 1;
		interuptable = true;
	}
	else if (!held) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		return idle();
	}
	else if (action == ACTION_WALK && currentFrame <= activeFrames) {

		if (currentFrame >= activeFrames) {
			//if action over, goto walk
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return walk(held);
		}

		//max speed
		float mag = velocity.x;
		if (velocity.x > runSpeed*0.3f)
			velocity.x = runSpeed * 0.3f;
		if (velocity.x < (0 - (runSpeed * 0.3f)))
			velocity.x = (0 - (runSpeed * 0.3f));

		//stuff goes here
		int direction = (int)facingRight;
		if (facingRight == 0)
			direction = -1;
		force.x = direction * runAccel * 0.3f;
		//std::cout << "walk" << std::endl;

		currentFrame++;
	}
	return result;
}

Transform Character::run(bool held)
{
	Transform result = Transform();
	if (interuptable == true && action != ACTION_RUN) {
		action = ACTION_RUN;
		activeFrames = 30;
		currentFrame = 1;
		interuptable = true;
	}
	else if (!held) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		return walk(true);
	}
	else if (action == ACTION_RUN && currentFrame <= activeFrames) {

		if (currentFrame >= activeFrames) {
			//if action over, goto walk
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return run(held);
		}

		//stuff goes here
		int direction = (int)facingRight;
		if (facingRight == 0)
			direction = -1;

		force.x = direction * runAccel;
		//std::cout << "run" << std::endl;

		currentFrame++;
	}
	return result;
}

Transform Character::initialDash(bool right, bool left)
{
	Transform result = Transform();
	if (interuptable == true && action != ACTION_INTIAL_DASH) {
		action = ACTION_INTIAL_DASH;
		activeFrames = dashLength;
		currentFrame = 1;
		interuptable = true;

		int direction = (int)facingRight;
		if (facingRight == 0) {
			direction = -1;
		}



	}
	else if (action == ACTION_INTIAL_DASH && currentFrame <= activeFrames) {

		if (currentFrame >= activeFrames) {
			//if action over, goto run
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return run(true);
		}
		

		//stuff goes here
		int direction = (int)facingRight;
		if (facingRight == 0) {
			direction = -1;
			partiQueue.push(RDASHDUST);						//$$$
		}
		else {
			partiQueue.push(LDASHDUST);						//$$$
		}
		//dashdancing
		if (direction == 1 && left == true) { 
			velocity.x *= -0.1f; 
			direction *= -1;
			facingRight = false;
			currentFrame = 1;
		}
		if (direction == -1 && right == true) {
			velocity.x *= -0.1f;
			direction *= -1;
			facingRight = true;
			currentFrame = 1;
		}

		force.x = direction * runAccel;

		currentFrame++;
		//std::cout << " dash" << std::endl;
	}
	return result;
}


Transform Character::dash(bool right, bool left)
{
	Transform result = Transform();
	//if (interuptable == true && action != ACTION_DASH) {
	if (action != ACTION_DASH && ((getPosition().y == floor && dashTimer > 15) || (getPosition().y > floor && !usedAirDash1) || (getPosition().y > floor && !usedAirDash2 && jumpsLeft == 0))) {
		action = ACTION_DASH;
		activeFrames = 8;
		currentFrame = 1;

		dashTimer = 0;
		if (jumpsLeft == 1) {
			usedAirDash1 = true;
		}
		else if (jumpsLeft == 0) {
			usedAirDash2 = true;
			usedAirDash1 = true;
		}
		
		interuptable = false;

		//decide which way youll dash
		if (left) {
			velocity.y = 0.0f;
			velocity.x = runSpeed * -2.0f;
			facingRight = false;
		}
		else if (right) {
			velocity.y = 0.0f;
			velocity.x = runSpeed * 2.0f;
			facingRight = true;
		}
	}
	//Auto Finish
	if (action != ACTION_DASH || currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		if (position.y > 0.0f)
			return fall();
		else
			return run(true);//idle()
	}
	else if (action == ACTION_DASH && currentFrame < activeFrames) {

		//stuff goes here
		int direction = (int)facingRight;
		if (facingRight == 0)
			direction = -1;

		velocity.y = 0.0f;
		velocity.x = direction * runSpeed * dashMultiplier;

		currentFrame++;
	}
	return result;
}

Transform Character::prejump()
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
			return jump();
		}

		//stuff goes here

		currentFrame++;
	}
	return result;
}

Transform Character::jump()
{
	Transform result = Transform();
	if (interuptable == true && action != ACTION_JUMP) {
		action = ACTION_JUMP;
		activeFrames = jumpFrames;
		currentFrame = 1;
		interuptable = false;
		velocity.x *= 0.9f;
		partiQueue.push(LANDDUST);						//$$$
	}
	else if (action == ACTION_JUMP && currentFrame <= activeFrames) {

		if (currentFrame == activeFrames) {
			jumpForceX = 0;
			//if action over, goto fall
			action = ACTION_PLACEHOLDER;
			interuptable = true;
			return fall();
		}

		//stuff goes here
		velocity.y = jumpForce;
		velocity.x += jumpForceX;
		currentFrame++;
	}
	return result;
}

Transform Character::jump2()
{
	Transform result = Transform();
	if (interuptable == true && action != ACTION_JUMP2 && jumpsLeft > 0) {
		action = ACTION_JUMP2;
		activeFrames = jumpFrames;
		currentFrame = 1;
		interuptable = false;
		jumpsLeft--;
		velocity.x *= 0.5f;
		partiQueue.push(LANDDUST);						//$$$
	}
	else if (action == ACTION_JUMP2 && currentFrame <= activeFrames) {

		if (currentFrame == activeFrames) {
			//if action over, goto fall
			action = ACTION_PLACEHOLDER;
			interuptable = true;
			return fall();
		}

		//stuff goes here
		velocity.y = jumpForce;

		currentFrame++;
	}
	return result;
}

Transform Character::fall()
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
			return fall();
		}

		//stuff goes here

		currentFrame++;
	}
	return result;
}