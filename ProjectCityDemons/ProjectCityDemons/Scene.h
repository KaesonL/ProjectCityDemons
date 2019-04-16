#pragma once
#include <windows.h>
//Must be included before the other includes since these include glew.h and its picky
#include "ShaderProgram.h"
#include "Mesh.h"
#include "Texture.h"
#include "FrameBuffer.h"
#include "ParticleEffect.h"
#include "Object.h"
#include "PointLightObj.h"
#include "Camera.h"
#include "FmodWrapper.h"
#include "FMOD/inc/fmod.hpp"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "char_blue.h"
#include "InputHandler.h"
#pragma comment(lib, "Xinput9_1_0.lib") 
#include "Timer.h"

//debugging
#include <chrono>
#include <iostream>

//vsync
#include "include/wglext.h"

class Scene{
	
public:
	Scene(); //initlaizes everything in scene
	~Scene(); // unloads all objects

	void init(Character* players[2]);//called from game, passes characters for players
	bool update(); //updates scene, returns true if still running
	void draw(); //draws scene
	unsigned int close();//cane called from game to close scene and get next action

private:
	//scene objects
	std::vector<Object*> gameObjects;
	std::vector<PointLightObj*> pointLights;
	Character* Players[2];
	InputHandler* Controllers;//updating players and inputs could be done in game?

	//timer stuff
	Timer *updateTimer = nullptr;
	float TotalGameTime = 0.0f;
	std::vector<Texture*> time;

};