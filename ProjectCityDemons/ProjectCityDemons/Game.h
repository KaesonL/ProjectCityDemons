#pragma once
#include <windows.h>
//Must be included before the other includes since these include glew.h and its picky
#include "ShaderProgram.h"
#include "Mesh.h"
#include "Texture.h"
#include "FrameBuffer.h"
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
#include "Scene.h"

//debugging
#include <chrono>
#include <iostream>

//vsync
#include "include/wglext.h"


/*
Credit to:
Emilian Cioca
Gil Robern
Jessica Le
*/

#define WINDOW_WIDTH			1280
#define WINDOW_HEIGHT			720
#define FULLSCREEN_WIDTH		1920
#define FULLSCREEN_HEIGHT		1080
#define FRAMES_PER_SECOND		60
#define BLOOM_THRESHOLDBRIGHT	0.95f
#define BLOOM_THRESHOLD1		0.01f //Threshold value for basic court
#define BLOOM_THRESHOLD2		0.01f //Threshold value for knight court
#define BLOOM_THRESHOLD3		0.01f //Threshold value for ninja court
#define BLOOM_DOWNSCALE			2.0f //The number of times you down sample the pixels (how many times we divide the resolution)
#define BLOOM_BLUR_PASSES		20	//How many times to repeat the blur process?
#define SHADOW_RESOLUTION		8192


class Game
{
public:
	Game();
	~Game();

	void initializeGame();
	void update();
	void draw();

	/* input callback functions */
	void keyboardDown(unsigned char key, int mouseX, int mouseY);
	void keyboardUp(unsigned char key, int mouseX, int mouseY);
	void mouseClicked(int button, int state, int x, int y);
	void mouseMoved(int x, int y);

	/* Data Members */
	Timer *updateTimer = nullptr;
	float TotalGameTime = 0.0f;

	/// Shaders
	ShaderProgram* GBufferPass;
	ShaderProgram* BloomHighPass;
	ShaderProgram* SobelPass;
	ShaderProgram* BlurHorizontal;
	ShaderProgram* BlurVertical;
	ShaderProgram* BloomComposite;
	ShaderProgram* DeferredLighting;
	ShaderProgram* AniShader;
	ShaderProgram* PointLight;
	ShaderProgram* GrayScale;
	ShaderProgram* HPShader;

	/// Character Temps
	charBlueDragon* blueTemp;

	/// Players Characters
	Character* players[2];
	Scene* gameScene;

	Texture StepTexture;

	/// FRAMEBUFFERS ///
	FrameBuffer GBuffer; //Utility buffer to hold positions and normals
	FrameBuffer DeferredComposite; //Where the actual scene is loaded to
	FrameBuffer EdgeMap;
	//Bloom work buffers
	FrameBuffer WorkBuffer1;

	Camera GameCamera;

	Transform ShadowTransform;
	Transform ShadowProjection;
	Transform hudTransform;
	Transform hudProjection;

	Transform ViewToShadowMap;

	void updateInputs();
	bool gameDone = false;

	//controller
	InputHandler* Controllers;

	bool grayscale = false;
	bool toonActive = false;

	///Sound Stuff
	bool soundPlaying = false;

	//Sound gameSound;
	Sound gameTheme;
	Sound select;
	Sound menuMove;

	FMOD_VECTOR defaultPos;

	FMOD::Channel* themeChannel = NULL;
	FMOD::Channel* selectionChannel = NULL;

	FMOD::DSP *pitchShift;
	FMOD::DSP *highPass;

private:

};
