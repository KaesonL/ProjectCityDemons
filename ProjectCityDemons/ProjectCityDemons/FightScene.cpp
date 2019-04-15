#include "FightScene.h"

FightScene::FightScene(int numPlayers) {
	display = new Display(DISPLAY_WIDTH, DISPLAY_HEIGHT, "OpenGL");
	playerOne = new Character("./res/HumanBodyClothed.obj", "./res/basicShader", "./res/blackhatlogoshirt2.png");
	camera = new Camera(glm::vec3(0.0f, 0.0f, -5.0f), 70.0f, (float)DISPLAY_WIDTH / (float)DISPLAY_HEIGHT, 0.1f, 100.0f);
	camera->Pitch(0.1f);
	players = numPlayers;
	if (numPlayers == 2) {
		playerTwo = new Character("./res/humanbody.obj", "./res/basicShader", "./res/bricks.jpg");
	}
	inputs = { false, false, false, false, false, false }; //up, left, down, right, A, B
	inputs2 = { false, false, false, false, false, false }; //up, left, down, right, A, B
	//loop();


	bodyFloor = new Mesh("./res/floor.obj");
	shaderFloor = new Shader("./res/basicShader");
	textureFloor = new Texture("./res/tile.jpg");
	

}

void FightScene::loop() {
	SDL_Event e;
	bool isRunning = true;
	float counter = 0.0f;
	while (isRunning)
	{
		while (SDL_PollEvent(&e))
		{
			//Inputs
			if (e.type == SDL_QUIT)
				isRunning = false;
			if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym == SDLK_LEFT)
					inputs[1] = true;
				if (e.key.keysym.sym == SDLK_RIGHT)
					inputs[3] = true;
				if (e.key.keysym.sym == SDLK_UP)
					inputs[0] = true;
				if (e.key.keysym.sym == SDLK_DOWN)
					inputs[2] = true;
				if (e.key.keysym.sym == SDLK_KP_1)
					inputs[4] = true;
				if (e.key.keysym.sym == SDLK_KP_2)
					inputs[5] = true;
				if (e.key.keysym.sym == SDLK_a)
					inputs2[1] = true;
				if (e.key.keysym.sym == SDLK_d)
					inputs2[3] = true;
				if (e.key.keysym.sym == SDLK_w)
					inputs2[0] = true;
				if (e.key.keysym.sym == SDLK_s)
					inputs2[2] = true;
				if (e.key.keysym.sym == SDLK_n)
					inputs2[4] = true;
				if (e.key.keysym.sym == SDLK_m)
					inputs2[5] = true;
			}
			if (e.type == SDL_KEYUP) {
				if (e.key.keysym.sym == SDLK_LEFT)
					inputs[1] = false;
				if (e.key.keysym.sym == SDLK_RIGHT)
					inputs[3] = false;
				if (e.key.keysym.sym == SDLK_UP)
					inputs[0] = false;
				if (e.key.keysym.sym == SDLK_DOWN)
					inputs[2] = false;
				if (e.key.keysym.sym == SDLK_KP_1)
					inputs[4] = false;
				if (e.key.keysym.sym == SDLK_KP_2)
					inputs[5] = false;
				if (e.key.keysym.sym == SDLK_a)
					inputs2[1] = false;
				if (e.key.keysym.sym == SDLK_d)
					inputs2[3] = false;
				if (e.key.keysym.sym == SDLK_w)
					inputs2[0] = false;
				if (e.key.keysym.sym == SDLK_s)
					inputs2[2] = false;
				if (e.key.keysym.sym == SDLK_n)
					inputs2[4] = false;
				if (e.key.keysym.sym == SDLK_m)
					inputs2[5] = false;
			}
		}

		display->Clear(0.0f, 0.0f, 0.0f, 1.0f);

		//character collisions
		///Allow Player to pass through one another, but will disallow them to stand in the same spot, will slowly push eachother awayy
		float diffx = (playerOne->getPosition().x) - (playerTwo->getPosition().x);//difference between characters x
		float diffy = (playerOne->getPosition().y) - (playerTwo->getPosition().y);//difference between characters y
		if (abs(diffx) < 0.15f && abs(diffx) != 0 && abs(diffy) < 0.5f) {//if they are touching
			//push them off
			playerOne->setPosition(playerOne->getPosition() + glm::vec3(((diffx / abs(diffx))*0.01f), 0, 0));
			playerTwo->setPosition(playerTwo->getPosition() + glm::vec3(((diffx / abs(diffx))*-0.01f), 0, 0));
		}

		//hitbox collisions placeholder
		int i = 0;
		bool done = false;
		while(done == false) {
			if (i >= playerTwo->getHitboxes().size()) {
				done = true;
			}
			else {
				float diffHx = playerOne->getPosition().x - playerTwo->getHitboxes()[i]->getPosition().x;//difference between characters x
				float diffHy = playerOne->getPosition().y - playerTwo->getHitboxes()[i]->getPosition().y;//difference between characters y
				if (abs(diffHx) < 0.15f && (diffHy > -1.5f && diffHy < 0.1f)) {
					playerOne->hit(playerTwo->getHitboxes()[i]);
					playerTwo->comboAdd();
					playerTwo->getHitboxes()[i]->setDone();
					done = true;
					
				}
			}
			i++;
		}
		i = 0;
		done = false;
		while (done == false) {
			if (i >= playerOne->getHitboxes().size()) {
				done = true;
			}
			else {
				float diffHx = playerTwo->getPosition().x - playerOne->getHitboxes()[i]->getPosition().x;//difference between characters x
				float diffHy = playerTwo->getPosition().y - playerOne->getHitboxes()[i]->getPosition().y;//difference between characters y
				if (abs(diffHx) < 0.15f + (playerOne->getHitboxes()[i]->getSize() *0.1f) && (diffHy > -1.5f - (playerOne->getHitboxes()[i]->getSize() *0.1f) && diffHy < 0.1f + +(playerOne->getHitboxes()[i]->getSize() *0.1f))) {
					playerTwo->hit(playerOne->getHitboxes()[i]);
					playerOne->comboAdd();
					playerOne->getHitboxes()[i]->setDone();
					done = true;
				}
			}
			i++;
		}



		Transform move1(glm::vec3(0.0f, -2.0f, 0.5f), glm::vec3(-0.5f, 0.0f, 0.0f));
		//draw
		shaderFloor->Bind();
		textureFloor->Bind();
		shaderFloor->Update(move1, *camera);
		bodyFloor->Draw();

		//camera control
		float dist = abs(playerOne->getPosition().x - playerTwo->getPosition().x) * -1.0f;
		if (dist < -10) dist = -10;
		camera->setPositionXY((playerTwo->getPosition().x + playerOne->getPosition().x) / 2.0f, abs(dist*0.1f) + 1.5f + ((playerTwo->getPosition().y + playerOne->getPosition().y) / 2.0f));
		if (dist > -5) dist = -5;
		camera->setPositionZ(dist);

		//updates players (will be changed to a list later)
		playerOne->update(counter, *camera, inputs);
		playerTwo->update(counter, *camera, inputs2);


		//Swap Buffer
		display->SwapBuffers();
		SDL_Delay(1000.0f/70);
		counter += 1.0f;
	}
}

