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

#define BLOOM_THRESHOLDBRIGHT	0.95f
#define BLOOM_THRESHOLD			0.01f
#define BLOOM_DOWNSCALE			2.0f
#define BLOOM_BLUR_PASSES		20
#define SHADOW_RESOLUTION		8192

class Scene{
	
public:
	Scene(unsigned int, unsigned int, unsigned int); //initlaizes everything in scene
	~Scene(); // unloads all objects

	void init(Character* _players[2], InputHandler* _Controllers);//called from game, passes characters for players
	bool update(); //updates scene, returns true if still running
	void draw(); //draws scene
	unsigned int close();//can be called from game to close scene and get next action


	void sortObjects();
	Object* findObject(std::string _name);
	PointLightObj* findLight(std::string _name);

	void loadShaders();
	void loadParticles();

private:
	// Window Peram
	unsigned int WINDOW_WIDTH;
	unsigned int WINDOW_HEIGHT;
	unsigned int FRAMES_PER_SECOND;

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

	Texture StepTexture;

	// Camera
	Camera GameCamera;
	glm::vec3 seekPoint;

	// Directional Light & Shadow
	Transform ShadowTransform;
	Transform ShadowProjection;
	Transform ViewToShadowMap;

	// Particle Effects
	ParticleEffect DustLand;
	ParticleEffect DustDashL;
	ParticleEffect DustDashR;
	ParticleEffect DustLaunch;
	ParticleEffect HitSparkL;
	ParticleEffect HitSparkR;
	ParticleEffect MeterFlame1;
	ParticleEffect MeterFlame2;

	// Frame Buffers
	FrameBuffer GBuffer; //Utility buffer to hold positions and normals
	FrameBuffer DeferredComposite; //Where the actual scene is loaded to
	FrameBuffer ShadowMap;
	FrameBuffer EdgeMap;
	//Bloom work buffers
	FrameBuffer WorkBuffer1;
	FrameBuffer WorkBuffer2;
	FrameBuffer HudMap;

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