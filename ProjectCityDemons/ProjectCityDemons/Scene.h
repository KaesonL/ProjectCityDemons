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
	unsigned int close();//can be called from game to close scene and get next action


	void sortObjects();
	Object* findObject(std::string _name);
	PointLightObj* findLight(std::string _name);

private:

	// Entities
	std::vector<Object*> Objects;
	std::vector<PointLightObj*> pointLights;
	Character* Players[2];

	// Collision Positions
	float rightWall;
	float leftWall;
	float floor;

	// Input Handler Pointer
	InputHandler* Controllers;

	// Shaders
	ShaderProgram* GBufferPass;
	ShaderProgram* BloomHighPass;
	ShaderProgram* SobelPass;
	ShaderProgram* BlurHorizontal;
	ShaderProgram* BlurVertical;
	ShaderProgram* BloomComposite;
	ShaderProgram* DeferredLighting;
	ShaderProgram* GrayScale;
	ShaderProgram* PointLight;
	ShaderProgram* AniShader;
	ShaderProgram* ParticleProgram;

	// Camera
	Camera GameCamera;
	glm::vec3 seekPoint;

	// Directional Light & Shadow
	Transform ShadowTransform;
	Transform ShadowProjection;
	Transform ViewToShadowMap;

	// Particle Effects

	//Particle Effects
	ParticleEffect DustLand;
	ParticleEffect DustDashL;
	ParticleEffect DustDashR;
	ParticleEffect DustLaunch;
	ParticleEffect HitSparkL;
	ParticleEffect HitSparkR;
	ParticleEffect MeterFlame1;
	ParticleEffect MeterFlame2;

	// Timer stuff
	Timer *updateTimer = nullptr;
	float TotalGameTime = 0.0f;

	// Sound
	Sound musicTrack;
	Sound hitFX;
	Sound gameEnd;

	FMOD_VECTOR defaultPos;

	FMOD::Channel* musicChannel = NULL;
	FMOD::Channel* sfxChannel = NULL;
	FMOD::Channel* p1Channel = NULL;
	FMOD::Channel* p2Channel = NULL;
};