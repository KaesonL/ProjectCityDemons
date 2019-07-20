#include "Game.h"
#include "Utilities.h"

/*
Credit to:
Emilian Cioca
Gil Robern
Jessica Le
John Wang
*/

/*
Keys:
$$$ - Particle Signal

*/

#define VSYNC true

Game::Game()
	: GBuffer(3), DeferredComposite(1), EdgeMap(1), WorkBuffer1(1), Controllers()//, godRaysBuffer1(1), godRaysBuffer2(1)
	//This constructor in the initializer list is to solve the issue of creating a frame buffer object without no default constructor
	//This will occur before the brackets of the constructor starts (Reference at Week #6 video Time: 47:00)
	//The number is the number of color textures
{
}

Game::~Game()
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

	StepTexture.Unload();
}

bool WGLExtensionSupported(const char *extension_name)
{
	// this is pointer to function which returns pointer to string with list of all wgl extensions
	PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = NULL;

	// determine pointer to wglGetExtensionsStringEXT function
	_wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");

	if (strstr(_wglGetExtensionsStringEXT(), extension_name) == NULL)
	{
		// string was not found
		return false;
	}

	// extension is supported
	return true;
}

void Game::initializeGame()
{
	//Only needs to be done once
	glEnable(GL_DEPTH_TEST);
	
	InitFullScreenQuad();

	glutFullScreen();//if no fullscreen, comment out this line

	//init vsync
	PFNWGLSWAPINTERVALEXTPROC       wglSwapIntervalEXT = NULL;
	PFNWGLGETSWAPINTERVALEXTPROC    wglGetSwapIntervalEXT = NULL;
	if (WGLExtensionSupported("WGL_EXT_swap_control")){
		// Extension is supported, init pointers.
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

		// this is another function from WGL_EXT_swap_control extension
		wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapIntervalEXT");
	}
	if (VSYNC)
		wglSwapIntervalEXT(1);
	else
		wglSwapIntervalEXT(0);

	//===================================================================//
	//						Init Characters
	//===================================================================//
	blueTemp = (new charBlueDragon("./Assets/Textures/blueDtextureRed.png"));

	//====================================================================//
	//						Load Shaders
	//====================================================================//
	GBufferPass = new ShaderProgram();
	BloomHighPass = new ShaderProgram();
	SobelPass = new ShaderProgram();
	BlurHorizontal = new ShaderProgram();
	BlurVertical = new ShaderProgram();
	BloomComposite = new ShaderProgram();
	DeferredLighting = new ShaderProgram();
	AniShader = new ShaderProgram();
	PointLight = new ShaderProgram();
	GrayScale = new ShaderProgram();
	HPShader = new ShaderProgram();


	if (!HPShader->Load("./Assets/Shaders/StaticGeometry.vert", "./Assets/Shaders/HPShader.frag")){
		std::cout << "GBP Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}

	if (!GBufferPass->Load("./Assets/Shaders/StaticGeometry.vert", "./Assets/Shaders/GBufferPass.frag")){
		std::cout << "GBP Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}

	if (!BloomHighPass->Load("./Assets/Shaders/PassThrough.vert", "./Assets/Shaders/Bloom/BloomHighPass.frag")){
		std::cout << "BHP Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}

	if (!BlurHorizontal->Load("./Assets/Shaders/PassThrough.vert", "./Assets/Shaders/Bloom/BlurHorizontal.frag")){
		std::cout << "BH Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}

	if (!BlurVertical->Load("./Assets/Shaders/PassThrough.vert", "./Assets/Shaders/Bloom/BlurVertical.frag")){
		std::cout << "BV Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}

	if (!BloomComposite->Load("./Assets/Shaders/PassThrough.vert", "./Assets/Shaders/Bloom/BloomComposite.frag")){
		std::cout << "BC Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}

	if (!DeferredLighting->Load("./Assets/Shaders/PassThrough.vert", "./Assets/Shaders/DeferredLighting.frag")){
		std::cout << "DL Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}

	if (!AniShader->Load("./Assets/Shaders/AnimationShader.vert", "./Assets/Shaders/GBufferPass.frag")){
		std::cout << "AS Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}

	if (!PointLight->Load("./Assets/Shaders/PassThroughLight.vert", "./Assets/Shaders/PointLight.frag")){
		std::cout << "SL Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}

	if (!GrayScale->Load("./Assets/Shaders/Passthrough.vert", "./Assets/Shaders/GreyScalePost.frag")){
		std::cout << "ADS Shaders failed to initialize. \n";
		system("pause");
		exit(0);
	}

	if (!SobelPass->Load("./Assets/Shaders/PassThrough.vert", "./Assets/Shaders/Toon/Sobel.frag")){
		std::cout << "SP Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}
//=======================================================================//
	//Init Scene & Frame Buffers
		GBuffer.InitDepthTexture(FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
		//0 is equal to 1 for the index. To make another color texture it is as easy as changing the list size in the contructor and copying the line below
		//These parameters can be changed to whatever you want
		GBuffer.InitColorTexture(0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT, GL_RGBA8, GL_NEAREST, GL_CLAMP_TO_EDGE); //Flat color
		GBuffer.InitColorTexture(1, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT, GL_RGB16, GL_NEAREST, GL_CLAMP_TO_EDGE); //Normals (xyz)
		//Buffer explained at Week 10 time: 5:30 - 7:45
		GBuffer.InitColorTexture(2, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT, GL_RGB32F, GL_NEAREST, GL_CLAMP_TO_EDGE); //View Space Positions (xyz)
	
	if (!GBuffer.CheckFBO()){
		std::cout << "GB FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}

		DeferredComposite.InitColorTexture(0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT, GL_RGBA8, GL_NEAREST, GL_CLAMP_TO_EDGE);
	if (!DeferredComposite.CheckFBO()){
		std::cout << "DC FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}

	//THis is a single channel texture explained at Week 11 time: ~3:30
	
		EdgeMap.InitColorTexture(0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT, GL_R8, GL_NEAREST, GL_CLAMP_TO_EDGE);
	if (!EdgeMap.CheckFBO()){
		std::cout << "EM FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}

	
		WorkBuffer1.InitColorTexture(0,FULLSCREEN_WIDTH / (unsigned int)BLOOM_DOWNSCALE, FULLSCREEN_HEIGHT / (unsigned int)BLOOM_DOWNSCALE, GL_RGB8/*GL_R11F_G11F_B10F*/, GL_LINEAR, GL_CLAMP_TO_EDGE); //These parameters can be changed to whatever you want
		if (!WorkBuffer1.CheckFBO()){
		std::cout << "WB1 FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}

	
	//================================================================//
	//Camera Init
	//================================================================//

	GameCamera.CameraProjection = Transform::PerspectiveProjection(60.0f, (float)FULLSCREEN_WIDTH / (float)FULLSCREEN_HEIGHT, 1.0f, 10000.0f);
	ShadowProjection = Transform::OrthographicProjection(-350.0f, 350.0f, 350.0f, -350.0f, -25.0f, 1000.0f);
	hudProjection = Transform::OrthographicProjection((float)FULLSCREEN_WIDTH * -0.5f, (float)FULLSCREEN_WIDTH * 0.5f, (float)FULLSCREEN_HEIGHT * 0.5f, (float)FULLSCREEN_HEIGHT * -0.5f, -10.0f, 100.0f);
	
	//start timer
	updateTimer = new Timer();

	//=================================================================//
	//Sound Stuff
	//=================================================================//

	gameTheme.Load("./Assets/Media/GameMusic.mp3", false, true);
	select.Load("./Assets/Media/SelectSound.wav", true, false);
	menuMove.Load("./Assets/Media/MenuMove.wav", true, false);
	defaultPos = { 0.0f, 0.0f, 0.0f };

	themeChannel = gameTheme.Play(defaultPos, defaultPos, true);
	themeChannel->setVolume(0.7f);

	//Set Priorities, 0 being the highest priority and 256 being the lowest priority
	themeChannel->setPriority(256);
	selectionChannel->setPriority(10);
	//Create a pitch shift effect
	Sound::engine.system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &pitchShift);
	Sound::engine.system->createDSPByType(FMOD_DSP_TYPE_HIGHPASS, &highPass);

	//init Controller System
	Controllers = new InputHandler();

	//toon shading ON
	toonActive = true;

	//start fight
	gameScene = new Scene(FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT, FRAMES_PER_SECOND);
	players[0] = new charBlueDragon(blueTemp);
	players[1] = new charBlueDragon(blueTemp);
	players[1]->bodyTexture.Load("./Assets/Textures/blueDtextureBlue.png");
	gameScene->init(players, Controllers);
}


void Game::update()
{
	Sound::engine.update();
	
	if (gameScene) {
		gameScene->update();
	}
}


void Game::draw()
{
	
	if (gameScene) {
		gameScene->draw();
	}
}

void Game::keyboardDown(unsigned char key, int mouseX, int mouseY)
{
	//std::cout << key << " : " << (int)key << std::endl;
	switch (key)
	{
	case 27: // the escape key
		exit(0);
		break;
	case 'q': 
		
		break;
	}
}

void Game::keyboardUp(unsigned char key, int mouseX, int mouseY)
{
	switch (key)
	{
	case 'W': //w
	case 'w': //w

		break;
	case 'b': //m
		if (grayscale == false)
			grayscale = true;
		else if (grayscale == true)
			grayscale = false;
		break;
	case 't':
		if (toonActive)
		{
			toonActive = false;
			std::cout << "toon OFF" << std::endl;
		}
		else if (!toonActive)
		{
			toonActive = true;
			std::cout << "toon ON" << std::endl;
		}
		break;
	case '.': //a
		DeferredLighting->ReloadShader();
		std::cout << "Reloaded Shaders\n";
		break;
	}
}

void Game::mouseClicked(int button, int state, int x, int y)
{
	if(state == GLUT_DOWN) 
	{
		switch(button)
		{
		case GLUT_LEFT_BUTTON:

			break;
		case GLUT_RIGHT_BUTTON:
		
		    
			break;
		case GLUT_MIDDLE_BUTTON:

			break;
		}
	}
	else
	{

	}
}

/*
 * mouseMoved(x,y)
 * - this occurs only when the mouse is pressed down
 *   and the mouse has moved.  you are given the x,y locations
 *   in window coordinates (from the top left corner) and thus 
 *   must be converted to screen coordinates using the screen to window pixels ratio
 *   and the y must be flipped to make the bottom left corner the origin.
 */
void Game::mouseMoved(int x, int y)
{
}


void Game::updateInputs()
{
	Controllers->update();
}
