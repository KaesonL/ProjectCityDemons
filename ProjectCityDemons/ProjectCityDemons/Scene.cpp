#include "Scene.h"
#include "Utilities.h"

Scene::Scene(unsigned int width, unsigned int height, unsigned int fps)
	: GBuffer(3), DeferredComposite(1), ShadowMap(0), EdgeMap(1), WorkBuffer1(1), WorkBuffer2(1), HudMap(1), Controllers()
{
	WINDOW_HEIGHT = height;
	WINDOW_WIDTH = width;
	FRAMES_PER_SECOND = fps;
}

Scene::~Scene()
{
}

void Scene::init(Character* _players[2], InputHandler* _Controllers)
{
	//init players & controller system
	Players[0] = _players[0];
	Players[1] = _players[1];
	Controllers = _Controllers;

	//load objects for scene
	Objects.push_back(new Object("./Assets/Models/basicCourtHigh", "./Assets/Textures/basicCourt.png", "stage1_env"));

	//load point lights
	///PointLightObj(*position*, *color*, *name*, *active?* = false by default);
	pointLights.push_back(new PointLightObj(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), "p1Score", false));
	pointLights.push_back(new PointLightObj(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), "p2Score", false));
	pointLights.push_back(new PointLightObj(glm::vec3(-18.0f, 10.0f, -19.0f), glm::vec3(0.5f, 0.5f, 0.5f), "lightLeft", false));
	pointLights.push_back(new PointLightObj(glm::vec3(18.0f, 10.0f, -19.0f), glm::vec3(0.5f, 0.5f, 0.5f), "lightRight", false));
	pointLights.push_back(new PointLightObj(glm::vec3(0.0f, 10.0f, -19.0f), glm::vec3(0.5f, 0.5f, 0.5f), "lightCenter", false));

	//init camera
	GameCamera.CameraProjection = Transform::PerspectiveProjection(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 1.0f, 10000.0f);
	ShadowProjection = Transform::OrthographicProjection(-350.0f, 350.0f, 350.0f, -350.0f, -25.0f, 1000.0f);

	//start timer
	updateTimer = new Timer();

	//init floor & walls
	rightWall = 25;
	leftWall = -25;
	floor = 0;


	//for toon shading
	if (!StepTexture.Load("./Assets/Textures/StepTexture.png")) {
		std::cout << "Step Texture failed to load.\n";
		system("pause");
		exit(0);
	}
	StepTexture.SetNearestFilter();
	///toonActive = true;

	loadShaders();

	loadParticles();

	//Init Scene & Frame Buffers
	GBuffer.InitDepthTexture(WINDOW_WIDTH, WINDOW_HEIGHT);
	GBuffer.InitColorTexture(0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA8, GL_NEAREST, GL_CLAMP_TO_EDGE); //Flat color
	GBuffer.InitColorTexture(1, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB16, GL_NEAREST, GL_CLAMP_TO_EDGE); //Normals (xyz)
	GBuffer.InitColorTexture(2, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB32F, GL_NEAREST, GL_CLAMP_TO_EDGE); //View Space Positions (xyz)
	if (!GBuffer.CheckFBO()) {
		std::cout << "GB FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}
	DeferredComposite.InitColorTexture(0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA8, GL_NEAREST, GL_CLAMP_TO_EDGE);
	if (!DeferredComposite.CheckFBO()) {
		std::cout << "DC FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}
	ShadowMap.InitDepthTexture(SHADOW_RESOLUTION, SHADOW_RESOLUTION);
	if (!ShadowMap.CheckFBO()) {
		std::cout << "SM FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}
	EdgeMap.InitColorTexture(0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_R8, GL_NEAREST, GL_CLAMP_TO_EDGE);
	if (!EdgeMap.CheckFBO()) {
		std::cout << "EM FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}
	WorkBuffer1.InitColorTexture(0, WINDOW_WIDTH / (unsigned int)BLOOM_DOWNSCALE, WINDOW_HEIGHT / (unsigned int)BLOOM_DOWNSCALE, GL_RGB8/*GL_R11F_G11F_B10F*/, GL_LINEAR, GL_CLAMP_TO_EDGE); //These parameters can be changed to whatever you want
	if (!WorkBuffer1.CheckFBO()) {
		std::cout << "WB1 FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}
	WorkBuffer2.InitColorTexture(0, WINDOW_WIDTH / (unsigned int)BLOOM_DOWNSCALE, WINDOW_HEIGHT / (unsigned int)BLOOM_DOWNSCALE, /*GL_R11F_G11F_B10F*/GL_RGB8, GL_LINEAR, GL_CLAMP_TO_EDGE); //These parameters can be changed to whatever you want
	if (!WorkBuffer2.CheckFBO()) {
		std::cout << "WB2 FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}
	HudMap.InitColorTexture(0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA8, GL_NEAREST, GL_CLAMP_TO_EDGE);
	if (!HudMap.CheckFBO()) {
		std::cout << "HudMap FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}

}

bool Scene::update()
{
	// Update Sound
	Sound::engine.update();

	// Update Inputs
	Controllers->update();

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
			//close();
		}
	}
	return true;
}

void Scene::draw()
{

	/// Clear Buffers ///
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0.1f, 0.2f, 0.3f, 0);
	DeferredComposite.Clear();

	glClearColor(0.0f, 0.0f, 0.0f, 0);
	GBuffer.Clear();
	ShadowMap.Clear();
	HudMap.Clear();
	WorkBuffer1.Clear();
	WorkBuffer2.Clear();

	/// Generate The Shadow Map ///
	glViewport(0, 0, SHADOW_RESOLUTION, SHADOW_RESOLUTION);

	GBufferPass->Bind();
	GBufferPass->SendUniformMat4("uModel", Transform().data, true);
	//The reason of the inverse is because it is easier to do transformations
	GBufferPass->SendUniformMat4("uView", ShadowTransform.GetInverse().data, true);
	GBufferPass->SendUniformMat4("uProj", ShadowProjection.data, true);

	ShadowMap.Bind();

	Object* temp;
	temp = findObject("stage1_env");
	glBindVertexArray(temp->body.VAO);
	glDrawArrays(GL_TRIANGLES, 0, temp->body.GetNumVertices());

	//glBindVertexArray(Background.VAO);
	//glDrawArrays(GL_TRIANGLES, 0, Background.GetNumVertices());

	//GBufferPass->SendUniformMat4("uModel",playerOne->transform.data, true);
	//playerOne->drawShadow(GBufferPass, 1);
	//GBufferPass->SendUniformMat4("uModel", playerTwo->transform.data, true);
	//playerTwo->drawShadow(GBufferPass, 1);

	GBufferPass->SendUniformMat4("uModel", Transform().data, true);
	GBufferPass->SendUniformMat4("uModel", Transform().data, true);


	glDisable(GL_CULL_FACE);//should fix random holes in knight

	GBufferPass->UnBind();
	//draw p1 shadow
	AniShader->Bind();
	AniShader->SendUniformMat4("uModel", Transform().data, true);
	AniShader->SendUniformMat4("uView", ShadowTransform.GetInverse().data, true);
	AniShader->SendUniformMat4("uProj", ShadowProjection.data, true);
	AniShader->SendUniformMat4("uModel", Players[0]->transform.data, true);
	Players[0]->draw(*AniShader, 0);
	AniShader->SendUniformMat4("uModel", Transform().data, true);

	//draw p2 shadow
	AniShader->Bind();
	AniShader->SendUniformMat4("uModel", Transform().data, true);
	AniShader->SendUniformMat4("uView", ShadowTransform.GetInverse().data, true);
	AniShader->SendUniformMat4("uProj", ShadowProjection.data, true);
	AniShader->SendUniformMat4("uModel", Players[1]->transform.data, true);
	Players[1]->draw(*AniShader, 0);
	AniShader->SendUniformMat4("uModel", Transform().data, true);

	glEnable(GL_CULL_FACE);//turn it back on after for preformance

	glBindVertexArray(0);

	ShadowMap.UnBind();
	AniShader->UnBind();

	/// Generate The Scene ///
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	GBufferPass->Bind();
	GBufferPass->SendUniformMat4("uModel", Transform().data, true);
	//The reason of the inverse is because it is easier to do transformations
	GBufferPass->SendUniformMat4("uView", GameCamera.CameraTransform.GetInverse().data, true);
	GBufferPass->SendUniformMat4("uProj", GameCamera.CameraProjection.data, true);
	//GBufferPass->SendUniformMat4("ViewToShadowMap", ViewToShadowMap.data, true);

	//MAKE SURE TO KNOW WHAT VIEWSPACE YOU ARE WORKING IN
	GBufferPass->SendUniform("uTex", 0);
	GBuffer.Bind();

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ShadowMap.GetDepthHandle());
	glActiveTexture(GL_TEXTURE0);

	//draws everything in scene
	sortObjects();
	for (int i = 0; i < (int)Objects.size(); i++) {

		GBufferPass->Bind();
		GBufferPass->SendUniformMat4("uModel", Transform().data, true);
		//The reason of the inverse is because it is easier to do transformations
		GBufferPass->SendUniformMat4("uView", GameCamera.CameraTransform.GetInverse().data, true);
		GBufferPass->SendUniformMat4("uProj", GameCamera.CameraProjection.data, true);
		//GBufferPass->SendUniformMat4("ViewToShadowMap", ViewToShadowMap.data, true);

		//MAKE SURE TO KNOW WHAT VIEWSPACE YOU ARE WORKING IN
		GBufferPass->SendUniform("uTex", 0);
		Objects[i]->draw(*GBufferPass, 1);
	}

	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);



	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	glBindVertexArray(0);

	Players[0]->drawBoxes(*GBufferPass);
	Players[1]->drawBoxes(*GBufferPass);


	glDisable(GL_CULL_FACE);//should fix random holes in knight
	//glDisable(GL_BLEND);//should fix random holes in knight

	AniShader->Bind();
	AniShader->SendUniformMat4("uView", GameCamera.CameraTransform.GetInverse().data, true);
	AniShader->SendUniformMat4("uProj", GameCamera.CameraProjection.data, true);
	Players[0]->draw(*AniShader, 1);

	AniShader->Bind();
	AniShader->SendUniformMat4("uView", GameCamera.CameraTransform.GetInverse().data, true);
	AniShader->SendUniformMat4("uProj", GameCamera.CameraProjection.data, true);
	Players[1]->draw(*AniShader, 1);

	glEnable(GL_CULL_FACE);//turn it back on after for preformance

	//Black and white
	///if (grayscale == true)
	///{
	///	GrayScale->Bind();
	///	GrayScale->SendUniform("uTex", 0);
	///
	///	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(0));
	///	DrawFullScreenQuad();
	///	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	///
	///	GrayScale->UnBind();
	///}
	///AniShader->UnBind();
	///GBuffer.UnBind();
	///GBufferPass->UnBind();

	// Detect Edges
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	SobelPass->Bind();
	SobelPass->SendUniform("uNormalMap", 0);
	SobelPass->SendUniform("uDepthMap", 1);
	float stroke = 1.1f;
	SobelPass->SendUniform("uPixelSize", glm::vec2(stroke / WINDOW_WIDTH, stroke / WINDOW_HEIGHT));

	EdgeMap.Bind();
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(1));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetDepthHandle());
	DrawFullScreenQuad();
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	EdgeMap.UnBind();
	SobelPass->UnBind();

	/// Create Scene From GBuffer ///
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	DeferredLighting->Bind();
	DeferredLighting->SendUniformMat4("ViewToShadowMap", ViewToShadowMap.data, true);
	DeferredLighting->SendUniform("uScene", 0);
	DeferredLighting->SendUniform("uShadowMap", 1);
	DeferredLighting->SendUniform("uNormalMap", 2);
	DeferredLighting->SendUniform("uPositionMap", 3);
	DeferredLighting->SendUniform("uEdgeMap", 4);
	DeferredLighting->SendUniform("uStepTexture", 5);
	DeferredLighting->SendUniform("LightDirection", glm::vec3(GameCamera.CameraTransform.GetInverse().getRotationMat() * glm::normalize(ShadowTransform.GetForward())));
	DeferredLighting->SendUniform("LightAmbient", glm::vec3(0.6f, 0.6f, 0.6f)); //You can LERP through colours to make night to day cycles
	DeferredLighting->SendUniform("LightDiffuse", glm::vec3(0.6f, 0.6f, 0.6f));
	DeferredLighting->SendUniform("LightSpecular", glm::vec3(0.6f, 0.6f, 0.6f));
	DeferredLighting->SendUniform("LightSpecularExponent", 500.0f);
	DeferredLighting->SendUniform("uToonActive", true);

	DeferredComposite.Bind();

	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(0));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ShadowMap.GetDepthHandle());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(1));
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(2));
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, EdgeMap.GetColorHandle(0));
	glActiveTexture(GL_TEXTURE5);
	StepTexture.Bind();

	DrawFullScreenQuad();

	//draw additional lights
	glEnable(GL_BLEND);
	//glBlendFunc(GL_ONE, GL_ONE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	PointLight->Bind();
	PointLight->SendUniform("uSceneAlbedo", 0);
	PointLight->SendUniform("uNormalMap", 2);
	PointLight->SendUniform("uPositionMap", 3);
	PointLight->SendUniform("uEdgeMap", 4);
	PointLight->SendUniform("uStepTexture", 5);
	PointLight->SendUniform("uToonActive", true);

	for (int i = 0; i < (int)pointLights.size(); i++) {
		if (pointLights[i]->active == true) {
			pointLights[i]->draw(*PointLight, GameCamera.CameraTransform);
			DrawFullScreenQuad();
		}
	}
	PointLight->UnBind();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);

	glBindTexture(GL_TEXTURE_2D, GL_NONE); //Could I do StepTexture.UnBInd()?
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE); //Why was this not here in week 10 vid?


	//draw particles
	ParticleProgram->Bind();
	ParticleProgram->SendUniform("uTex", 0);
	ParticleProgram->SendUniformMat4("uModel", Transform::Identity().data, true);
	ParticleProgram->SendUniformMat4("uView", GameCamera.CameraTransform.GetInverse().data, true);
	ParticleProgram->SendUniformMat4("uProj", GameCamera.CameraProjection.data, true);

	//render all particles
	DustDashL.Render();
	DustDashR.Render();
	DustLand.Render();
	DustLaunch.Render();
	HitSparkL.Render();
	HitSparkR.Render();
	MeterFlame1.Render();
	MeterFlame2.Render();

	ParticleProgram->UnBind();


	DeferredComposite.UnBind();
	DeferredLighting->UnBind();

	//drawHUD();

	/// Compute High Pass ///

	BloomHighPass->Bind();
	BloomHighPass->SendUniform("bloomOn", true);
	BloomHighPass->UnBind();

	glViewport(0, 0, (GLsizei)(WINDOW_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(WINDOW_HEIGHT / BLOOM_DOWNSCALE));

	//Moving data to the back buffer, at the same time as our last post process
	BloomHighPass->Bind();
	BloomHighPass->SendUniform("uTex", 0);
		BloomHighPass->SendUniform("uThreshold", BLOOM_THRESHOLD);
		BloomHighPass->SendUniform("uThreshold2", BLOOM_THRESHOLDBRIGHT);

	WorkBuffer1.Bind();

	glBindTexture(GL_TEXTURE_2D, DeferredComposite.GetColorHandle(0));
	DrawFullScreenQuad();
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	WorkBuffer1.UnBind();

	BloomHighPass->UnBind();

	/// Compute Blur ///
	glViewport(0, 0, (GLsizei)(WINDOW_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(WINDOW_HEIGHT / BLOOM_DOWNSCALE));
	for (int i = 0; i < BLOOM_BLUR_PASSES; i++)
	{
		//Horizontal Blur
		BlurHorizontal->Bind();
		BlurHorizontal->SendUniform("uTex", 0);
		BlurHorizontal->SendUniform("uPixelSize", 1.0f / WINDOW_WIDTH);

		WorkBuffer2.Bind();

		glBindTexture(GL_TEXTURE_2D, WorkBuffer1.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);

		WorkBuffer2.UnBind();

		BlurHorizontal->UnBind();

		//Vertical Blur
		BlurVertical->Bind();
		BlurVertical->SendUniform("uTex", 0);
		BlurVertical->SendUniform("uPixelSize", 1.0f / WINDOW_HEIGHT);

		WorkBuffer1.Bind();

		glBindTexture(GL_TEXTURE_2D, WorkBuffer2.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);

		WorkBuffer1.UnBind();

		BlurVertical->UnBind();
	}

	/// Composite To Back Buffer ///
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	BloomComposite->Bind();
	BloomComposite->SendUniform("uScene", 0);
	BloomComposite->SendUniform("uBloom", 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, DeferredComposite.GetColorHandle(0));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, WorkBuffer1.GetColorHandle(0));
	DrawFullScreenQuad();
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	BloomComposite->UnBind();


	glutSwapBuffers();
}

