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
	void sortObjects(unsigned int scene);
	Object* findObjects(unsigned int scene, std::string _name);
	PointLightObj* findLight(std::string _name);

	void initializeGame();
	void update();
	void updateTutScreen();
	void updateEndScreen();
	void updateScene();
	void updateMenu();
	void updateCSS();
	void updateSSS();
	void draw();
	void drawTutScreen();
	void drawEndScreen();
	void drawScene();
	void drawCSS();
	void drawSSS();
	void drawHUD();
	void drawTime();
	void drawScore();
	void drawMenu();
	void loadTime();
	/* input callback functions */
	void keyboardDown(unsigned char key, int mouseX, int mouseY);
	void keyboardUp(unsigned char key, int mouseX, int mouseY);
	void mouseClicked(int button, int state, int x, int y);
	void mouseMoved(int x, int y);

	/* Data Members */
	Timer *updateTimer = nullptr;
	float TotalGameTime = 0.0f;

	/// PROGRAMS ///
	ShaderProgram StaticGeometry;
	ShaderProgram GBufferPass;
	ShaderProgram BloomHighPass;
	ShaderProgram SobelPass;
	ShaderProgram BlurHorizontal;
	ShaderProgram BlurVertical;
	ShaderProgram BloomComposite;
	ShaderProgram DeferredLighting;
	ShaderProgram AniShader;
	ShaderProgram PointLight;
	ShaderProgram ParticleProgram;
	ShaderProgram AdShader;
	ShaderProgram GrayScale;
	ShaderProgram NetShader;
	ShaderProgram HPShader;

	Mesh boxMesh;
	Texture boxTexture;

	charBlueDragon* knightTemp;
	//charRedDevil* ninjaTemp;

	Character* players[2];

	std::vector<bool> inputs;
	std::vector<bool> inputs2;

	/// ENTITIES ///
	std::vector<Object*> gameObjects;
	std::vector<PointLightObj*> pointLights;
	Object* hitboxObj;
	std::vector<Object*> menuObjects;
	std::vector<Object*> cssObjects;
	std::vector<Object*> sssObjects;
	std::vector<Object*> endObjects;
	std::vector<Object*> tutObjects;

	unsigned int scene = 0; //0=menu, 1=selecting, 2=fighting
	bool gameDone = false;
	bool stun = true;
	int ultRumbleTimer = 0;

	//Menu
	unsigned int selectedButton = 0;
	float lastInputTime = 0.0f;

	//CSS
	unsigned int p1Char = 0;
	unsigned int p2Char = 0;
	bool p1Done = false;
	bool p2Done = false;

	//SSS
	unsigned int stageVal = 0;
	unsigned int oldStageVal = 0;
	bool stageDone = false;
	std::vector<std::string> stage1_env_objs;
	std::vector<std::string> stage2_env_objs;
	std::vector<std::string> stage3_env_objs;

	//End Screen
	bool endGame = false;
	bool p1KnightWin = false;
	bool p2KnightWin = false;
	bool p1NinjaWin = false;
	bool p2NinjaWin = false;
	bool tieGame = false;

	Mesh HudObj;
	Texture HudBack0;
	Texture HudBack1;
	Texture Overlay0;
	Texture Overlay1;
	Texture Bar0;
	Texture Bar1;
	Texture P1Line;
	Texture P2Line;
	Texture StepTexture;
	//Particle Effects
	ParticleEffect ConfettiEffectBlueRight;
	ParticleEffect ConfettiEffectBlueLeft;
	ParticleEffect ConfettiEffectRedRight;
	ParticleEffect ConfettiEffectRedLeft;
	ParticleEffect ConfettiEffectOrangeRight;
	ParticleEffect ConfettiEffectOrangeLeft;
	ParticleEffect ConfettiEffectPurpleRight;
	ParticleEffect ConfettiEffectPurpleLeft;
	ParticleEffect basicLeftNet;
	ParticleEffect basicRightNet;
	ParticleEffect basicLeftNet1;
	ParticleEffect basicRightNet1;
	ParticleEffect knightLeftNet;
	ParticleEffect knightRightNet;
	ParticleEffect knightLeftNet1;
	ParticleEffect knightRightNet1;
	ParticleEffect ninjaLeftNet;
	ParticleEffect ninjaRightNet;
	ParticleEffect ninjaLeftNet1;
	ParticleEffect ninjaRightNet1;
	ParticleEffect NinjaPetals;
	ParticleEffect NinjaPetals2;
	ParticleEffect DustLand;
	ParticleEffect DustDashL;
	ParticleEffect DustDashR;

	//new
	ParticleEffect DustLaunch;
	ParticleEffect HitSparkL;
	ParticleEffect HitSparkR;
	ParticleEffect MeterFlame1;
	ParticleEffect MeterFlame2;

	ParticleEffect KnightUltFX;
	ParticleEffect NinjaUltFX1;
	ParticleEffect NinjaUltFX2;


	std::vector<Texture*> time;
	std::vector<Hitbox*>Netbox;//2

	/// FRAMEBUFFERS ///
	FrameBuffer GBuffer; //Utility buffer to hold positions and normals
	FrameBuffer DeferredComposite; //Where the actual scene is loaded to
	FrameBuffer ShadowMap;
	FrameBuffer EdgeMap;
	//Bloom work buffers
	FrameBuffer WorkBuffer1;
	FrameBuffer WorkBuffer2;
	FrameBuffer HudMap;

	//Transform CameraTransform;
	//Transform CameraProjection;
	Camera GameCamera;
	glm::vec3 seekPoint;

	Transform ShadowTransform;
	Transform ShadowProjection;
	Transform hudTransform;
	Transform hudProjection;

	Transform ViewToShadowMap;

	///God Rays Stuff
	/*FrameBuffer godRaysBuffer1;
	FrameBuffer godRaysBuffer2;

	ShaderProgram godRaysRadialBlur;
	ShaderProgram godRaysComposite;*/

	void updateInputs();

	//walls
	float rightWall;
	float leftWall;
	float floor;

	//controller
	InputHandler* Controllers;

	int score1;
	int score2;

	bool p1Score = false;
	bool p2Score = false;

	bool grayscale = false;
	bool toonActive = false;

	bool isNinja1 = false;
	bool isNinja2 = false;

	///Sound Stuff
	bool soundPlaying = false;
	bool p1Jump1 = false;
	bool p1Jump2 = false;
	bool p2Jump = false;
	bool hornPlaying = false;
	bool thirtyPlaying = false;
	bool onePlaying = false;
	//bool knightJump;

	bool soundPitched = false;
	bool soundNormalized = true;
	bool soundHighPassed = false;

	float decreaseVal = 1.0f;

	//Sound gameSound;
	Sound gameTheme;
	Sound knightJump;
	Sound ninjaJump;
	Sound cheer;
	Sound horn;
	Sound mumble;
	Sound select;
	Sound menuMove;
	Sound assassin;
	Sound gaurdian;
	Sound oneMin;
	Sound thirtySec;

	FMOD_VECTOR defaultPos;
	FMOD_VECTOR p1Pos;
	FMOD_VECTOR p2Pos;
	//FMOD_VECTOR gameSoundPos;

	FMOD::Channel* themeChannel = NULL;
	FMOD::Channel* mumbleChannel = NULL;
	FMOD::Channel* knightChannel = NULL;
	FMOD::Channel* ninjaChannel = NULL;
	FMOD::Channel* otherChannel = NULL;
	FMOD::Channel* selectionChannel = NULL;

	FMOD::DSP *pitchShift;
	FMOD::DSP *highPass;

private:

};
