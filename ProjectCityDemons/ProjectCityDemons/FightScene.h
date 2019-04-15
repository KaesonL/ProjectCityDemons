#include <iostream>
#include <SDL/SDL.h>
#include "display.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "transform.h"
#include "camera.h"
#include "character.h"
#include <vector>

static const int DISPLAY_WIDTH = 1280;
static const int DISPLAY_HEIGHT = 720;

class FightScene {
public:

	FightScene(int numPlayers);
	void loop();

private:
	Display* display;
	Character* playerOne;
	Character* playerTwo;
	Camera* camera;
	int players;

	std::vector<bool> inputs;
	std::vector<bool> inputs2;

	//floor
	Mesh* bodyFloor;
	Shader* shaderFloor;
	Texture* textureFloor;

};