void Scene::loadShaders()
{
	GBufferPass = new ShaderProgram();
	BloomHighPass = new ShaderProgram();
	SobelPass = new ShaderProgram();
	BlurHorizontal = new ShaderProgram();
	BlurVertical = new ShaderProgram();
	BloomComposite = new ShaderProgram();
	DeferredLighting = new ShaderProgram();
	AniShader = new ShaderProgram();
	PointLight = new ShaderProgram();
	ParticleProgram = new ShaderProgram();
	GrayScale = new ShaderProgram();

	if (!GBufferPass->Load("./Assets/Shaders/StaticGeometry.vert", "./Assets/Shaders/GBufferPass.frag")) {
		std::cout << "GBP Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}
	if (!BloomHighPass->Load("./Assets/Shaders/PassThrough.vert", "./Assets/Shaders/Bloom/BloomHighPass.frag")) {
		std::cout << "BHP Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}
	if (!BlurHorizontal->Load("./Assets/Shaders/PassThrough.vert", "./Assets/Shaders/Bloom/BlurHorizontal.frag")) {
		std::cout << "BH Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}
	if (!BlurVertical->Load("./Assets/Shaders/PassThrough.vert", "./Assets/Shaders/Bloom/BlurVertical.frag")) {
		std::cout << "BV Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}
	if (!BloomComposite->Load("./Assets/Shaders/PassThrough.vert", "./Assets/Shaders/Bloom/BloomComposite.frag")) {
		std::cout << "BC Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}
	if (!DeferredLighting->Load("./Assets/Shaders/PassThrough.vert", "./Assets/Shaders/DeferredLighting.frag")) {
		std::cout << "DL Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}
	if (!AniShader->Load("./Assets/Shaders/AnimationShader.vert", "./Assets/Shaders/GBufferPass.frag")) {
		std::cout << "AS Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}
	if (!PointLight->Load("./Assets/Shaders/PassThroughLight.vert", "./Assets/Shaders/PointLight.frag")) {
		std::cout << "SL Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}
	if (!GrayScale->Load("./Assets/Shaders/Passthrough.vert", "./Assets/Shaders/GreyScalePost.frag")) {
		std::cout << "ADS Shaders failed to initialize. \n";
		system("pause");
		exit(0);
	}
	if (!SobelPass->Load("./Assets/Shaders/PassThrough.vert", "./Assets/Shaders/Toon/Sobel.frag")) {
		std::cout << "SP Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}
}

void Scene::loadParticles()
{
	//Particle Program

	if (!ParticleProgram->Load(
		"./Assets/Shaders/Particles/BillBoard.vert",
		"./Assets/Shaders/Particles/BillBoard.frag",
		"./Assets/Shaders/Particles/BillBoard.geom"))
	{
		std::cout << "PP failed to initialize.\n";
		system("pause");
		exit(0);
	}

	if (!DustLand.Init("./Assets/Textures/fog.png", (unsigned int)50, (unsigned int)2))
	{
		std::cout << "Dust Particle-Effect failed ot initialize.\n";
		system("pause");
		exit(0);
	}
	DustLand.LerpAlpha = glm::vec2(0.3f, 0.0f);
	DustLand.LerpSize = glm::vec2(1.0f, 3.0f);
	DustLand.RangeLifetime = glm::vec2(0.1f, 0.5f);
	DustLand.RangeVelocity = glm::vec2(3.0f, 7.0f);
	DustLand.RangeZ = glm::vec2(-2.1f, -2.0f);
	DustLand.InitialXRange = glm::vec2(-15.0f, 15.0f);
	DustLand.InitialYRange = glm::vec2(0.7f, 1.5f);
	DustLand.HaveGravity = true;
	DustLand.Mass = 1.0f;
	DustLand.Gravity = -0.15f;

	if (!DustDashL.Init("./Assets/Textures/fog.png", (unsigned int)50, (unsigned int)1))
	{
		std::cout << "Dust Particle-Effect failed ot initialize.\n";
		system("pause");
		exit(0);
	}
	DustDashL.LerpAlpha = glm::vec2(0.5f, 0.0f);
	DustDashL.LerpSize = glm::vec2(1.5f, 3.0f);
	DustDashL.RangeLifetime = glm::vec2(0.1f, 0.5f);
	DustDashL.RangeVelocity = glm::vec2(1.0f, 5.0f);
	DustDashL.RangeZ = glm::vec2(-2.1f, -2.1f);
	DustDashL.InitialXRange = glm::vec2(-5.0f, -3.0f);
	DustDashL.InitialYRange = glm::vec2(0.7f, 1.5f);
	DustDashL.HaveGravity = true;
	DustDashL.Mass = 1.0f;
	DustDashL.Gravity = -0.15f;

	if (!DustDashR.Init("./Assets/Textures/fog.png", (unsigned int)50, (unsigned int)1))
	{
		std::cout << "Dust Particle-Effect failed ot initialize.\n";
		system("pause");
		exit(0);
	}
	DustDashR.LerpAlpha = glm::vec2(0.5f, 0.0f);
	DustDashR.LerpSize = glm::vec2(1.5f, 3.0f);
	DustDashR.RangeLifetime = glm::vec2(0.1f, 0.5f);
	DustDashR.RangeVelocity = glm::vec2(1.0f, 5.0f);
	DustDashR.RangeZ = glm::vec2(-2.1f, -2.1f);
	DustDashR.InitialXRange = glm::vec2(3.0f, 5.0f);
	DustDashR.InitialYRange = glm::vec2(0.7f, 1.5f);
	DustDashR.HaveGravity = true;
	DustDashR.Mass = 1.0f;
	DustDashR.Gravity = -0.15f;

	if (!DustLaunch.Init("./Assets/Textures/fog.png", (unsigned int)500, (unsigned int)9))
	{
		std::cout << "Dust Particle-Effect failed ot initialize.\n";
		system("pause");
		exit(0);
	}
	DustLaunch.LerpAlpha = glm::vec2(0.3f, 0.0f);
	DustLaunch.LerpSize = glm::vec2(1.5f, 2.0f);
	DustLaunch.RangeLifetime = glm::vec2(0.1f, 1.5f);
	DustLaunch.RangeVelocity = glm::vec2(1.0f, 2.0f);
	DustLaunch.RangeZ = glm::vec2(1.0f, 1.0f);
	DustLaunch.InitialXRange = glm::vec2(-1.0f, 1.0f);
	DustLaunch.InitialYRange = glm::vec2(-2.0f, 2.0f);
	DustLaunch.HaveGravity = true;
	DustLaunch.Mass = 1.0f;
	DustLaunch.Gravity = 0.01f;

	if (!HitSparkR.Init("./Assets/Textures/Spark.png", (unsigned int)500, (unsigned int)30))
	{
		std::cout << "Spark Particle-Effect failed ot initialize.\n";
		system("pause");
		exit(0);
	}
	HitSparkR.LerpAlpha = glm::vec2(0.5f, 0.0f);
	HitSparkR.LerpSize = glm::vec2(0.2f, 0.3f);
	HitSparkR.RangeLifetime = glm::vec2(0.9f, 1.2f);
	HitSparkR.RangeVelocity = glm::vec2(10.0f, 20.0f);
	HitSparkR.RangeZ = glm::vec2(-1.0f, -1.0f);
	HitSparkR.InitialXRange = glm::vec2(-0.1f, 1.0f);
	HitSparkR.InitialYRange = glm::vec2(-0.4f, 0.9f);
	HitSparkR.HaveGravity = true;
	HitSparkR.Mass = 1.0f;
	HitSparkR.Gravity = 0.6f;
	HitSparkR.noiseOn = false;
	HitSparkR.noiseStrength = 1;

	if (!HitSparkL.Init("./Assets/Textures/Spark.png", (unsigned int)500, (unsigned int)30))
	{
		std::cout << "Spark Particle-Effect failed ot initialize.\n";
		system("pause");
		exit(0);
	}
	HitSparkL.LerpAlpha = glm::vec2(0.5f, 0.0f);
	HitSparkL.LerpSize = glm::vec2(0.2f, 0.3f);
	HitSparkL.RangeLifetime = glm::vec2(0.9f, 1.2f);
	HitSparkL.RangeVelocity = glm::vec2(10.0f, 20.0f);
	HitSparkL.RangeZ = glm::vec2(-1.0f, -1.0f);
	HitSparkL.InitialXRange = glm::vec2(-1.0f, 0.1f);
	HitSparkL.InitialYRange = glm::vec2(-0.4f, 0.9f);
	HitSparkL.HaveGravity = true;
	HitSparkL.Mass = 1.0f;
	HitSparkL.Gravity = 0.6f;
	HitSparkL.noiseOn = false;
	HitSparkL.noiseStrength = 1;

	if (!MeterFlame1.Init("./Assets/Textures/redFog.png", (unsigned int)500, (unsigned int)20))
	{
		std::cout << "MeterFlame Particle-Effect failed ot initialize.\n";
		system("pause");
		exit(0);
	}
	MeterFlame1.LerpAlpha = glm::vec2(0.1f, 0.0f);
	MeterFlame1.LerpSize = glm::vec2(1.0f, 1.5f);
	MeterFlame1.RangeLifetime = glm::vec2(0.2f, 0.6f);
	MeterFlame1.RangeVelocity = glm::vec2(1.0f, 5.0f);
	MeterFlame1.RangeZ = glm::vec2(-1.0f, -1.0f);
	MeterFlame1.InitialXRange = glm::vec2(-5.0f, 5.0f);
	MeterFlame1.InitialYRange = glm::vec2(0.0f, 1.0f);
	MeterFlame1.HaveGravity = true;
	MeterFlame1.Mass = 1.0f;
	MeterFlame1.Gravity = -0.3f;
	MeterFlame1.noiseOn = false;
	MeterFlame1.noiseStrength = 10;

	if (!MeterFlame2.Init("./Assets/Textures/blueFog.png", (unsigned int)500, (unsigned int)20))
	{
		std::cout << "MeterFlame Particle-Effect failed ot initialize.\n";
		system("pause");
		exit(0);
	}
	MeterFlame2.LerpAlpha = glm::vec2(0.1f, 0.0f);
	MeterFlame2.LerpSize = glm::vec2(1.0f, 1.5f);
	MeterFlame2.RangeLifetime = glm::vec2(0.2f, 0.6f);
	MeterFlame2.RangeVelocity = glm::vec2(1.0f, 5.0f);
	MeterFlame2.RangeZ = glm::vec2(-1.0f, -1.0f);
	MeterFlame2.InitialXRange = glm::vec2(-5.0f, 5.0f);
	MeterFlame2.InitialYRange = glm::vec2(0.0f, 1.0f);
	MeterFlame2.HaveGravity = true;
	MeterFlame2.Mass = 1.0f;
	MeterFlame2.Gravity = -0.3f;
	MeterFlame2.noiseOn = false;
	MeterFlame2.noiseStrength = 10;
}

unsigned int Scene::close()
{
	delete updateTimer;

	BloomHighPass->UnLoad();
	BlurHorizontal->UnLoad();
	BlurVertical->UnLoad();
	BloomComposite->UnLoad();
	GBufferPass->UnLoad();
	DeferredLighting->UnLoad();
	SobelPass->UnLoad();
	AniShader->UnLoad();
	PointLight->UnLoad();
	ParticleProgram->UnLoad();
	StepTexture.Unload();

	return 0;
}

void Scene::sortObjects()
{
	//go through list
	for (int i = 0; i < (int)(Objects.size() - 1); i++) {
		for (int j = i + 1; j < (int)Objects.size(); j++) {
			float dist1 = (float)(GameCamera.CameraTransform.GetTranslation() - Objects[i]->transform.GetTranslation()).length();
			float dist2 = (float)(GameCamera.CameraTransform.GetTranslation() - Objects[j]->transform.GetTranslation()).length();
			if ((dist1 < dist2 && Objects[i]->blending) || (Objects[i]->blending && !Objects[j]->blending)) {
				Object* temp = Objects[i];
				Objects[i] = Objects[j];
				Objects[j] = temp;
			}
		}
	}
}

Object * Scene::findObject(std::string _name)
{
	for (int i = 0; i < (int)Objects.size(); i++) {

		if (Objects[i]->name == _name)
			return Objects[i];

	}
	return nullptr;
}

PointLightObj * Scene::findLight(std::string _name)
{
	for (int i = 0; i < (int)pointLights.size(); i++) {

		if (pointLights[i]->name == _name)
			return pointLights[i];

	}
	return nullptr;
}