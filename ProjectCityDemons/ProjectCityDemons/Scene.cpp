#include "Scene.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::init(Character * players[2])
{

}

bool Scene::update()
{
	// Update Clock
	updateTimer->tick();
	float deltaTime = updateTimer->getElapsedTimeSeconds();
	TotalGameTime += deltaTime;

	// Character Facing
	if (Players[0]->getPosition().x > Players[1]->getPosition().x) {
		if (Players[0]->action == Players[0]->ACTION_IDLE || Players[0]->action == Players[0]->ACTION_FALL)
			Players[0]->facingRight = false;
		if (Players[1]->action == Players[1]->ACTION_IDLE || Players[1]->action == Players[1]->ACTION_FALL)
			Players[1]->facingRight = true;
	}
	else {
		if (Players[0]->action == Players[0]->ACTION_IDLE || Players[0]->action == Players[0]->ACTION_FALL)
			Players[0]->facingRight = true;
		if (Players[1]->action == Players[1]->ACTION_IDLE || Players[1]->action == Players[1]->ACTION_FALL)
			Players[1]->facingRight = false;
	}

	// Dynamic Screen Walls
	leftWall = max(GameCamera.getPosition().x - 10, -25.0f);
	rightWall = min(GameCamera.getPosition().x + 10, 25.0f);
	Players[0]->leftWall = leftWall;
	Players[0]->rightWall = rightWall;
	Players[1]->leftWall = leftWall;
	Players[1]->rightWall = rightWall;
	Players[0]->floor = floor;
	Players[1]->floor = floor;

	// Character Collision
	///Allow Player to pass through one another, but will disallow them to stand in the same spot, will slowly push eachother awayy
	float diffx = (Players[0]->getPosition().x) - (Players[1]->getPosition().x);///difference between characters x
	float diffy = (Players[0]->getPosition().y) - (Players[1]->getPosition().y);///difference between characters y
	if (abs(diffx) < 0.7f && abs(diffx) != 0 && abs(diffy) < 1.7f) {///if they are touching
		///push them off
		Players[0]->setPosition(Players[0]->getPosition() + glm::vec3(((diffx / abs(diffx))*0.01f), 0, 0));
		Players[1]->setPosition(Players[1]->getPosition() + glm::vec3(((diffx / abs(diffx))*-0.01f), 0, 0));
	}

	// Rumble first 5 frames of either player being hit;
	for (int i = 0; i < 2; i++) {
		if (Players[i]->isHit() && Players[i]->currentFrame < 6) {
			Controllers->SetVibration(0, 10, 10);//controller 0, power 10 on left and right
			Controllers->SetVibration(1, 10, 10);//controller 1, power 10 on left and right
			GameCamera.setRumble(4, 0.04f);
			Players[i]->activeTexture = &(Players[i]->hurtTexture);
		}
		else if (Players[i]->isHit() && Players[i]->currentFrame == 7) {
			Controllers->SetVibration(0, 0, 0);//controller 0, power 0 on left and right (off)
			Controllers->SetVibration(1, 0, 0);//controller , power 0 on left and right (off)
			Players[i]->activeTexture = &(Players[i]->bodyTexture);
		}
	}

	// Hitbox Collision
	///for each player
	for (unsigned int attacker = 0; attacker < 2; attacker++) {
		unsigned int defender = !attacker;///other player
		///for each hitbox the attacker has
		for (unsigned int i = 0; i < Players[attacker]->getHitboxes().size(); i++) {
			///for each hurtbox on the other player
			for (unsigned int j = 0; j < Players[defender]->getHurtboxes().size(); j++) {
				glm::vec3 diff = Players[attacker]->getHitboxes()[i]->getPosition() - Players[defender]->getHurtboxes()[j]->getPosition();///vector between centers
				float size = (Players[attacker]->getHitboxes()[i]->getSize() + Players[defender]->getHurtboxes()[j]->getSize()) *0.5f;///sum of sizes
				/// if theyre touching
				if (glm::length(diff) < size) {
					Controllers->SetVibration(attacker, 10, 10);
					Controllers->SetVibration(defender, 10, 10);
					Sleep(40 + (int)(Players[attacker]->getHitboxes()[i]->getKnockback() * 3));
					Controllers->SetVibration(attacker, 0, 0);
					Controllers->SetVibration(defender, 0, 0);
					/// if Blocking & in an action where they can block
					if ((((Controllers->getButton(1, MyInputs::Left) && !Players[attacker]->getHitboxes()[i]->facingRight) || (Controllers->getButton(1, MyInputs::Right) && Players[attacker]->getHitboxes()[i]->facingRight)))
						&& (Players[defender]->action == Players[defender]->ACTION_FALL || Players[defender]->action == Players[defender]->ACTION_JUMP || Players[defender]->action == Players[defender]->ACTION_JUMP2 ||
							Players[defender]->action == Players[defender]->ACTION_PREJUMP || Players[defender]->action == Players[defender]->ACTION_WALK || Players[defender]->action == Players[defender]->ACTION_RUN)) {
						///if holding back, and in a action wheret they can block
						///block
					}
					///if not Blocking (hit)
					else {
						if (Players[defender]->action != Players[defender]->ACTION_HIT) {///if defender not in hitstun, reset combo counter
							Players[attacker]->comboClear();
							Players[attacker]->resetTimer();
						}
						else {
							Players[attacker]->comboAdd();
						}
						Players[defender]->hit(Players[attacker]->getHitboxes()[i]);
					}
					///stall
					Players[attacker]->setVelocity(glm::vec3(Players[attacker]->getVelocity().x, 0.08f, 0.0f));
					Players[defender]->setVelocity(glm::vec3(Players[defender]->getVelocity().x, 0.0f, 0.0f));
					///delete hitbox
					Players[attacker]->getHitboxes()[i]->setDone();
					/// for loops
					i = 100;
					j = 100;
				}
			}
		}
	}

	// Update Players
	Players[0]->update((int)deltaTime, Controllers, 0);
	Players[1]->update((int)deltaTime, Controllers, 1);

	// Particle Calls
	for (int i = 0; i < 2; i++) {
		///check 2 per frame
		///only checking 1 per frame caused issues with spawn timing (if statement)
		///checking everything in queue every frame caused issues with random frame drops (while loop)

		if (Players[0]->partiQueue.size() > 0) {
			switch (Players[0]->partiQueue.front())
			{
			case LANDDUST:
				DustLand.RangeX = glm::vec2(Players[0]->getPosition().x + 0.5f, Players[0]->getPosition().x - 0.5f);
				DustLand.RangeY = glm::vec2(Players[0]->getPosition().y - 1.4f, Players[0]->getPosition().y - 1.6f);
				DustLand.Spawn(0.05f);
				break;
			case LDASHDUST:
				DustDashL.RangeX = glm::vec2(Players[0]->getPosition().x, Players[0]->getPosition().x - 1.0f);
				DustDashL.RangeY = glm::vec2(Players[0]->getPosition().y - 1.4f, Players[0]->getPosition().y - 1.6f);
				DustDashL.Spawn(0.05f);
				break;
			case RDASHDUST:
				DustDashR.RangeX = glm::vec2(Players[0]->getPosition().x + 1.0f, Players[0]->getPosition().x);
				DustDashR.RangeY = glm::vec2(Players[0]->getPosition().y - 1.4f, Players[0]->getPosition().y - 1.6f);
				DustDashR.Spawn(0.05f);
				break;
			case LAUNCHDUST:
				DustLaunch.RangeX = glm::vec2(Players[0]->getPosition().x + 0.7f, Players[0]->getPosition().x - 0.7f);
				DustLaunch.RangeY = glm::vec2(Players[0]->getPosition().y + 1.5f, Players[0]->getPosition().y + 3.0f);
				DustLaunch.Spawn(0.05f);
				break;
			case HITSPARKL:
				HitSparkL.RangeX = glm::vec2(Players[0]->getPosition().x, Players[0]->getPosition().x);
				HitSparkL.RangeY = glm::vec2(Players[0]->getPosition().y + 1.5f, Players[0]->getPosition().y + 1.5f);
				HitSparkL.Spawn(0.05f);
				break;
			case HITSPARKR:
				HitSparkR.RangeX = glm::vec2(Players[0]->getPosition().x, Players[0]->getPosition().x);
				HitSparkR.RangeY = glm::vec2(Players[0]->getPosition().y + 1.5f, Players[0]->getPosition().y + 1.5f);
				HitSparkR.Spawn(0.05f);
				break;
			case METERFLAME:
				MeterFlame1.RangeX = glm::vec2(Players[0]->getPosition().x + 0.7f, Players[0]->getPosition().x - 1.7f);
				MeterFlame1.RangeY = glm::vec2(Players[0]->getPosition().y - 0.7f, Players[0]->getPosition().y + 4.0f);
				MeterFlame1.Spawn(0.001f);
				break;
			default:
				break;
			}

			Players[0]->partiQueue.pop();
		}
		if (Players[1]->partiQueue.size() > 0) {
			//cout << "Spawned";
			switch (Players[1]->partiQueue.front())
			{
			case LANDDUST:
				DustLand.RangeX = glm::vec2(Players[1]->getPosition().x + 0.5f, Players[1]->getPosition().x - 0.5f);
				DustLand.RangeY = glm::vec2(Players[1]->getPosition().y - 1.4f, Players[1]->getPosition().y - 1.6f);
				DustLand.Spawn(0.05f);
				break;
			case LDASHDUST:
				DustDashL.RangeX = glm::vec2(Players[1]->getPosition().x + 1.0f, Players[1]->getPosition().x - 1.0f);
				DustDashL.RangeY = glm::vec2(Players[1]->getPosition().y - 1.4f, Players[1]->getPosition().y - 1.6f);
				DustDashL.Spawn(0.05f);
				break;
			case RDASHDUST:
				DustDashR.RangeX = glm::vec2(Players[1]->getPosition().x + 1.0f, Players[1]->getPosition().x - 1.0f);
				DustDashR.RangeY = glm::vec2(Players[1]->getPosition().y - 1.4f, Players[1]->getPosition().y - 1.6f);
				DustDashR.Spawn(0.05f);
				break;
			case LAUNCHDUST:
				DustLaunch.RangeX = glm::vec2(Players[1]->getPosition().x + 0.7f, Players[1]->getPosition().x - 0.7f);
				DustLaunch.RangeY = glm::vec2(Players[1]->getPosition().y + 1.5f, Players[1]->getPosition().y + 3.0f);
				DustLaunch.Spawn(0.05f);
				break;
			case HITSPARKL:
				HitSparkL.RangeX = glm::vec2(Players[1]->getPosition().x, Players[1]->getPosition().x);
				HitSparkL.RangeY = glm::vec2(Players[1]->getPosition().y + 1.5f, Players[1]->getPosition().y + 1.5f);
				HitSparkL.Spawn(0.05f);
				break;
			case HITSPARKR:
				HitSparkR.RangeX = glm::vec2(Players[1]->getPosition().x, Players[1]->getPosition().x);
				HitSparkR.RangeY = glm::vec2(Players[1]->getPosition().y + 1.5f, Players[1]->getPosition().y + 1.5f);
				HitSparkR.Spawn(0.05f);
				break;
			case METERFLAME:
				MeterFlame2.RangeX = glm::vec2(Players[1]->getPosition().x + 0.7f, Players[1]->getPosition().x - 1.7f);
				MeterFlame2.RangeY = glm::vec2(Players[1]->getPosition().y - 0.7f, Players[1]->getPosition().y + 4.0f);
				MeterFlame2.Spawn(0.001f);
				break;
			default:
				break;
			}

			Players[1]->partiQueue.pop();

		}
	}

	// Camera
	///camera control using seek point and target zoom
	seekPoint.x = (Players[1]->getPosition().x + Players[0]->getPosition().x) * 0.5f;//seek point is inbetween the 2 players
	seekPoint.y = ((Players[1]->getPosition().y + Players[0]->getPosition().y) * 0.5f) - 2;//seek point is inbetween the 2 players
	///camera bounds
	if (abs(seekPoint.x) > 25) {
		seekPoint.x /= abs(seekPoint.x);
		seekPoint.x *= 25;
	}
	if (seekPoint.y > 13) {
		seekPoint.y /= abs(seekPoint.y);
		seekPoint.y *= 25;
	}
	if (seekPoint.y < 0) {
		seekPoint.y = 0;
	}

	GameCamera.targetZoom = 5;
	GameCamera.seekPoint = seekPoint + glm::vec3(0, 5, 0);
	GameCamera.update();

	// Shadows
	ShadowTransform = Transform::Identity();
	ShadowTransform.RotateX(130.0f);
	ShadowTransform.RotateY(180.0f);//make light look down
	ShadowTransform.Translate(glm::vec3(10.0f, 20.0f, -20.0f));
	//ShadowTransform.RotateY(180.0f);

	Transform bias = Transform(0.5f, 0.0f, 0.0f, 0.5f,
		0.0f, 0.5f, 0.0f, 0.5f,
		0.0f, 0.0f, 0.5f, 0.5f,
		0.0f, 0.0f, 0.0f, 1.0f);

	ViewToShadowMap = Transform::Identity();
	ViewToShadowMap = bias * ShadowProjection * ShadowTransform.GetInverse() * GameCamera.CameraTransform;

	// PARTICLE EFFECTS
	/// Update Patricle Effects
	DustDashL.Update(deltaTime);
	DustDashR.Update(deltaTime);
	DustLand.Update(deltaTime);
	DustLaunch.Update(deltaTime);
	HitSparkL.Update(deltaTime);
	HitSparkR.Update(deltaTime);
	MeterFlame1.Update(deltaTime);
	MeterFlame2.Update(deltaTime);

	//End Game
	if (Players[0]->dead == true || Players[1]->dead == true || TotalGameTime >= 99)
	{
		///Make game black and white
		///grayscale = true;

		///Delay timer before going to results screen
		static float tempTime;
		tempTime += updateTimer->getElapsedTimeSeconds();

		///play game end sound
		sfxChannel = gameEnd.Play(defaultPos, defaultPos, false);

		///Check who won
		if (Players[1]->dead == true) {
			
		}
		else if (Players[0]->dead == true) {
			
		}
		else {
			///time out or quit
			///check who has less health
		}

		if (tempTime >= 1.5f){
			close();
		}
	}
}

void Scene::draw()
{
}

unsigned int Scene::close()
{
	return 0;
}

void Scene::sortObjects()
{
}

Object * Scene::findObject(std::string _name)
{
	return nullptr;
}

PointLightObj * Scene::findLight(std::string _name)
{
	return nullptr;
}
