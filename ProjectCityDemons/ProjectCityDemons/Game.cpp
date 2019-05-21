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
	: GBuffer(3), DeferredComposite(1), ShadowMap(0), EdgeMap(1), WorkBuffer1(1), WorkBuffer2(1), HudMap(1), Controllers()//, godRaysBuffer1(1), godRaysBuffer2(1)
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
	ParticleProgram->UnLoad();

	HudObj.Unload();
	Overlay0.Unload();
	Overlay1.Unload();
	Bar0.Unload();
	Bar1.Unload();
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


//=================================================================//
	//Load All Game Objects
	///Object(*mesh path*, *texture path*, *enable blending?* = false by deafult);
		
	//load objects for scene
	gameObjects.push_back(new Object("./Assets/Models/basicCourtHigh", "./Assets/Textures/basicCourt.png", "stage1_env"));

	//load objects for knight scene
	gameObjects.push_back(new Object("./Assets/Models/knightCourtHigh", "./Assets/Textures/knightCourt.png", "stage2_env"));

	//load objects for ninja scene
	gameObjects.push_back(new Object("./Assets/Models/ninjaCourt", "./Assets/Textures/ninjaCourt.png", "stage3_env"));

	gameObjects.push_back(new Object("./Assets/Models/ninjaBack", "./Assets/Textures/ninjaBack.png", "ninja_background"));
	stage3_env_objs.push_back("ninja_background");
	gameObjects.push_back(new Object("./Assets/Models/ninjaBridge", "./Assets/Textures/ninjaBridge.png", "ninja_bridge"));
	stage3_env_objs.push_back("ninja_bridge");
	gameObjects.push_back(new Object("./Assets/Models/ninjaSides", "./Assets/Textures/ninjaSides.png", "ninja_sides"));
	stage3_env_objs.push_back("ninja_sides");
	gameScene = new Scene(FULLSCREEN_HEIGHT, FULLSCREEN_WIDTH, FRAMES_PER_SECOND);

	//Load All Main Menu Objects
	menuObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/titleScreen.png", "background", true));
		menuObjects[0]->setScale(glm::vec3(FULLSCREEN_WIDTH *0.35f, FULLSCREEN_HEIGHT *0.51f, 1));
	menuObjects[0]->RotateY(90);
	menuObjects[0]->setPosition(glm::vec3(0, -555, -1));
	

	menuObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/startButton.png", "button1", true));
		menuObjects[1]->setScale(300.0f);
	menuObjects[1]->RotateY(90.0f);
	menuObjects[1]->setPosition(glm::vec3(570, -250, 0));


	menuObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/tutButton.png", "button2", true));
		menuObjects[2]->setScale(300.0f);
	menuObjects[2]->RotateY(90.0f);
	menuObjects[2]->setPosition(glm::vec3(570, -450, 0));

	menuObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/quitButton.png", "button3", true));
		menuObjects[3]->setScale(300.0f);
	menuObjects[3]->RotateY(90.0f);
	menuObjects[3]->setPosition(glm::vec3(570, -650, 0));


	//Load All Character Select Objects

	///background image
	cssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/selectback.png", "background", true));
		cssObjects[0]->setScale(glm::vec3(FULLSCREEN_WIDTH *0.35f, FULLSCREEN_HEIGHT *0.51f, 1));
	cssObjects[0]->RotateY(90);
	cssObjects[0]->setPosition(glm::vec3(0, -555, -1));

	///knight Icon
	cssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/KnightIcon.png", "knightIcon", true));
		cssObjects[1]->setScale(120.0f);
	cssObjects[1]->RotateY(90.0f);
	cssObjects[1]->setPosition(glm::vec3(-200, -100, 0));

	///ninja Icon
	cssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/NinjaIcon.png", "ninjaIcon", true));
		cssObjects[2]->setScale(120.0f);
	cssObjects[2]->RotateY(90.0f);
	cssObjects[2]->setPosition(glm::vec3(200, -100, 0));

	///p1 Icon
	cssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/p1select.png", "p1Select", true));
		cssObjects[3]->setScale(120.0f);
	cssObjects[3]->RotateY(90.0f);
	cssObjects[3]->setPosition(glm::vec3(-200, -100, 2));

	///p2 Icon
	cssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/p2select.png", "p2Select", true));
		cssObjects[4]->setScale(120.0f);
	cssObjects[4]->RotateY(90.0f);
	cssObjects[4]->setPosition(glm::vec3(200, -100, 1));


	///p1 knight pic
	cssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/P1KnightPic.png", "p1KnightPic", true));
		cssObjects[5]->setScale(300.0f);
	cssObjects[5]->RotateY(90.0f);
	cssObjects[5]->setPosition(glm::vec3(-530, -550, 2));

	///p2 knight pic
	cssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/P2KnightPic.png", "p2KnightPic", true));
		cssObjects[6]->setScale(300.0f);
	cssObjects[6]->RotateY(90.0f);
	cssObjects[6]->setPosition(glm::vec3(530, -550, 1));

	///p1 ninja pic
	cssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/P1NinjaPic.png", "p1NinjaPic", true));
		cssObjects[7]->setScale(300.0f);
	cssObjects[7]->RotateY(90.0f);
	cssObjects[7]->setPosition(glm::vec3(-530, -550, 2));
	cssObjects[7]->hide = true;

	///p2 nijna pic
	cssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/P2NinjaPic.png", "p2NinjaPic", true));
		cssObjects[8]->setScale(300.0f);
	cssObjects[8]->RotateY(90.0f);
	cssObjects[8]->setPosition(glm::vec3(530, -550, 1));
	cssObjects[8]->hide = true;
	// Stage Select Screen

///background image
	sssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/stageSelect.png", "background", true));
		sssObjects[0]->setScale(glm::vec3(FULLSCREEN_WIDTH *0.35f, FULLSCREEN_HEIGHT *0.51f, 1));
	sssObjects[0]->RotateY(90);
	sssObjects[0]->setPosition(glm::vec3(0, -555, -1));

	///knight stage Icon
	sssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/basicCourtSelect.png", "courtIconDefault", true));
		sssObjects[1]->setScale(120.0f);
	sssObjects[1]->RotateY(90.0f);
	sssObjects[1]->setPosition(glm::vec3(-350, -500, 0));

	///default stage Icon
	sssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/knightCourtSelect.png", "courtIconKnight", true));
		sssObjects[2]->setScale(120.0f);
	sssObjects[2]->RotateY(90.0f);
	sssObjects[2]->setPosition(glm::vec3(0, -500, 0));

	///ninja stage Icon
	sssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/ninjaCourtSelect.png", "courtIconNinja", true));
		sssObjects[3]->setScale(120.0f);
	sssObjects[3]->RotateY(90.0f);
	sssObjects[3]->setPosition(glm::vec3(350, -500, 0));

	///select Icon
	sssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/p1select.png", "select", true));
		sssObjects[4]->setScale(120.0f);
	sssObjects[4]->RotateY(90.0f);
	sssObjects[4]->setPosition(glm::vec3(-350, -500, 2));

	///basic court pic
	sssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/basicStage.png", "courtPicDefault", true));
		sssObjects[5]->setScale(glm::vec3(500.0f, 350.0f, 1.0f));
	sssObjects[5]->RotateY(90.0f);
	sssObjects[5]->setPosition(glm::vec3(0, -300, 0));

	///knight court pic
	sssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/knightStage.png", "courtPicKnight", true));
		sssObjects[6]->setScale(glm::vec3(500.0f, 350.0f, 1.0f));
	sssObjects[6]->RotateY(90.0f);
	sssObjects[6]->setPosition(glm::vec3(0, -300, 0));

	///ninja court pic
	sssObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/ninjaStage.png", "courtPicNinja", true));
		sssObjects[7]->setScale(glm::vec3(500.0f, 350.0f, 1.0f));
	sssObjects[7]->RotateY(90.0f);
	sssObjects[7]->setPosition(glm::vec3(0, -300, 0));

	//End Screen Assets
	///Background image
	endObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/knightRedWin.png", "redKnightEnd", true));
		endObjects[0]->setScale(glm::vec3(FULLSCREEN_WIDTH *0.35f, FULLSCREEN_HEIGHT *0.51f, 1));
	endObjects[0]->RotateY(90);
	endObjects[0]->setPosition(glm::vec3(0, -555, -1));

	endObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/knightBlueWin.png", "blueKnightEnd", true));
		endObjects[1]->setScale(glm::vec3(FULLSCREEN_WIDTH *0.35f, FULLSCREEN_HEIGHT *0.51f, 1));
	endObjects[1]->RotateY(90);
	endObjects[1]->setPosition(glm::vec3(0, -555, -1));

	endObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/ninjaRedWin.png", "redNinjaRed", true));
		endObjects[2]->setScale(glm::vec3(FULLSCREEN_WIDTH *0.35f, FULLSCREEN_HEIGHT *0.51f, 1));
	endObjects[2]->RotateY(90);
	endObjects[2]->setPosition(glm::vec3(0, -555, -1));

	endObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/ninjaBlueWin.png", "blueNinjaEnd", true));
		endObjects[3]->setScale(glm::vec3(FULLSCREEN_WIDTH *0.35f, FULLSCREEN_HEIGHT *0.51f, 1));
	endObjects[3]->RotateY(90);
	endObjects[3]->setPosition(glm::vec3(0, -555, -1));

	endObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/tie.png", "tieGameEnd", true));
		endObjects[4]->setScale(glm::vec3(FULLSCREEN_WIDTH *0.35f, FULLSCREEN_HEIGHT *0.51f, 1));
	endObjects[4]->RotateY(90);
	endObjects[4]->setPosition(glm::vec3(0, -555, -1));

	//Tutorial Screen Assets
	///Background image
	tutObjects.push_back(new Object("./Assets/Models/UI_Object", "./Assets/Textures/howTo.png", "tutorial", true));
	tutObjects[0]->setScale(glm::vec3(FULLSCREEN_WIDTH *0.35f, FULLSCREEN_HEIGHT *0.51f, 1));
	tutObjects[0]->RotateY(90);
	tutObjects[0]->setPosition(glm::vec3(0, -555, -1));

	//================================================================//
	//Init PointLights
	//================================================================//

	///PointLightObj(*position*, *color*, *name*, *active?* = false by default);
	pointLights.push_back(new PointLightObj(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), "p1Score", false));
	pointLights.push_back(new PointLightObj(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), "p2Score", false));
	pointLights.push_back(new PointLightObj(glm::vec3(-18.0f, 10.0f, -19.0f), glm::vec3(0.5f, 0.5f, 0.5f), "lightLeft", false));
	pointLights.push_back(new PointLightObj(glm::vec3(18.0f, 10.0f, -19.0f), glm::vec3(0.5f, 0.5f, 0.5f), "lightRight", false));
	pointLights.push_back(new PointLightObj(glm::vec3(0.0f, 10.0f, -19.0f), glm::vec3(0.5f, 0.5f, 0.5f), "lightCenter", false));


	//================================================================//
	//Load Hud Obj and Texture
	//================================================================//

	std::vector<std::string> hud1;
	hud1.push_back("./Assets/Models/UI_Object");
	HudObj.LoadFromFile(hud1);

	if (!HudBack0.Load("./Assets/Textures/HudBack0.png")){
		std::cout << "BKG Texture failed to load.\n";
		system("pause");
		exit(0);
	}
	if (!Overlay0.Load("./Assets/Textures/Overlay0.png")){
		std::cout << "BKG Texture failed to load.\n";
		system("pause");
		exit(0);
	}
	if (!Bar0.Load("./Assets/Textures/Bar0.png")){
		std::cout << "BKG Texture failed to load.\n";
		system("pause");
		exit(0);
	}
	if (!HudBack1.Load("./Assets/Textures/HudBack1.png")){
		std::cout << "BKG Texture failed to load.\n";
		system("pause");
		exit(0);
	}
	if (!Overlay1.Load("./Assets/Textures/Overlay1.png")){
		std::cout << "BKG Texture failed to load.\n";
		system("pause");
		exit(0);
	}
	if (!Bar1.Load("./Assets/Textures/Bar1.png")){
		std::cout << "BKG Texture failed to load.\n";
		system("pause");
		exit(0);
	}

	//for toon shading
	if (!StepTexture.Load("./Assets/Textures/StepTexture.png")){
		std::cout << "Step Texture failed to load.\n";
		system("pause");
		exit(0);
	}
	StepTexture.SetNearestFilter();

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
	ParticleProgram = new ShaderProgram();
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
	//Load the god rays shaders here

//================================================================//
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

	ShadowMap.InitDepthTexture(SHADOW_RESOLUTION, SHADOW_RESOLUTION);
	if (!ShadowMap.CheckFBO()){
		std::cout << "SM FBO failed to initialize.\n";
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

	
		WorkBuffer2.InitColorTexture(0, FULLSCREEN_WIDTH / (unsigned int)BLOOM_DOWNSCALE, FULLSCREEN_HEIGHT / (unsigned int)BLOOM_DOWNSCALE, /*GL_R11F_G11F_B10F*/GL_RGB8, GL_LINEAR, GL_CLAMP_TO_EDGE); //These parameters can be changed to whatever you want
		if (!WorkBuffer2.CheckFBO()){
		std::cout << "WB2 FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}
	//0 is equal to 1 for the index. To make another color texture it is as easy as changing the list size in the contructor and copying the line below
	//These parameters can be changed to whatever you want
		HudMap.InitColorTexture(0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT, GL_RGBA8, GL_NEAREST, GL_CLAMP_TO_EDGE);
	if (!HudMap.CheckFBO()){
		std::cout << "HudMap FBO failed to initialize.\n";
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

	//load textures for in game timer
	loadTime();

	//=================================================================//
	//Sound Stuff
	//=================================================================//

	gameTheme.Load("./Assets/Media/GameMusic.wav", false, true);
	knightJump.Load("./Assets/Media/KnightJump.wav", true, false);
	ninjaJump.Load("./Assets/Media/NinjaJump.wav", true, false);
	cheer.Load("./Assets/Media/CheerTemp.wav", true, false);
	horn.Load("./Assets/Media/Horn.wav", true, false);
	mumble.Load("./Assets/Media/Mumble.wav", true, true);
	select.Load("./Assets/Media/SelectSound.wav", true, false);
	menuMove.Load("./Assets/Media/MenuMove.wav", true, false);
	assassin.Load("./Assets/Media/Assassin.wav", true, false);
	gaurdian.Load("./Assets/Media/Gaurdian.wav", true, false);
	oneMin.Load("./Assets/Media/1Minute.wav", true, false);
	thirtySec.Load("./Assets/Media/30Seconds.wav", true, false);

	defaultPos = { 0.0f, 0.0f, 0.0f };
	p1Pos = { 0.0f, 0.0f, 0.0f };
	p2Pos = { 0.0f, 0.0f, 0.0f };

	themeChannel = gameTheme.Play(defaultPos, defaultPos, true);
	themeChannel->setVolume(0.7f);
	mumbleChannel = mumble.Play(defaultPos, defaultPos, true);

	//Set Priorities, 0 being the highest priority and 256 being the lowest priority
	themeChannel->setPriority(256);
	mumbleChannel->setPriority(255);
	selectionChannel->setPriority(10);
	otherChannel->setPriority(10);
	knightChannel->setPriority(0);
	ninjaChannel->setPriority(0);

	//Create a pitch shift effect
	Sound::engine.system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &pitchShift);
	Sound::engine.system->createDSPByType(FMOD_DSP_TYPE_HIGHPASS, &highPass);

	//init Controller System
	Controllers = new InputHandler();

	//init floor & walls
	rightWall = 25;
	leftWall = -25;
	floor = 0;

	//toon shading ON
	toonActive = true;
}


void Game::update()
{
	Sound::engine.update();
	//auto end = chrono::steady_clock::now();
	//auto start = chrono::steady_clock::now();
	if (scene == 5)
	{
		updateTutScreen();

		if (!soundNormalized)
		{
			mumble.Stop(mumbleChannel);
			//themeChannel->removeDSP(pitchShift);
			gameTheme.Stop(themeChannel);
			themeChannel = gameTheme.Play(defaultPos, defaultPos, true);
			themeChannel->setVolume(0.7f);


			mumbleChannel = mumble.Play(defaultPos, defaultPos, true);

			soundNormalized = true;
			soundPitched = false;
			soundHighPassed = false;
		}
	}
	else if (scene == 4)
	{
		updateEndScreen();

		if (!soundHighPassed)
		{
			mumble.Stop(mumbleChannel);
			gameTheme.Stop(themeChannel);
			themeChannel = gameTheme.Play(defaultPos, defaultPos, true);

			highPass->setParameterFloat(FMOD_DSP_HIGHPASS_CUTOFF, 1000.0f);
			themeChannel->addDSP(0, highPass);

			pitchShift->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 0.8f);
			themeChannel->addDSP(0, pitchShift);

			themeChannel->setVolume(0.8f);

			mumbleChannel = mumble.Play(defaultPos, defaultPos, true);

			soundPitched = false;
			soundNormalized = false;
			soundHighPassed = true;
		}
	}
	else if (scene == 3) {
		updateScene();

		/*if (!soundPlaying)
		{
			soundChannel->setPaused(true);

			gameSoundPos = { 0.0f, 0.0f, 0.0f };
			gameSoundChannel = gameSound.Play(true);
			soundPlaying = true;
		}

		Sound::SetPosition(gameSoundChannel, gameSoundPos);
		Sound::engine.update();*/

		//gameTheme.Stop(themeChannel);
		//themeChannel = gameTheme.Play();

		if (!soundPitched)
		{
			mumble.Stop(mumbleChannel);
			pitchShift->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 0.5f);
			themeChannel->addDSP(0, pitchShift);

			themeChannel->setFrequency(60000.0f);
			
			themeChannel->setVolume(0.7f);

			//mumbleChannel = mumble.Play(deaultPos, deaultPos, true);

			soundPitched = true;
			soundNormalized = false;
			soundHighPassed = false;
		}

	}
	else if (scene == 2) {
		updateSSS();
		
		if (!soundNormalized)
		{
			mumble.Stop(mumbleChannel);
			//themeChannel->removeDSP(pitchShift);
			gameTheme.Stop(themeChannel);
			themeChannel = gameTheme.Play(defaultPos, defaultPos, true);

			themeChannel->setVolume(0.7f);

			mumbleChannel = mumble.Play(defaultPos, defaultPos, true);
			
			soundNormalized = true;
			soundPitched = false;
			soundHighPassed = false;
		}
	}
	else if (scene == 1) {
		updateCSS();
		
		if (!soundNormalized)
		{
			mumble.Stop(mumbleChannel);
			//themeChannel->removeDSP(pitchShift);
			gameTheme.Stop(themeChannel);
			themeChannel = gameTheme.Play(defaultPos, defaultPos, true);

			mumbleChannel = mumble.Play(defaultPos, defaultPos, true);
			
			themeChannel->setVolume(0.7f);

			soundNormalized = true;
			soundPitched = false;
			soundHighPassed = false;
		}
	}
	else {
		updateMenu();
		
		if (!soundNormalized)
		{
			//mumble.Stop(mumbleChannel);
			//themeChannel->removeDSP(pitchShift);
			gameTheme.Stop(themeChannel);
			themeChannel = gameTheme.Play(defaultPos, defaultPos, true);

			mumbleChannel = mumble.Play(defaultPos, defaultPos, true);
			
			themeChannel->setVolume(0.7f);

			soundNormalized = true;
			soundPitched = false;
			soundHighPassed = false;
		}
	}
	//end = chrono::steady_clock::now();
	//cout << chrono::duration_cast<chrono::microseconds>(end - start).count() << ":";

}

void Game::updateTutScreen()
{
	updateTimer->tick();
	float deltaTime = updateTimer->getElapsedTimeSeconds();
	TotalGameTime += deltaTime;

	updateInputs();
	
	//press back to main menu
	if (Controllers->getButtonDown(0, MyInputs::Basic) || Controllers->getButtonDown(1, MyInputs::Basic)) {
		lastInputTime = 0.0f;

		stageDone = false;
		stageVal = 1;
		scene = 0;
		TotalGameTime = 0.0f;
		lastInputTime = 0.0f;
		p1Char = 0;
		p2Char = 0;
		p1Done = false;
		p2Done = false;
		isNinja1 = false;
		isNinja2 = false;
		onePlaying = false;
		thirtyPlaying = false;
	}
}

void Game::updateEndScreen()
{
	// update our clock so we have the delta time since the last update
	updateTimer->tick();
	float deltaTime = updateTimer->getElapsedTimeSeconds();
	TotalGameTime += deltaTime;

	if (TotalGameTime >= 1.5f)
		updateInputs();

	Controllers->SetVibration(0, 0, 0);//controller 0, power 0 on left and right (off)
	Controllers->SetVibration(1, 0, 0);//controller , power 0 on left and right (off)

	//press
	if (Controllers->getButtonDown(0, MyInputs::Start) || Controllers->getButtonDown(1, MyInputs::Start)) {
		//Play Select Sound
		selectionChannel = select.Play(defaultPos, defaultPos, false);
		
		lastInputTime = 0.0f;
		endGame = true;
	}

	//Once A is pressed go back to menu
	if (endGame)
	{
		scene = 0;
		p1Char = 0;
		p2Char = 0;
		p1KnightWin = false;
		p1NinjaWin = false;
		p2KnightWin = false;
		p2NinjaWin = false;
		tieGame = false;
		endGame = false;
		isNinja1 = false;
		isNinja2 = false;
		onePlaying = false;
		thirtyPlaying = false;
		TotalGameTime = 0.0f;
		deltaTime = 0;
		updateTimer = new Timer();
	}
}

void Game::updateMenu()
{
	// update our clock so we have the delta time since the last update
	updateTimer->tick();
	float deltaTime = updateTimer->getElapsedTimeSeconds();
	TotalGameTime += deltaTime;

	updateInputs();

	//check if move input
	unsigned int oldButton = selectedButton;
	if (TotalGameTime - lastInputTime > 0.2f) {
		if (Controllers->getButtonDown(0, MyInputs::Up) || Controllers->getButtonDown(1, MyInputs::Up)) {
			selectedButton--;
			//Play menu option move sound
			selectionChannel = menuMove.Play(defaultPos, defaultPos, false);
		}
		if (Controllers->getButtonDown(0, MyInputs::Down) || Controllers->getButtonDown(1, MyInputs::Down)) {
			selectedButton++;
			//Play menu option move sound
			selectionChannel = menuMove.Play(defaultPos, defaultPos, false);
		}
	}

	//correction
	if (selectedButton < 1)
		selectedButton = 1;
	else if (selectedButton > 3)
			selectedButton = 3;

	//change button sizes
	if (selectedButton != oldButton) {
		lastInputTime = TotalGameTime;
		if (selectedButton == 1) {
				findObjects(0, "button1")->setScale(1.1f * 300.0f);
				findObjects(0, "button2")->setScale((1.0f / 1.1f) * 300.0f);
				findObjects(0, "button3")->setScale((1.0f / 1.1f) * 300.0f);
			
		}
		else if (selectedButton == 2) {
			
				findObjects(0, "button2")->setScale(1.1f * 300.0f);
				findObjects(0, "button1")->setScale((1.0f / 1.1f) * 300.0f);
				findObjects(0, "button3")->setScale((1.0f / 1.1f) * 300.0f);
			
		}
		else if (selectedButton == 3) {
			
				findObjects(0, "button3")->setScale(1.1f * 300.0f);
				findObjects(0, "button1")->setScale((1.0f / 1.1f) * 300.0f);
				findObjects(0, "button2")->setScale((1.0f / 1.1f) * 300.0f);
			
		}
	}

	//press
	if (Controllers->getButtonDown(0, MyInputs::Start) || Controllers->getButtonDown(1, MyInputs::Start)) {

		//Play Select Sound
		selectionChannel = select.Play(defaultPos, defaultPos, false);

		if (selectedButton == 1) {
			scene = 1;
			lastInputTime = 0.0f;
		}
		else if (selectedButton == 2)
		{
			scene = 5;
			lastInputTime = 0.0f;
		}
		else if (selectedButton == 3) {
			exit(0);
		}
	}

}

void Game::updateCSS()
{
	// update our clock so we have the delta time since the last update
	updateTimer->tick();
	float deltaTime = updateTimer->getElapsedTimeSeconds();
	TotalGameTime += deltaTime;

	updateInputs();

	//Go back to main menu
	if (Controllers->getButtonDown(0, MyInputs::Start) || Controllers->getButtonDown(1, MyInputs::Start)) {
		lastInputTime = 0.0f;

		stageDone = false;
		stageVal = 1;
		scene = 0;
		TotalGameTime = 0.0f;
		lastInputTime = 0.0f;
		p1Char = 0;
		p2Char = 0;
		p1Done = false;
		p2Done = false;
		isNinja1 = false;
		isNinja2 = false;
		onePlaying = false;
		thirtyPlaying = false;
	}

	//check inputs
	if (TotalGameTime - lastInputTime > 0.2f) { //This delay needs to be tested
		if (!p1Done) {
			if (Controllers->getButtonDown(0, MyInputs::Left))
			{
				lastInputTime = TotalGameTime;
				p1Char--;
				//Play menu option move sound
				selectionChannel = menuMove.Play(defaultPos, defaultPos, false);
			}
			else if (Controllers->getButtonDown(0, MyInputs::Right))
			{
				lastInputTime = TotalGameTime;
				p1Char++;
				//Play menu option move sound
				selectionChannel = menuMove.Play(defaultPos, defaultPos, false);
			}
		}

		if (!p2Done) {
			if (Controllers->getButtonDown(1, MyInputs::Left))
			{
				lastInputTime = TotalGameTime;
				p2Char--;
				//Play menu option move sound
				selectionChannel = menuMove.Play(defaultPos, defaultPos, false);
			}
			else if (Controllers->getButtonDown(1, MyInputs::Right))
			{
				lastInputTime = TotalGameTime;
				p2Char++;
				//Play menu option move sound
				selectionChannel = menuMove.Play(defaultPos, defaultPos, false);
			}
		}
	}

	//correction
	if (p1Char < 1)
		p1Char = 1;
	else if (p1Char > 2)
		p1Char = 2;
	if (p2Char < 1)
		p2Char = 1;
	else if (p2Char > 2)
		p2Char = 2;

	//move curser
	if (p1Char == 1) {
		findObjects(1, "p1Select")->setPosition(
			findObjects(1, "knightIcon")->getPosition());

		findObjects(1, "p1KnightPic")->hide = false;
		findObjects(1, "p1NinjaPic")->hide = true;
	}
	else if (p1Char == 2) {
		findObjects(1, "p1Select")->setPosition(
			findObjects(1, "ninjaIcon")->getPosition());

		findObjects(1, "p1KnightPic")->hide = true;
		findObjects(1, "p1NinjaPic")->hide = false;
	}
	if (p2Char == 1) {
		findObjects(1, "p2Select")->setPosition(
			findObjects(1, "knightIcon")->getPosition());

		findObjects(1, "p2KnightPic")->hide = false;
		findObjects(1, "p2NinjaPic")->hide = true;
	}
	else if (p2Char == 2) {
		findObjects(1, "p2Select")->setPosition(
			findObjects(1, "ninjaIcon")->getPosition());

		findObjects(1, "p2KnightPic")->hide = true;
		findObjects(1, "p2NinjaPic")->hide = false;
	}


	//press
	if (!p1Done) {
		if (Controllers->getButtonDown(0, MyInputs::Basic)) {
			//Play Select Sound
			selectionChannel = select.Play(defaultPos, defaultPos, false);

			if (p1Char == 1) {
				p1Done = true;
				//Announce gaurdian
				selectionChannel = gaurdian.Play(defaultPos, defaultPos, false);
			}
			else if (p1Char == 2) {
				p1Done = true;
				//Announce assassin
				selectionChannel = assassin.Play(defaultPos, defaultPos, false);
			}
		}
	}
	if (!p2Done) {
		if (Controllers->getButtonDown(1, MyInputs::Basic)) {
			//Play Select Sound
			selectionChannel = select.Play(defaultPos, defaultPos, false);

			if (p2Char == 1) {
				p2Done = true;
				//Announce gaurdian
				selectionChannel = gaurdian.Play(defaultPos, defaultPos, false);
			}
			else if (p2Char == 2) {
				p2Done = true;
				//Announce assassin
				selectionChannel = assassin.Play(defaultPos, defaultPos, false);
			}
		}
	}

	//Both Done
	if (p1Done && p2Done) {


		scene = 2;
		lastInputTime = 0.0f;
		TotalGameTime = 0.0f;
		deltaTime = 0;
		updateTimer = new Timer();
	}
}


void Game::updateSSS()
{
	// update our clock so we have the delta time since the last update
	updateTimer->tick();
	float deltaTime = updateTimer->getElapsedTimeSeconds();
	TotalGameTime += deltaTime;

	updateInputs();

	//Go back to main menu
	if (Controllers->getButtonDown(0, MyInputs::Start) || Controllers->getButtonDown(1, MyInputs::Start)) {
		lastInputTime = 0.0f;

		stageDone = false;
		stageVal = 1;
		scene = 0;
		TotalGameTime = 0.0f;
		lastInputTime = 0.0f;
		p1Char = 0;
		p2Char = 0;
		p1Done = false;
		p2Done = false;
		isNinja1 = false;
		isNinja2 = false;
		onePlaying = false;
		thirtyPlaying = false;
	}

	//check if move input
	if (TotalGameTime - lastInputTime > 0.2f && !stageDone) {
		if (Controllers->getButtonDown(0, MyInputs::Left) || Controllers->getButtonDown(1, MyInputs::Left)){
			stageVal++;
			lastInputTime = TotalGameTime;
			//Play menu option move sound
			selectionChannel = menuMove.Play(defaultPos, defaultPos, false);
		}
		if (Controllers->getButtonDown(0, MyInputs::Right) || Controllers->getButtonDown(1, MyInputs::Right)) {
			stageVal--;
			lastInputTime = TotalGameTime;
			//Play menu option move sound
			selectionChannel = menuMove.Play(defaultPos, defaultPos, false);
		}
	}

	//correction
	if (stageVal < 1)
		stageVal = 1;
	else if (stageVal > 3)
		stageVal = 3;

	//move curser
	if (stageVal == 1) {
		findObjects(2, "select")->setPosition(
			findObjects(2, "courtIconDefault")->getPosition());

		findObjects(2, "courtPicDefault")->hide = false;
		findObjects(2, "courtPicKnight")->hide = true;
		findObjects(2, "courtPicNinja")->hide = true;
	}
	else if (stageVal == 2) {
		findObjects(2, "select")->setPosition(
			findObjects(2, "courtIconKnight")->getPosition());

		findObjects(2, "courtPicDefault")->hide = true;
		findObjects(2, "courtPicKnight")->hide = false;
		findObjects(2, "courtPicNinja")->hide = true;
	}
	else if (stageVal == 3) {
		findObjects(2, "select")->setPosition(
			findObjects(2, "courtIconNinja")->getPosition());

		findObjects(2, "courtPicDefault")->hide = true;
		findObjects(2, "courtPicKnight")->hide = true;
		findObjects(2, "courtPicNinja")->hide = false;
	}


	//press
	if ((Controllers->getButtonDown(0, MyInputs::Start) || Controllers->getButtonDown(1, MyInputs::Start)) && !stageDone) {
		//Play Select Sound
		selectionChannel = select.Play(defaultPos, defaultPos, false);
		
		lastInputTime = 0.0f;
		stageDone = true;
	}

	//Both Done
	if (stageDone) {

		//load default court
		if (stageVal == 1) {

			//hide ninja court
			findObjects(3, "stage3_env")->hide = true;
			for (int i = 0; i < (int)stage3_env_objs.size(); i++)
				findObjects(3, stage3_env_objs[i])->hide = true;
			//hide knight court
			findObjects(3, "stage2_env")->hide = true;
			for (int i = 0; i < (int)stage2_env_objs.size(); i++)
				findObjects(3, stage2_env_objs[i])->hide = true;
			//show default court
			findObjects(3, "stage1_env")->hide = false;
			for (int i = 0; i < (int)stage1_env_objs.size(); i++)
				findObjects(3, stage1_env_objs[i])->hide = false;

		}
		//load knight court
		else if (stageVal == 2) {

			//hide ninja court
			findObjects(3, "stage3_env")->hide = true;
			for (int i = 0; i < (int)stage3_env_objs.size(); i++)
				findObjects(3, stage3_env_objs[i])->hide = true;
			//hide default court
			findObjects(3, "stage1_env")->hide = true;
			for (int i = 0; i < (int)stage1_env_objs.size(); i++)
				findObjects(3, stage1_env_objs[i])->hide = true;
			//show knight court
			findObjects(3, "stage2_env")->hide = false;
			for (int i = 0; i < (int)stage2_env_objs.size(); i++)
				findObjects(3, stage2_env_objs[i])->hide = false;

		}
		//load ninja court
		else if (stageVal == 3) {

			//hide default court
			findObjects(3, "stage1_env")->hide = true;
			for (int i = 0; i < (int)stage1_env_objs.size(); i++)
				findObjects(3, stage1_env_objs[i])->hide = true;
			//hide knight court
			findObjects(3, "stage2_env")->hide = true;
			for (int i = 0; i < (int)stage2_env_objs.size(); i++)
				findObjects(3, stage2_env_objs[i])->hide = true;
			//show knight court
			findObjects(3, "stage3_env")->hide = false;
			for (int i = 0; i < (int)stage3_env_objs.size(); i++)
				findObjects(3, stage3_env_objs[i])->hide = false;

		}

		//Player 1 knight
		if (p1Char == 1) {
			players[0] = new charBlueDragon(blueTemp);
			players[0]->bodyTexture.Load("./Assets/Textures/player1.png");

		}
		//Player 1 ninja
		else if (p1Char == 2) {
			//players[0] = new charRedDevil(ninjaTemp);
			players[0]->bodyTexture.Load("./Assets/Textures/player1ninja.png");

			isNinja1 = true;

		}

		//Player 2 knight
		if (p2Char == 1) {
			players[1] = new charBlueDragon(blueTemp);
			players[1]->bodyTexture.Load("./Assets/Textures/player2.png");

		}
		//Player 2 ninja
		else if (p2Char == 2) {
			//players[1] = new charRedDevil(ninjaTemp);
			players[1]->bodyTexture.Load("./Assets/Textures/player2ninja.png");

			isNinja2 = true;

		}

		scene = 3;
		players[0]->respawn();
		players[1]->respawn();
		players[0]->setPosition(glm::vec3(-5, 0, 0));
		players[1]->setPosition(glm::vec3(5, 0, 0));
		players[1]->facingRight = false;
		//updateTimer = new Timer();
		TotalGameTime = 0.0f;
		deltaTime = 0;
		updateTimer = new Timer();
	}
}

void Game::updateScene()
{
	// update our clock so we have the delta time since the last update
	updateTimer->tick();

	float deltaTime = updateTimer->getElapsedTimeSeconds();
	TotalGameTime += deltaTime;

	//character facing
	if (players[0]->getPosition().x > players[1]->getPosition().x) {
		if (players[0]->action == players[0]->ACTION_IDLE || players[0]->action == players[0]->ACTION_FALL)
			players[0]->facingRight = false;
		if (players[1]->action == players[1]->ACTION_IDLE || players[1]->action == players[1]->ACTION_FALL)
			players[1]->facingRight = true;
	}
	else {
		if (players[0]->action == players[0]->ACTION_IDLE || players[0]->action == players[0]->ACTION_FALL)
			players[0]->facingRight = true;
		if (players[1]->action == players[1]->ACTION_IDLE || players[1]->action == players[1]->ACTION_FALL)
			players[1]->facingRight = false;
	}

	//dynamic screen walls
	leftWall = max(GameCamera.getPosition().x - 10, -25.0f);
	rightWall = min(GameCamera.getPosition().x + 10, 25.0f);
	players[0]->leftWall = leftWall;
	players[0]->rightWall = rightWall;
	players[1]->leftWall = leftWall;
	players[1]->rightWall = rightWall;
	players[0]->floor = floor;
	players[1]->floor = floor;

	//character collisions
	///Allow Player to pass through one another, but will disallow them to stand in the same spot, will slowly push eachother awayy
	float diffx = (players[0]->getPosition().x) - (players[1]->getPosition().x);//difference between characters x
	float diffy = (players[0]->getPosition().y) - (players[1]->getPosition().y);//difference between characters y
	if (abs(diffx) < 0.7f && abs(diffx) != 0 && abs(diffy) < 1.7f) {//if they are touching
		//push them off
		players[0]->setPosition(players[0]->getPosition() + glm::vec3(((diffx / abs(diffx))*0.01f), 0, 0));
		players[1]->setPosition(players[1]->getPosition() + glm::vec3(((diffx / abs(diffx))*-0.01f), 0, 0));
	}

	//rumble fr first 5 frames of either player being hit;
	for (int i = 0; i < 2; i++) {
		if (players[i]->isHit() && players[i]->currentFrame < 6) {
			Controllers->SetVibration(0, 10, 10);//controller 0, power 10 on left and right
			Controllers->SetVibration(1, 10, 10);//controller 1, power 10 on left and right
			GameCamera.setRumble(4, 0.04f);
			players[i]->activeTexture = &(players[i]->hurtTexture);
		}
		else if (players[i]->isHit() && players[i]->currentFrame == 7) {
			Controllers->SetVibration(0, 0, 0);//controller 0, power 0 on left and right (off)
			Controllers->SetVibration(1, 0, 0);//controller , power 0 on left and right (off)
			players[i]->activeTexture = &(players[i]->bodyTexture);
		}
	}

	//new hitbox collisions
	for (unsigned int i = 0; i < players[0]->getHitboxes().size(); i++) {
		for (unsigned int j = 0; j < players[1]->getHurtboxes().size(); j++) {

			glm::vec3 diff = players[0]->getHitboxes()[i]->getPosition() - players[1]->getHurtboxes()[j]->getPosition();
			float size = (players[0]->getHitboxes()[i]->getSize() + players[1]->getHurtboxes()[j]->getSize()) *0.5f;
			if (/*diff.Length()*/ glm::length(diff) < size) {

				Controllers->SetVibration(0, 10, 10);
				Controllers->SetVibration(1, 10, 10);
				Sleep(40 + (int)(players[0]->getHitboxes()[i]->getKnockback() * 3));
				Controllers->SetVibration(0, 0, 0);
				Controllers->SetVibration(1, 0, 0);

				if ((((Controllers->getButton(1, MyInputs::Left) && !players[0]->getHitboxes()[i]->facingRight) || (Controllers->getButton(1, MyInputs::Right) && players[0]->getHitboxes()[i]->facingRight)))
					&& (players[1]->action == players[1]->ACTION_FALL || players[1]->action == players[1]->ACTION_JUMP || players[1]->action == players[1]->ACTION_JUMP2 ||
						players[1]->action == players[1]->ACTION_PREJUMP || players[1]->action == players[1]->ACTION_WALK || players[1]->action == players[1]->ACTION_RUN)) {
					//if holding back, and in a action wheret they can block

					//block

				}
				else {//hit

					if (players[1]->action != players[1]->ACTION_HIT) {
						players[0]->comboClear();
						players[0]->resetTimer();
					}
					else {
						players[0]->comboAdd();
					}

					players[0]->setVelocity(glm::vec3(players[0]->getVelocity().x, 0.08f, 0.0f));
					players[1]->setVelocity(glm::vec3(players[1]->getVelocity().x, 0.0f, 0.0f));

					players[1]->hit(players[0]->getHitboxes()[i]);
				}

				players[0]->getHitboxes()[i]->setDone();
				i = 100;
				j = 100;
			}

		}
	}
	for (unsigned int i = 0; i < players[1]->getHitboxes().size(); i++) {
		for (unsigned int j = 0; j < players[0]->getHurtboxes().size(); j++) {

			glm::vec3 diff = players[1]->getHitboxes()[i]->getPosition() - players[0]->getHurtboxes()[j]->getPosition();
			float size = (players[1]->getHitboxes()[i]->getSize() + players[0]->getHurtboxes()[j]->getSize()) *0.5f;
			if (/*diff.Length()*/ glm::length(diff) < size) {

				Controllers->SetVibration(0, 10, 10);
				Controllers->SetVibration(1, 10, 10);
				Sleep(40 + (int)(players[1]->getHitboxes()[i]->getKnockback() * 3));
				Controllers->SetVibration(0, 0, 0);
				Controllers->SetVibration(1, 0, 0);

				if ((((Controllers->getButton(0, MyInputs::Left) && !players[1]->getHitboxes()[i]->facingRight) || (Controllers->getButton(0, MyInputs::Right) && players[1]->getHitboxes()[i]->facingRight)))
					&& (players[0]->action == players[0]->ACTION_FALL || players[0]->action == players[0]->ACTION_JUMP || players[0]->action == players[0]->ACTION_JUMP2 ||
						players[0]->action == players[0]->ACTION_PREJUMP || players[0]->action == players[0]->ACTION_WALK || players[0]->action == players[0]->ACTION_RUN)) {
					//if holding back, and in a action wheret they can block

					//block

				}
				else {//hit

					if (players[0]->action != players[0]->ACTION_HIT) {
						players[1]->comboClear();
						players[1]->resetTimer();
					}
					else {
						players[1]->comboAdd();
					}

					players[0]->setVelocity(glm::vec3(players[0]->getVelocity().x, 0.0f, 0.0f));
					players[1]->setVelocity(glm::vec3(players[1]->getVelocity().x, 0.08f, 0.0f));

					players[0]->hit(players[1]->getHitboxes()[i]);
				}

				players[1]->getHitboxes()[i]->setDone();
				i = 100;
				j = 100;
			}

		}
	}

	updateInputs();

	//Go back to main menu on pressing start button
	if (Controllers->getButtonDown(0, MyInputs::Start) || Controllers->getButtonDown(1, MyInputs::Start)) {
		lastInputTime = 0.0f;

		stageDone = false;
		stageVal = 1;
		scene = 0;
		TotalGameTime = 0.0f;
		lastInputTime = 0.0f;
		p1Char = 0;
		p2Char = 0;
		p1Done = false;
		p2Done = false;
		isNinja1 = false;
		isNinja2 = false;
		onePlaying = false;
		thirtyPlaying = false;
	}


	players[0]->update((int)deltaTime, Controllers, 0);
	players[1]->update((int)deltaTime, Controllers, 1);

	for (int i = 0; i < 2; i++) {
		//check 2 per loop
		///only checking 1 per frame caused issues with spawn timing (if statement)
		///checking everything in queue every frame caused issues with random frame drops (while loop)

		//particle listener
		if (players[0]->partiQueue.size() > 0) {
			//cout << "Spawned";
			switch (players[0]->partiQueue.front())
			{
			case LANDDUST:
				DustLand.RangeX = glm::vec2(players[0]->getPosition().x + 0.5f, players[0]->getPosition().x - 0.5f);
				DustLand.RangeY = glm::vec2(players[0]->getPosition().y - 1.4f, players[0]->getPosition().y - 1.6f);
				DustLand.Spawn(0.05f);
				break;
			case LDASHDUST:
				DustDashL.RangeX = glm::vec2(players[0]->getPosition().x, players[0]->getPosition().x - 1.0f);
				DustDashL.RangeY = glm::vec2(players[0]->getPosition().y - 1.4f, players[0]->getPosition().y - 1.6f);
				DustDashL.Spawn(0.05f);
				break;
			case RDASHDUST:
				DustDashR.RangeX = glm::vec2(players[0]->getPosition().x + 1.0f, players[0]->getPosition().x);
				DustDashR.RangeY = glm::vec2(players[0]->getPosition().y - 1.4f, players[0]->getPosition().y - 1.6f);
				DustDashR.Spawn(0.05f);
				break;
			case LAUNCHDUST:
				DustLaunch.RangeX = glm::vec2(players[0]->getPosition().x + 0.7f, players[0]->getPosition().x - 0.7f);
				DustLaunch.RangeY = glm::vec2(players[0]->getPosition().y + 1.5f, players[0]->getPosition().y + 3.0f);
				DustLaunch.Spawn(0.05f);
				break;
			case HITSPARKL:
				HitSparkL.RangeX = glm::vec2(players[0]->getPosition().x, players[0]->getPosition().x);
				HitSparkL.RangeY = glm::vec2(players[0]->getPosition().y + 1.5f, players[0]->getPosition().y + 1.5f);
				HitSparkL.Spawn(0.05f);
				break;
			case HITSPARKR:
				HitSparkR.RangeX = glm::vec2(players[0]->getPosition().x, players[0]->getPosition().x);
				HitSparkR.RangeY = glm::vec2(players[0]->getPosition().y + 1.5f, players[0]->getPosition().y + 1.5f);
				HitSparkR.Spawn(0.05f);
				break;
			case METERFLAME:
				MeterFlame1.RangeX = glm::vec2(players[0]->getPosition().x + 0.7f, players[0]->getPosition().x - 1.7f);
				MeterFlame1.RangeY = glm::vec2(players[0]->getPosition().y - 0.7f, players[0]->getPosition().y + 4.0f);
				MeterFlame1.Spawn(0.001f);
				break;
			default:
				break;
			}

			players[0]->partiQueue.pop();
		}
		if (players[1]->partiQueue.size() > 0) {
			//cout << "Spawned";
			switch (players[1]->partiQueue.front())
			{
			case LANDDUST:
				DustLand.RangeX = glm::vec2(players[1]->getPosition().x + 0.5f, players[1]->getPosition().x - 0.5f);
				DustLand.RangeY = glm::vec2(players[1]->getPosition().y - 1.4f, players[1]->getPosition().y - 1.6f);
				DustLand.Spawn(0.05f);
				break;
			case LDASHDUST:
				DustDashL.RangeX = glm::vec2(players[1]->getPosition().x + 1.0f, players[1]->getPosition().x - 1.0f);
				DustDashL.RangeY = glm::vec2(players[1]->getPosition().y - 1.4f, players[1]->getPosition().y - 1.6f);
				DustDashL.Spawn(0.05f);
				break;
			case RDASHDUST:
				DustDashR.RangeX = glm::vec2(players[1]->getPosition().x + 1.0f, players[1]->getPosition().x - 1.0f);
				DustDashR.RangeY = glm::vec2(players[1]->getPosition().y - 1.4f, players[1]->getPosition().y - 1.6f);
				DustDashR.Spawn(0.05f);
				break;
			case LAUNCHDUST:
				DustLaunch.RangeX = glm::vec2(players[1]->getPosition().x + 0.7f, players[1]->getPosition().x - 0.7f);
				DustLaunch.RangeY = glm::vec2(players[1]->getPosition().y + 1.5f, players[1]->getPosition().y + 3.0f);
				DustLaunch.Spawn(0.05f);
				break;
			case HITSPARKL:
				HitSparkL.RangeX = glm::vec2(players[1]->getPosition().x, players[1]->getPosition().x);
				HitSparkL.RangeY = glm::vec2(players[1]->getPosition().y + 1.5f, players[1]->getPosition().y + 1.5f);
				HitSparkL.Spawn(0.05f);
				break;
			case HITSPARKR:
				HitSparkR.RangeX = glm::vec2(players[1]->getPosition().x, players[1]->getPosition().x);
				HitSparkR.RangeY = glm::vec2(players[1]->getPosition().y + 1.5f, players[1]->getPosition().y + 1.5f);
				HitSparkR.Spawn(0.05f);
				break;
			case METERFLAME:
				MeterFlame2.RangeX = glm::vec2(players[1]->getPosition().x + 0.7f, players[1]->getPosition().x - 1.7f);
				MeterFlame2.RangeY = glm::vec2(players[1]->getPosition().y - 0.7f, players[1]->getPosition().y + 4.0f);
				MeterFlame2.Spawn(0.001f);
				break;
			default:
				break;
			}

			players[1]->partiQueue.pop();

		}
	}

	//DYNAMIC CAM


	//camera control using seek point and target zoom
	seekPoint.x = (players[1]->getPosition().x + players[0]->getPosition().x) * 0.5f;//seek point is inbetween the 2 players
	seekPoint.y = ((players[1]->getPosition().y + players[0]->getPosition().y) * 0.5f) - 2;//seek point is inbetween the 2 players

	//camera bounds
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
	//GameCamera.CameraTransform = Transform::Identity();
	//GameCamera.CameraTransform.Translate(glm::vec3((players[1]->getPosition().x + players[0]->getPosition().x) / 2.0f, abs(sqrtf(dist*0.01f)*18.5f) + 10.0f + ((players[1]->getPosition().y + players[0]->getPosition().y) / 2.0f), (dist* 0.75f) + 9));

	hudTransform = Transform::Identity();
	hudTransform.RotateY(0.0f);

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
	//ShadowTransform.Translate(vec3(0.0f, 0.0f, 0.0f));


	///PARTICLE EFFECTS
	//Update Patricle Effects
	DustDashL.Update(deltaTime);
	DustDashR.Update(deltaTime);
	DustLand.Update(deltaTime);
	DustLaunch.Update(deltaTime);
	HitSparkL.Update(deltaTime);
	HitSparkR.Update(deltaTime);
	MeterFlame1.Update(deltaTime);
	MeterFlame2.Update(deltaTime);

	//Sound Effects//
	p1Pos = { players[0]->getPosition().x, players[0]->getPosition().y, players[0]->getPosition().z };
	p2Pos = { players[1]->getPosition().x, players[1]->getPosition().y, players[1]->getPosition().z };

	if (players[0]->action == 5)
	{
		//if (knightChannel->isPlaying(p1Jump) == false)
		if (!p1Jump1)
		{
			if (!isNinja1)
				knightChannel = knightJump.Play(defaultPos, defaultPos, false);
			else
				ninjaChannel = ninjaJump.Play(defaultPos, defaultPos, false);
			p1Jump1 = true;
		}
	}
	if (players[0]->action == 6)
	{
		if (!p1Jump2)
			//if (knightChannel->isPlaying(p1Jump) == false)
		{
			if (!isNinja1)
				knightChannel = knightJump.Play(defaultPos, defaultPos, false);
			else
				ninjaChannel = ninjaJump.Play(defaultPos, defaultPos, false);
			p1Jump2 = true;
		}
	}

	if (players[0]->action != 5)
	{
		//std::cout << "knight jump sound";
		//if (knightChannel->isPlaying(p1Jump))
		//if (p1Jump)
		//{
			//knightChannel = knightJump.Play(themePos, { 0.0f, 0.0f, 0.0f }, false);
			//p1Jump = true;
		//}
		p1Jump1 = false;
	}
	if (players[0]->action != 6)
	{
		p1Jump2 = false;
	}

	//Announcer voice effects for times
	if (TotalGameTime >= 240.0f)
	{
		if (!onePlaying)
		{
			otherChannel = oneMin.Play(defaultPos, defaultPos, false);
			onePlaying = true;
		}
	}
	if (TotalGameTime >= 270.0f)
	{
		if (!thirtyPlaying)
		{
			otherChannel = thirtySec.Play(defaultPos, defaultPos, false);
			thirtyPlaying = true;
		}
	}

	//End Game
	if (players[0]->dead == true || players[1]->dead == true || TotalGameTime >= 99)
	{
		//Make game black and white
		grayscale = true;

		//Delay timer before going to results screen
		static float tempTime;
		tempTime += updateTimer->getElapsedTimeSeconds();

		if (!hornPlaying)
		{
			otherChannel->setPriority(0);
			otherChannel = horn.Play(defaultPos, defaultPos, false);
			hornPlaying = true;
		}

		//Bring volume to 0
		decreaseVal -= 0.005f;
		themeChannel->setVolume(decreaseVal);

		//Check if player 1 won
		if (players[1]->dead == true) {
			//Check what character they are
			if (p1Char == 1)
				p1KnightWin = true;
			else if (p1Char == 2)
				p1NinjaWin = true;
		}
		//Check if player 2 won
		else if (players[0]->dead == true) {
			//Check what character they are
			if (p2Char == 1)
				p2KnightWin = true;
			else if (p2Char == 2)
				p2NinjaWin = true;
		}
		else {
			tieGame = true;
		}

		if (tempTime >= 1.5f)
		{
			stageDone = false;
			stageVal = 1;
			scene = 4;
			TotalGameTime = 0.0f;
			lastInputTime = 0.0f;
			p1Char = 0;
			p2Char = 0;
			p1Done = false;
			p2Done = false;
			tempTime = 0.0f;
			decreaseVal = 1.0f;
			grayscale = false;
			isNinja1 = false;
			isNinja2 = false;
			hornPlaying = false;
			onePlaying = false;
			thirtyPlaying = false;

			//to be removed later
			p1Char = 1;
			p2Char = 2;
			players[0] = new charBlueDragon(blueTemp);
			players[1] = new charBlueDragon(blueTemp);
			players[1]->bodyTexture.Load("./Assets/Textures/blueDtextureBlue.png");

			stageVal = 1;
			//hide ninja court
			findObjects(3, "stage3_env")->hide = true;
			for (int i = 0; i < (int)stage3_env_objs.size(); i++)
				findObjects(3, stage3_env_objs[i])->hide = true;
			//hide knight court
			findObjects(3, "stage2_env")->hide = true;
			for (int i = 0; i < (int)stage2_env_objs.size(); i++)
				findObjects(3, stage2_env_objs[i])->hide = true;
			//show default court
			findObjects(3, "stage1_env")->hide = false;
			for (int i = 0; i < (int)stage1_env_objs.size(); i++)
				findObjects(3, stage1_env_objs[i])->hide = false;
			scene = 3;
			players[0]->respawn();
			players[0]->facingRight = true;
			players[1]->respawn();
			players[1]->facingRight = false;
			TotalGameTime = 0.0f;
			lastInputTime = 0.0f;
			players[0]->setPosition(glm::vec3(-5, 0, 0));
			players[1]->setPosition(glm::vec3(5, 0, 0));
		}
	}
}

void Game::draw()
{
	if (scene == 5)
	{
		drawTutScreen();
	}
	else if (scene == 4) {
		drawEndScreen();
	}
	else if (scene == 3) {
		drawScene();
	}
	else if (scene == 2) {
		drawSSS();
	}
	else if(scene == 1) {
		drawCSS();
	}
	else {
		drawMenu();
	}
}

void Game::drawTutScreen()
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

	/// Create Scene From GBuffer ///
	glViewport(0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);

	DeferredLighting->Bind();
	DeferredLighting->SendUniformMat4("ViewToShadowMap", ViewToShadowMap.data, true);
	DeferredLighting->SendUniform("uScene", 0);
	DeferredLighting->SendUniform("uShadowMap", 1);
	DeferredLighting->SendUniform("uNormalMap", 2);
	DeferredLighting->SendUniform("uPositionMap", 3);

	DeferredLighting->SendUniform("LightDirection", glm::vec3(GameCamera.CameraTransform.GetInverse().getRotationMat() * glm::normalize(ShadowTransform.GetForward())));
	DeferredLighting->SendUniform("LightAmbient", glm::vec3(0.6f, 0.6f, 0.6f)); //You can LERP through colours to make night to day cycles
	DeferredLighting->SendUniform("LightDiffuse", glm::vec3(0.6f, 0.6f, 0.6f));
	DeferredLighting->SendUniform("LightSpecular", glm::vec3(0.6f, 0.6f, 0.6f));
	DeferredLighting->SendUniform("LightSpecularExponent", 500.0f);

	DeferredComposite.Bind();

	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(0));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ShadowMap.GetDepthHandle());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(1));
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(2));

	DrawFullScreenQuad();

	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE); //Why was this not here in week 10 vid?

	DeferredComposite.UnBind();
	DeferredLighting->UnBind();

	//===============================================================
	DeferredComposite.Bind();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);  // disable writes to Z-Buffer
	glDisable(GL_DEPTH_TEST);  // disable depth-testing
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);

	//new projection
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();//save old state
	glLoadIdentity();//reset
	gluOrtho2D((float)FULLSCREEN_WIDTH * -0.5f, (float)FULLSCREEN_WIDTH * 0.5f, (float)FULLSCREEN_HEIGHT * -0.5f, (float)FULLSCREEN_HEIGHT * 0.5f);//create ortho
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();//save old state
	glLoadIdentity();//reset

//////////////////////////
	//now ready to draw 2d
//////////////////////////
	GBufferPass->Bind();
	hudTransform = Transform::Identity();
	GBufferPass->SendUniformMat4("uView", hudTransform.GetInverse().data, true);
	GBufferPass->SendUniformMat4("uProj", hudProjection.data, true);

	//draws everything in menu
	sortObjects(5);
	for (int i = 0; i < (int)tutObjects.size(); i++) {
		tutObjects[i]->draw(*GBufferPass, 1);
	}

	GBufferPass->UnBind();

	//restore projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();//restore state
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();//restore state

	DeferredComposite.UnBind();
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
	//===============================================================

		/// Compute High Pass ///
	glViewport(0, 0, (GLsizei)(FULLSCREEN_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(FULLSCREEN_HEIGHT / BLOOM_DOWNSCALE));
	//Moving data to the back buffer, at the same time as our last post process
	BloomHighPass->Bind();
	BloomHighPass->SendUniform("uTex", 0);
	BloomHighPass->SendUniform("uThreshold", 1.0f);
	WorkBuffer1.Bind();
	glBindTexture(GL_TEXTURE_2D, DeferredComposite.GetColorHandle(0));
	DrawFullScreenQuad();
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	WorkBuffer1.UnBind();
	BloomHighPass->UnBind();

	/// Compute Blur ///
	glViewport(0, 0, (GLsizei)(FULLSCREEN_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(FULLSCREEN_HEIGHT / BLOOM_DOWNSCALE));
	for (int i = 0; i < BLOOM_BLUR_PASSES; i++)
	{
		//Horizontal Blur
		BlurHorizontal->Bind();
		BlurHorizontal->SendUniform("uTex", 0);
		BlurHorizontal->SendUniform("uPixelSize", 1.0f / FULLSCREEN_WIDTH);
		WorkBuffer2.Bind();
		glBindTexture(GL_TEXTURE_2D, WorkBuffer1.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		WorkBuffer2.UnBind();
		BlurHorizontal->UnBind();

		//Vertical Blur
		BlurVertical->Bind();
		BlurVertical->SendUniform("uTex", 0);
		BlurVertical->SendUniform("uPixelSize", 1.0f / FULLSCREEN_HEIGHT);
		WorkBuffer1.Bind();
		glBindTexture(GL_TEXTURE_2D, WorkBuffer2.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		WorkBuffer1.UnBind();
		BlurVertical->UnBind();
	}

	/// Composite To Back Buffer ///
	glViewport(0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
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

void Game::drawEndScreen()
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

	/// Create Scene From GBuffer ///
	glViewport(0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
	DeferredLighting->Bind();
	DeferredLighting->SendUniformMat4("ViewToShadowMap", ViewToShadowMap.data, true);
	DeferredLighting->SendUniform("uScene", 0);
	DeferredLighting->SendUniform("uShadowMap", 1);
	DeferredLighting->SendUniform("uNormalMap", 2);
	DeferredLighting->SendUniform("uPositionMap", 3);

	DeferredLighting->SendUniform("LightDirection", glm::vec3(GameCamera.CameraTransform.GetInverse().getRotationMat() * glm::normalize(ShadowTransform.GetForward())));
	DeferredLighting->SendUniform("LightAmbient", glm::vec3(0.6f, 0.6f, 0.6f)); //You can LERP through colours to make night to day cycles
	DeferredLighting->SendUniform("LightDiffuse", glm::vec3(0.6f, 0.6f, 0.6f));
	DeferredLighting->SendUniform("LightSpecular", glm::vec3(0.6f, 0.6f, 0.6f));
	DeferredLighting->SendUniform("LightSpecularExponent", 500.0f);

	DeferredComposite.Bind();

	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(0));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ShadowMap.GetDepthHandle());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(1));
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(2));



	DrawFullScreenQuad();

	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE); //Why was this not here in week 10 vid?

	DeferredComposite.UnBind();
	DeferredLighting->UnBind();

	//===============================================================
		//DeferredComposite.Bind();
	DeferredComposite.Bind();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);  // disable writes to Z-Buffer
	glDisable(GL_DEPTH_TEST);  // disable depth-testing
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);

	//new projection
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();//save old state
	glLoadIdentity();//reset
	gluOrtho2D((float)FULLSCREEN_WIDTH * -0.5f, (float)FULLSCREEN_WIDTH * 0.5f, (float)FULLSCREEN_HEIGHT * -0.5f, (float)FULLSCREEN_HEIGHT * 0.5f);//create ortho
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();//save old state
	glLoadIdentity();//reset

//////////////////////////
	//now ready to draw 2d
//////////////////////////
	GBufferPass->Bind();
	hudTransform = Transform::Identity();
	GBufferPass->SendUniformMat4("uView", hudTransform.GetInverse().data, true);
	GBufferPass->SendUniformMat4("uProj", hudProjection.data, true);

	//draws everything in menu
	sortObjects(4);
	for (int i = 0; i < (int)endObjects.size(); i++) {
		if (p1KnightWin)
			endObjects[0]->draw(*GBufferPass, 1);
		else if (p1NinjaWin)
			endObjects[2]->draw(*GBufferPass, 1);
		else if (p2KnightWin)
			endObjects[1]->draw(*GBufferPass, 1);
		else if (p2NinjaWin)
			endObjects[3]->draw(*GBufferPass, 1);
		else if (tieGame)
			endObjects[4]->draw(*GBufferPass, 1);
	}

	GBufferPass->UnBind();

	//restore projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();//restore state
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();//restore state

	DeferredComposite.UnBind();
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
	//===============================================================

		/// Compute High Pass ///
	glViewport(0, 0, (GLsizei)(FULLSCREEN_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(FULLSCREEN_HEIGHT / BLOOM_DOWNSCALE));
	//Moving data to the back buffer, at the same time as our last post process
	BloomHighPass->Bind();
	BloomHighPass->SendUniform("uTex", 0);
	BloomHighPass->SendUniform("uThreshold", 1.0f);
	WorkBuffer1.Bind();
	glBindTexture(GL_TEXTURE_2D, DeferredComposite.GetColorHandle(0));
	DrawFullScreenQuad();
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	WorkBuffer1.UnBind();
	BloomHighPass->UnBind();

	/// Compute Blur ///
	glViewport(0, 0, (GLsizei)(FULLSCREEN_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(FULLSCREEN_HEIGHT / BLOOM_DOWNSCALE));
	for (int i = 0; i < BLOOM_BLUR_PASSES; i++)
	{
		//Horizontal Blur
		BlurHorizontal->Bind();
		BlurHorizontal->SendUniform("uTex", 0);
		BlurHorizontal->SendUniform("uPixelSize", 1.0f / FULLSCREEN_WIDTH);
		WorkBuffer2.Bind();
		glBindTexture(GL_TEXTURE_2D, WorkBuffer1.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		WorkBuffer2.UnBind();
		BlurHorizontal->UnBind();

		//Vertical Blur
		BlurVertical->Bind();
		BlurVertical->SendUniform("uTex", 0);
		BlurVertical->SendUniform("uPixelSize", 1.0f / FULLSCREEN_HEIGHT);
		WorkBuffer1.Bind();
		glBindTexture(GL_TEXTURE_2D, WorkBuffer2.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		WorkBuffer1.UnBind();
		BlurVertical->UnBind();
	}

	/// Composite To Back Buffer ///
	glViewport(0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
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

void Game::drawScene()
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
	//godRaysBuffer1.Clear();
	//godRaysBuffer2.Clear();

	/// Generate The Shadow Map ///
	glViewport(0, 0, SHADOW_RESOLUTION, SHADOW_RESOLUTION);

	GBufferPass->Bind();
	GBufferPass->SendUniformMat4("uModel", Transform().data, true);
	//The reason of the inverse is because it is easier to do transformations
	GBufferPass->SendUniformMat4("uView", ShadowTransform.GetInverse().data, true);
	GBufferPass->SendUniformMat4("uProj", ShadowProjection.data, true);

	ShadowMap.Bind();

	Object* temp;
	if (stageVal == 1)
		temp = findObjects(3, "stage1_env");
	else if (stageVal == 2)
		temp = findObjects(3, "stage2_env");
	else
		temp = findObjects(3, "stage3_env");

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
	AniShader->SendUniformMat4("uModel", players[0]->transform.data, true);
	players[0]->draw(*AniShader, 0);
	AniShader->SendUniformMat4("uModel", Transform().data, true);

	//draw p2 shadow
	AniShader->Bind();
	AniShader->SendUniformMat4("uModel", Transform().data, true);
	AniShader->SendUniformMat4("uView", ShadowTransform.GetInverse().data, true);
	AniShader->SendUniformMat4("uProj", ShadowProjection.data, true);
	AniShader->SendUniformMat4("uModel", players[1]->transform.data, true);
	players[1]->draw(*AniShader, 0);
	AniShader->SendUniformMat4("uModel", Transform().data, true);

	glEnable(GL_CULL_FACE);//turn it back on after for preformance

	glBindVertexArray(0);

	ShadowMap.UnBind();
	AniShader->UnBind();

	/// Generate The Scene ///
	glViewport(0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);

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
	sortObjects(3);
	for (int i = 0; i < (int)gameObjects.size(); i++) {

		GBufferPass->Bind();
		GBufferPass->SendUniformMat4("uModel", Transform().data, true);
		//The reason of the inverse is because it is easier to do transformations
		GBufferPass->SendUniformMat4("uView", GameCamera.CameraTransform.GetInverse().data, true);
		GBufferPass->SendUniformMat4("uProj", GameCamera.CameraProjection.data, true);
		//GBufferPass->SendUniformMat4("ViewToShadowMap", ViewToShadowMap.data, true);

		//MAKE SURE TO KNOW WHAT VIEWSPACE YOU ARE WORKING IN
		GBufferPass->SendUniform("uTex", 0);
		gameObjects[i]->draw(*GBufferPass, 1);
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

	players[0]->drawBoxes(*GBufferPass);
	players[1]->drawBoxes(*GBufferPass);


	glDisable(GL_CULL_FACE);//should fix random holes in knight
	//glDisable(GL_BLEND);//should fix random holes in knight

	AniShader->Bind();
	AniShader->SendUniformMat4("uView", GameCamera.CameraTransform.GetInverse().data, true);
	AniShader->SendUniformMat4("uProj", GameCamera.CameraProjection.data, true);
	players[0]->draw(*AniShader, 1);

	AniShader->Bind();
	AniShader->SendUniformMat4("uView", GameCamera.CameraTransform.GetInverse().data, true);
	AniShader->SendUniformMat4("uProj", GameCamera.CameraProjection.data, true);
	players[1]->draw(*AniShader, 1);

	glEnable(GL_CULL_FACE);//turn it back on after for preformance

	//Black and white
	if (grayscale == true)
	{
		GrayScale->Bind();
		GrayScale->SendUniform("uTex", 0);

		glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);

		GrayScale->UnBind();
	}
	AniShader->UnBind();
	GBuffer.UnBind();
	GBufferPass->UnBind();



	/// Detect Edges ///
	glViewport(0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);

	//Rednering it with the shader
	SobelPass->Bind();
	SobelPass->SendUniform("uNormalMap", 0);
	SobelPass->SendUniform("uDepthMap", 1);
	float stroke = 1.1f;
	SobelPass->SendUniform("uPixelSize", glm::vec2(stroke / FULLSCREEN_WIDTH, stroke / FULLSCREEN_HEIGHT));

	//Where we are rendering
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
	glViewport(0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);

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
	DeferredLighting->SendUniform("uToonActive", toonActive);

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
	PointLight->SendUniform("uToonActive", toonActive);

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

	drawHUD();

	/// Compute High Pass ///

	BloomHighPass->Bind();
	BloomHighPass->SendUniform("bloomOn", true);
	BloomHighPass->UnBind();

	glViewport(0, 0, (GLsizei)(FULLSCREEN_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(FULLSCREEN_HEIGHT / BLOOM_DOWNSCALE));

	//Moving data to the back buffer, at the same time as our last post process
	BloomHighPass->Bind();
	BloomHighPass->SendUniform("uTex", 0);
	if (stageVal == 1)
	{
		BloomHighPass->SendUniform("uThreshold", BLOOM_THRESHOLD1);
		BloomHighPass->SendUniform("uThreshold2", BLOOM_THRESHOLDBRIGHT);
	}
	if (stageVal == 2)
	{
		BloomHighPass->SendUniform("uThreshold", BLOOM_THRESHOLD2);
		BloomHighPass->SendUniform("uThreshold2", BLOOM_THRESHOLDBRIGHT);
	}
	if (stageVal == 3)
	{
		BloomHighPass->SendUniform("uThreshold", BLOOM_THRESHOLD3);
		BloomHighPass->SendUniform("uThreshold2", BLOOM_THRESHOLDBRIGHT);
	}

	WorkBuffer1.Bind();

	glBindTexture(GL_TEXTURE_2D, DeferredComposite.GetColorHandle(0));
	DrawFullScreenQuad();
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	WorkBuffer1.UnBind();

	BloomHighPass->UnBind();

	/// Compute Blur ///
	glViewport(0, 0, (GLsizei)(FULLSCREEN_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(FULLSCREEN_HEIGHT / BLOOM_DOWNSCALE));
	for (int i = 0; i < BLOOM_BLUR_PASSES; i++)
	{
		//Horizontal Blur
		BlurHorizontal->Bind();
		BlurHorizontal->SendUniform("uTex", 0);
		BlurHorizontal->SendUniform("uPixelSize", 1.0f / FULLSCREEN_WIDTH);

		WorkBuffer2.Bind();

		glBindTexture(GL_TEXTURE_2D, WorkBuffer1.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);

		WorkBuffer2.UnBind();

		BlurHorizontal->UnBind();

		//Vertical Blur
		BlurVertical->Bind();
		BlurVertical->SendUniform("uTex", 0);
		BlurVertical->SendUniform("uPixelSize", 1.0f / FULLSCREEN_HEIGHT);

		WorkBuffer1.Bind();

		glBindTexture(GL_TEXTURE_2D, WorkBuffer2.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);

		WorkBuffer1.UnBind();

		BlurVertical->UnBind();
	}

	/// Composite To Back Buffer ///
	glViewport(0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);

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

void Game::drawCSS()
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

	/// Create Scene From GBuffer ///
		glViewport(0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
	
	DeferredLighting->Bind();
	DeferredLighting->SendUniformMat4("ViewToShadowMap", ViewToShadowMap.data, true);
	DeferredLighting->SendUniform("uScene", 0);
	DeferredLighting->SendUniform("uShadowMap", 1);
	DeferredLighting->SendUniform("uNormalMap", 2);
	DeferredLighting->SendUniform("uPositionMap", 3);
	//DeferredLighting->SendUniform("uEdgeMap", 4);
	//DeferredLighting->SendUniform("uStepTexture", 4);

	DeferredLighting->SendUniform("LightDirection", glm::vec3(GameCamera.CameraTransform.GetInverse().getRotationMat() * glm::normalize(ShadowTransform.GetForward())));
	DeferredLighting->SendUniform("LightAmbient", glm::vec3(0.6f, 0.6f, 0.6f)); //You can LERP through colours to make night to day cycles
	DeferredLighting->SendUniform("LightDiffuse", glm::vec3(0.6f, 0.6f, 0.6f));
	DeferredLighting->SendUniform("LightSpecular", glm::vec3(0.6f, 0.6f, 0.6f));
	DeferredLighting->SendUniform("LightSpecularExponent", 500.0f);

	DeferredComposite.Bind();

	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(0));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ShadowMap.GetDepthHandle());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(1));
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(2));



	DrawFullScreenQuad();

	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE); //Why was this not here in week 10 vid?

	DeferredComposite.UnBind();
	DeferredLighting->UnBind();

	//===============================================================
		//DeferredComposite.Bind();
	DeferredComposite.Bind();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);  // disable writes to Z-Buffer
	glDisable(GL_DEPTH_TEST);  // disable depth-testing
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);

	//new projection
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();//save old state
	glLoadIdentity();//reset
		gluOrtho2D((float)FULLSCREEN_WIDTH * -0.5f, (float)FULLSCREEN_WIDTH * 0.5f, (float)FULLSCREEN_HEIGHT * -0.5f, (float)FULLSCREEN_HEIGHT * 0.5f);//create ortho
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();//save old state
	glLoadIdentity();//reset

//////////////////////////
	//now ready to draw 2d
//////////////////////////
	GBufferPass->Bind();
	hudTransform = Transform::Identity();
	GBufferPass->SendUniformMat4("uView", hudTransform.GetInverse().data, true);
	GBufferPass->SendUniformMat4("uProj", hudProjection.data, true);

//draws everything in menu
	sortObjects(1);
	for (int i = 0; i < (int)cssObjects.size(); i++) {
		cssObjects[i]->draw(*GBufferPass, 1);
	}

	GBufferPass->UnBind();

	//restore projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();//restore state
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();//restore state

	DeferredComposite.UnBind();
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
	//===============================================================

	BloomHighPass->Bind();
	BloomHighPass->SendUniform("bloomOn", false);
	BloomHighPass->UnBind();

		/// Compute High Pass ///
		glViewport(0, 0, (GLsizei)(FULLSCREEN_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(FULLSCREEN_HEIGHT / BLOOM_DOWNSCALE));
	//Moving data to the back buffer, at the same time as our last post process
	BloomHighPass->Bind();
	BloomHighPass->SendUniform("uTex", 0);
	BloomHighPass->SendUniform("uThreshold", 1.0f);
	WorkBuffer1.Bind();
	glBindTexture(GL_TEXTURE_2D, DeferredComposite.GetColorHandle(0));
	DrawFullScreenQuad();
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	WorkBuffer1.UnBind();
	BloomHighPass->UnBind();

	/// Compute Blur ///
		glViewport(0, 0, (GLsizei)(FULLSCREEN_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(FULLSCREEN_HEIGHT / BLOOM_DOWNSCALE));
	for (int i = 0; i < BLOOM_BLUR_PASSES; i++)
	{
		//Horizontal Blur
		BlurHorizontal->Bind();
		BlurHorizontal->SendUniform("uTex", 0);
			BlurHorizontal->SendUniform("uPixelSize", 1.0f / FULLSCREEN_WIDTH);
		WorkBuffer2.Bind();
		glBindTexture(GL_TEXTURE_2D, WorkBuffer1.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		WorkBuffer2.UnBind();
		BlurHorizontal->UnBind();

		//Vertical Blur
		BlurVertical->Bind();
		BlurVertical->SendUniform("uTex", 0);
			BlurVertical->SendUniform("uPixelSize", 1.0f / FULLSCREEN_HEIGHT);
		WorkBuffer1.Bind();
		glBindTexture(GL_TEXTURE_2D, WorkBuffer2.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		WorkBuffer1.UnBind();
		BlurVertical->UnBind();
	}

	/// Composite To Back Buffer ///
		glViewport(0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
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


void Game::drawSSS()
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

	/// Create Scene From GBuffer ///
		glViewport(0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
	
	DeferredLighting->Bind();
	DeferredLighting->SendUniformMat4("ViewToShadowMap", ViewToShadowMap.data, true);
	DeferredLighting->SendUniform("uScene", 0);
	DeferredLighting->SendUniform("uShadowMap", 1);
	DeferredLighting->SendUniform("uNormalMap", 2);
	DeferredLighting->SendUniform("uPositionMap", 3);
	//DeferredLighting->SendUniform("uEdgeMap", 4);
	//DeferredLighting->SendUniform("uStepTexture", 4);

	DeferredLighting->SendUniform("LightDirection", glm::vec3(GameCamera.CameraTransform.GetInverse().getRotationMat() * glm::normalize(ShadowTransform.GetForward())));
	DeferredLighting->SendUniform("LightAmbient", glm::vec3(0.6f, 0.6f, 0.6f)); //You can LERP through colours to make night to day cycles
	DeferredLighting->SendUniform("LightDiffuse", glm::vec3(0.6f, 0.6f, 0.6f));
	DeferredLighting->SendUniform("LightSpecular", glm::vec3(0.6f, 0.6f, 0.6f));
	DeferredLighting->SendUniform("LightSpecularExponent", 500.0f);

	DeferredComposite.Bind();

	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(0));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ShadowMap.GetDepthHandle());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(1));
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(2));



	DrawFullScreenQuad();

	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE); //Why was this not here in week 10 vid?

	DeferredComposite.UnBind();
	DeferredLighting->UnBind();

	//===============================================================
		//DeferredComposite.Bind();
	DeferredComposite.Bind();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);  // disable writes to Z-Buffer
	glDisable(GL_DEPTH_TEST);  // disable depth-testing
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);

	//new projection
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();//save old state
	glLoadIdentity();//reset
		gluOrtho2D((float)FULLSCREEN_WIDTH * -0.5f, (float)FULLSCREEN_WIDTH * 0.5f, (float)FULLSCREEN_HEIGHT * -0.5f, (float)FULLSCREEN_HEIGHT * 0.5f);//create ortho
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();//save old state
	glLoadIdentity();//reset

//////////////////////////
	//now ready to draw 2d
//////////////////////////
	GBufferPass->Bind();
	hudTransform = Transform::Identity();
	GBufferPass->SendUniformMat4("uView", hudTransform.GetInverse().data, true);
	GBufferPass->SendUniformMat4("uProj", hudProjection.data, true);

	//draws everything in menu
	sortObjects(2);
	for (int i = 0; i < (int)sssObjects.size(); i++) {
		sssObjects[i]->draw(*GBufferPass, 1);
	}

	GBufferPass->UnBind();

	//restore projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();//restore state
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();//restore state

	DeferredComposite.UnBind();
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
	//===============================================================

	BloomHighPass->Bind();
	BloomHighPass->SendUniform("bloomOn", false);
	BloomHighPass->UnBind();

		/// Compute High Pass ///
		glViewport(0, 0, (GLsizei)(FULLSCREEN_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(FULLSCREEN_HEIGHT / BLOOM_DOWNSCALE));
	//Moving data to the back buffer, at the same time as our last post process
	BloomHighPass->Bind();
	BloomHighPass->SendUniform("uTex", 0);
	BloomHighPass->SendUniform("uThreshold", 1.0f);
	WorkBuffer1.Bind();
	glBindTexture(GL_TEXTURE_2D, DeferredComposite.GetColorHandle(0));
	DrawFullScreenQuad();
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	WorkBuffer1.UnBind();
	BloomHighPass->UnBind();

	/// Compute Blur ///
		glViewport(0, 0, (GLsizei)(FULLSCREEN_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(FULLSCREEN_HEIGHT / BLOOM_DOWNSCALE));
	for (int i = 0; i < BLOOM_BLUR_PASSES; i++){
		//Horizontal Blur
		BlurHorizontal->Bind();
		BlurHorizontal->SendUniform("uTex", 0);
			BlurHorizontal->SendUniform("uPixelSize", 1.0f / FULLSCREEN_WIDTH);
		WorkBuffer2.Bind();
		glBindTexture(GL_TEXTURE_2D, WorkBuffer1.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		WorkBuffer2.UnBind();
		BlurHorizontal->UnBind();

		//Vertical Blur
		BlurVertical->Bind();
		BlurVertical->SendUniform("uTex", 0);
			BlurVertical->SendUniform("uPixelSize", 1.0f / FULLSCREEN_HEIGHT);
		WorkBuffer1.Bind();
		glBindTexture(GL_TEXTURE_2D, WorkBuffer2.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		WorkBuffer1.UnBind();
		BlurVertical->UnBind();
	}

	/// Composite To Back Buffer ///
		glViewport(0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
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

void Game::drawHUD()
{
	//DeferredComposite.Bind();
	DeferredComposite.Bind();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);  // disable writes to Z-Buffer
	glDisable(GL_DEPTH_TEST);  // disable depth-testing
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);

	//new projection
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();//save old state
	glLoadIdentity();//reset
	//gluOrtho2D(0.0, 1.0, 0.0, 1.0);//create ortho
	gluOrtho2D((float)FULLSCREEN_WIDTH * -0.5f, (float)FULLSCREEN_WIDTH * 0.5f, (float)FULLSCREEN_HEIGHT * -0.5f, (float)FULLSCREEN_HEIGHT * 0.5f);//create ortho
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();//save old state
	glLoadIdentity();//reset

	//////////////////////////
	//now ready to draw 2d
	//////////////////////////
	GBufferPass->Bind();
	hudTransform = Transform::Identity();
	//hudTransform.Translate(vec3(WINDOW_WIDTH * -0.5f, WINDOW_HEIGHT* -0.5f, 0));
	GBufferPass->SendUniformMat4("uView", hudTransform.GetInverse().data, true);
	GBufferPass->SendUniformMat4("uProj", hudProjection.data, true);

	Transform hudLoc = Transform::Identity();
	//Draw Player 1 HUD

	//draw quad for p1 bar back
	hudLoc = Transform::Identity();
	hudLoc.Scale(280.0f);
	hudLoc.Translate(glm::vec3(-535, 370, 0));
	hudLoc.RotateY(90.0f);
	GBufferPass->SendUniformMat4("uModel", hudLoc.data, true);
	HudBack0.Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());
	HudBack0.UnBind();

	//draw quad for bar
	hudLoc = Transform::Identity();
	hudLoc.Scale(280.0f);
	hudLoc.Translate(glm::vec3(-535, 370, 0));
	hudLoc.RotateY(90.0f);

	HPShader->Bind();
	hudTransform = Transform::Identity();
	HPShader->SendUniformMat4("uView", hudTransform.GetInverse().data, true);
	HPShader->SendUniformMat4("uProj", hudProjection.data, true);

	HPShader->SendUniformMat4("uModel", hudLoc.data, true);
	HPShader->SendUniform("uMax", players[0]->maxHealth);
	HPShader->SendUniform("uPlayer", 0);//0 right side, 1,left
	///draw grey
	HPShader->SendUniform("uHP", players[0]->greyHealth);
	HPShader->SendUniform("uGrey", true);
	Bar0.Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());
	///draw hp
	HPShader->SendUniform("uHP", players[0]->currentHealth);
	HPShader->SendUniform("uGrey", false);
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());
	///unbind
	Bar0.UnBind();

	GBufferPass->Bind();
	hudTransform = Transform::Identity();
	GBufferPass->SendUniformMat4("uView", hudTransform.GetInverse().data, true);
	GBufferPass->SendUniformMat4("uProj", hudProjection.data, true);

	//draw quad for overlay
	hudLoc = Transform::Identity();
	hudLoc.Scale(280.0f);
	hudLoc.Translate(glm::vec3(-535, 370, 0));
	hudLoc.RotateY(90.0f);
	GBufferPass->SendUniformMat4("uModel", hudLoc.data, true);
	Overlay0.Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());
	Overlay0.UnBind();

	//Draw Player 2 HUD

	//draw quad for p2 bar back
	hudLoc = Transform::Identity();
	hudLoc.Scale(280.0f);
	hudLoc.Translate(glm::vec3(535, 370, 0));
	hudLoc.RotateY(90.0f);
	GBufferPass->SendUniformMat4("uModel", hudLoc.data, true);
	HudBack1.Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());
	HudBack1.UnBind();

	//draw quad for bar
	hudLoc = Transform::Identity();
	hudLoc.Scale(280.0f);
	hudLoc.Translate(glm::vec3(535, 370, 0));
	hudLoc.RotateY(90.0f);

	HPShader->Bind();
	hudTransform = Transform::Identity();
	HPShader->SendUniformMat4("uView", hudTransform.GetInverse().data, true);
	HPShader->SendUniformMat4("uProj", hudProjection.data, true);

	HPShader->SendUniformMat4("uModel", hudLoc.data, true);
	HPShader->SendUniform("uMax", players[1]->maxHealth);
	HPShader->SendUniform("uPlayer", 1);//0 right side, 1,left
	///draw grey
	HPShader->SendUniform("uHP", players[1]->greyHealth);
	HPShader->SendUniform("uGrey", true);
	Bar1.Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());
	///draw hp
	HPShader->SendUniform("uHP", players[1]->currentHealth);
	HPShader->SendUniform("uGrey", false);
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());
	///unbind
	Bar1.UnBind();

	GBufferPass->Bind();
	hudTransform = Transform::Identity();
	//hudTransform.Translate(vec3(WINDOW_WIDTH * -0.5f, WINDOW_HEIGHT* -0.5f, 0));
	GBufferPass->SendUniformMat4("uView", hudTransform.GetInverse().data, true);
	GBufferPass->SendUniformMat4("uProj", hudProjection.data, true);

	//draw quad for overlay
	hudLoc = Transform::Identity();
	hudLoc.Scale(280.0f);
	hudLoc.Translate(glm::vec3(535, 370, 0));
	hudLoc.RotateY(90.0f);
	GBufferPass->SendUniformMat4("uModel", hudLoc.data, true);
	Overlay1.Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());
	Overlay1.UnBind();


	int timer = 100 - (int)TotalGameTime;
	if (timer < 0) timer = 0;
	int secT = (int)(timer / 10);
	int secO = (int)timer % 10;
	if (secT > 9) secT = 9;
	if (secO > 9) secO = 9;

	//Draw Time
	GBufferPass->Bind();
	GBufferPass->SendUniformMat4("uView", hudTransform.GetInverse().data, true);
	GBufferPass->SendUniformMat4("uProj", hudProjection.data, true);
	///sec tens
	hudLoc = Transform();
	hudLoc.Scale(80.0f);
	hudLoc.RotateY(90.0f);
	hudLoc.RotateX(-90.0f);
	hudLoc.Translate(glm::vec3(20, 450, 0));
	GBufferPass->SendUniformMat4("uModel", hudLoc.data, true);
	time[secT]->Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());

	///sec ones
	hudLoc = Transform();
	hudLoc.Scale(80.0f);
	hudLoc.RotateY(90.0f);
	hudLoc.RotateX(-90.0f);
	hudLoc.Translate(glm::vec3(135, 450, 0.1f));
	GBufferPass->SendUniformMat4("uModel", hudLoc.data, true);
	time[secO]->Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());

	time[secO]->UnBind();

	GBufferPass->UnBind();

	//restore projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();//restore state
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();//restore state

	//DeferredComposite.UnBind();
	DeferredComposite.UnBind();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
}

void Game::drawMenu()
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

	/// Create Scene From GBuffer ///
	glViewport(0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);

	DeferredLighting->Bind();
	DeferredLighting->SendUniformMat4("ViewToShadowMap", ViewToShadowMap.data, true);
	DeferredLighting->SendUniform("uScene", 0);
	DeferredLighting->SendUniform("uShadowMap", 1);
	DeferredLighting->SendUniform("uNormalMap", 2);
	DeferredLighting->SendUniform("uPositionMap", 3);
	//DeferredLighting->SendUniform("uEdgeMap", 4);
	//DeferredLighting->SendUniform("uStepTexture", 4);

	DeferredLighting->SendUniform("LightDirection", glm::vec3(GameCamera.CameraTransform.GetInverse().getRotationMat() * glm::normalize(ShadowTransform.GetForward())));
	DeferredLighting->SendUniform("LightAmbient", glm::vec3(0.6f, 0.6f, 0.6f)); //You can LERP through colours to make night to day cycles
	DeferredLighting->SendUniform("LightDiffuse", glm::vec3(0.6f, 0.6f, 0.6f));
	DeferredLighting->SendUniform("LightSpecular", glm::vec3(0.6f, 0.6f, 0.6f));
	DeferredLighting->SendUniform("LightSpecularExponent", 500.0f);

	DeferredComposite.Bind();

	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(0));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ShadowMap.GetDepthHandle());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(1));
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(2));



	DrawFullScreenQuad();

	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE); //Why was this not here in week 10 vid?

	DeferredComposite.UnBind();
	DeferredLighting->UnBind();

	//===============================================================
		//DeferredComposite.Bind();
	DeferredComposite.Bind();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);  // disable writes to Z-Buffer
	glDisable(GL_DEPTH_TEST);  // disable depth-testing
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);

	//new projection
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();//save old state
	glLoadIdentity();//reset
	gluOrtho2D((float)FULLSCREEN_WIDTH * -0.5f, (float)FULLSCREEN_WIDTH * 0.5f, (float)FULLSCREEN_HEIGHT * -0.5f, (float)FULLSCREEN_HEIGHT * 0.5f);//create ortho
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();//save old state
	glLoadIdentity();//reset

	//////////////////////////
	//now ready to draw 2d
	//////////////////////////
	GBufferPass->Bind();
	hudTransform = Transform::Identity();
	GBufferPass->SendUniformMat4("uView", hudTransform.GetInverse().data, true);
	GBufferPass->SendUniformMat4("uProj", hudProjection.data, true);

	//draws everything in menu
	sortObjects(0);
	for (int i = 0; i < (int)menuObjects.size(); i++) {
		menuObjects[i]->draw(*GBufferPass, 1);
	}

	GBufferPass->UnBind();

	//restore projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();//restore state
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();//restore state

	//DeferredComposite.UnBind();
	DeferredComposite.UnBind();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);

	//===============================================================

	BloomHighPass->Bind();
	BloomHighPass->SendUniform("bloomOn", false);
	BloomHighPass->UnBind();

	/// Compute High Pass ///
	glViewport(0, 0, (GLsizei)(FULLSCREEN_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(FULLSCREEN_HEIGHT / BLOOM_DOWNSCALE));

	//Moving data to the back buffer, at the same time as our last post process
	BloomHighPass->Bind();
	BloomHighPass->SendUniform("uTex", 0);
	BloomHighPass->SendUniform("uThreshold", 1.0f);

	WorkBuffer1.Bind();

	glBindTexture(GL_TEXTURE_2D, DeferredComposite.GetColorHandle(0));
	DrawFullScreenQuad();
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	WorkBuffer1.UnBind();

	BloomHighPass->UnBind();

	/// Compute Blur ///
	glViewport(0, 0, (GLsizei)(FULLSCREEN_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(FULLSCREEN_HEIGHT / BLOOM_DOWNSCALE));
	for (int i = 0; i < BLOOM_BLUR_PASSES; i++)
	{
		//Horizontal Blur
		BlurHorizontal->Bind();
		BlurHorizontal->SendUniform("uTex", 0);
		BlurHorizontal->SendUniform("uPixelSize", 1.0f / FULLSCREEN_WIDTH);

		WorkBuffer2.Bind();

		glBindTexture(GL_TEXTURE_2D, WorkBuffer1.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);

		WorkBuffer2.UnBind();

		BlurHorizontal->UnBind();

		//Vertical Blur
		BlurVertical->Bind();
		BlurVertical->SendUniform("uTex", 0);
		BlurVertical->SendUniform("uPixelSize", 1.0f / FULLSCREEN_HEIGHT);

		WorkBuffer1.Bind();

		glBindTexture(GL_TEXTURE_2D, WorkBuffer2.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);

		WorkBuffer1.UnBind();

		BlurVertical->UnBind();
	}

	//drawHUD();


	/// Composite To Back Buffer ///
	glViewport(0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);

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

void Game::drawTime()
{
	GBuffer.Bind();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);  // disable writes to Z-Buffer
	glDisable(GL_DEPTH_TEST);  // disable depth-testing
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);

	GBufferPass->Bind();
	GBufferPass->SendUniformMat4("uView", GameCamera.CameraTransform.GetInverse().data, true);
	GBufferPass->SendUniformMat4("uProj", GameCamera.CameraProjection.data, true);

	int timer = 100 - (int)TotalGameTime;
	if (timer < 0) timer = 0;
	int secT = (int)(timer / 10);
	int secO = (int)timer % 10;
	if (secT > 9) secT = 9;
	if (secO > 9) secO = 9;

	//Draw Time
	Transform hudLoc;
	///sec tens
	hudLoc = Transform();
	hudLoc.Scale(2.0f);
	hudLoc.RotateX(14.0f);
	hudLoc.RotateY(90.0f);
	hudLoc.RotateX(-90.0f);
	hudLoc.Translate(glm::vec3(2.1f, 15.5f, 0));
	GBufferPass->SendUniformMat4("uModel", hudLoc.data, true);
	time[secT]->Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());

	///sec ones
	hudLoc = Transform();
	hudLoc.Scale(2.0f);
	hudLoc.RotateX(14.0f);
	hudLoc.RotateY(90.0f);
	hudLoc.RotateX(-90.0f);
	hudLoc.Translate(glm::vec3(5.1f, 15.5f, 0));
	GBufferPass->SendUniformMat4("uModel", hudLoc.data, true);

	time[secO]->Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());

	//unbind last used texture
	time[secO]->UnBind();

	//glEnable(GL_DEPTH_TEST);
	//glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);

	GBufferPass->UnBind();
	GBuffer.UnBind();
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
	case 'w': //w
		//inputs[0] = true;
		break;
	case 'd': //d
		//inputs[1] = true;
		break;
	case 's': //s
		//inputs[2] = true;
		break;
	case 'a': //a
		//inputs[3] = true;
		break;
	case 'v': //n
		//inputs[4] = true;
		break;
	case 'b': //m
		//inputs[5] = true;
		break;
	case 'i': //up
		//inputs2[0] = true;
		break;
	case 'l': //right
		//inputs2[1] = true;
		break;
	case 'k': //down
		//inputs2[2] = true;
		break;
	case 'j': //left
		//inputs2[3] = true;
		break;
	case '.': //a
		//inputs2[4] = true;
		break;
	case '/': //b
		//inputs2[5] = true;
		break;
	}
}

void Game::keyboardUp(unsigned char key, int mouseX, int mouseY)
{
	switch(key)
	{
	case 'R': //w
	case 'r': //w
		if (scene == 3) {
			scene = 3;
			players[0]->respawn();
			players[0]->facingRight = true;
			players[1]->respawn();
			players[0]->setPosition(glm::vec3(-5, 0, 0));
			players[1]->setPosition(glm::vec3(5, 0, 0));
			players[1]->facingRight = false;
			//updateTimer = new Timer();
			TotalGameTime = 0.0f;
			lastInputTime = 0.0f;
		}
		else {
			//players[0] = (new Knight("./Assets/Models/Knight.obj", "./Assets/Textures/player1.png"));
			//players[1] = (new Ninja("./Assets/Models/Knight.obj", "./Assets/Textures/player2.png"));
			p1Char = 1;
			p2Char = 2;
			players[0] = new charBlueDragon(blueTemp);
			players[1] = new charBlueDragon(blueTemp);
			//players[1] = new charRedDevil(ninjaTemp);
			players[1]->bodyTexture.Load("./Assets/Textures/blueDtextureBlue.png");
			
			stageVal = 1;
			//hide ninja court
			findObjects(3, "stage3_env")->hide = true;
			for (int i = 0; i < (int)stage3_env_objs.size(); i++)
				findObjects(3, stage3_env_objs[i])->hide = true;
			//hide knight court
			findObjects(3, "stage2_env")->hide = true;
			for (int i = 0; i < (int)stage2_env_objs.size(); i++)
				findObjects(3, stage2_env_objs[i])->hide = true;
			//show default court
			findObjects(3, "stage1_env")->hide = false;
			for (int i = 0; i < (int)stage1_env_objs.size(); i++)
				findObjects(3, stage1_env_objs[i])->hide = false;
			scene = 3;
			players[0]->respawn();
			players[0]->facingRight = true;
			players[1]->respawn();
			players[1]->setPosition(glm::vec3(5, 0, 0));
			players[0]->setPosition(glm::vec3(-5, 0, 0));
			players[1]->facingRight = false;
			//updateTimer = new Timer();
			TotalGameTime = 0.0f;
			lastInputTime = 0.0f;
		}
		break;
	case 'Q': //w
	case 'q': //w
		stageDone = false;
		stageVal = 1;
		scene = 0;
		TotalGameTime = 0.0f;
		lastInputTime = 0.0f;
		p1Done = false;
		p2Done = false;
		p1Char = 0;
		p2Char = 0;
		isNinja1 = false;
		isNinja2 = false;
		onePlaying = false;
		thirtyPlaying = false;
		break;
	case 'W': //w
	case 'w': //w
		stageDone = false;
		stageVal = 1;
		scene = 1;
		TotalGameTime = 0.0f;
		lastInputTime = 0.0f;
		p1Done = false;
		p2Done = false;
		p1Char = 0;
		p2Char = 0;
		isNinja1 = false;
		isNinja2 = false;
		onePlaying = false;
		thirtyPlaying = false;

		break;
	case 'E': //w
	case 'e': //w
		scene = 2;
		stageDone = false;
		stageVal = 1;
		TotalGameTime = 0.0f;
		lastInputTime = 0.0f;
		p1Done = false;
		p2Done = false;
		p1Char = 1;
		p2Char = 1;
		TotalGameTime = 0.0f;
		lastInputTime = 0.0f;
		isNinja1 = false;
		isNinja2 = false;
		onePlaying = false;
		thirtyPlaying = false;

		break;
	case 'd': //d
		//inputs[1] = false;
		stageDone = false;
		stageVal = 1;
		scene = 4;
		TotalGameTime = 0.0f;
		lastInputTime = 0.0f;
		p1Done = false;
		p2Done = false;
		p1Char = 0;
		p2Char = 0;
		isNinja1 = false;
		isNinja2 = false;
		onePlaying = false;
		thirtyPlaying = false;
		break;
	case 's': //s
		//inputs[2] = false;
		break;
	case 'a': //a
		//inputs[3] = false;
		break;
	case 'v': //n
		//inputs[4] = false;
		break;
	case 'b': //m
		if (grayscale == false)
			grayscale = true;
		else if (grayscale == true)
			grayscale = false;
		break;
	case 'i': //up
		//inputs2[0] = false;
		break;
	case 'l': //right
		//inputs2[1] = false;
		break;
	case 'k': //down
		//inputs2[2] = false;
		break;
	case 'j': //left
		//inputs2[3] = false;
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
		//GBufferPass->ReloadShader();
		//AdShader.ReloadShader();
		//BloomHighPass->ReloadShader();
		//PointLight.ReloadShader();
		//NetShader.ReloadShader();
		DeferredLighting->ReloadShader();
		std::cout << "Reloaded Shaders\n";
		//inputs2[4] = false;
		break;
	case '/': //b
		//inputs2[5] = false;
		break;
	case 'm':
		for (int i = 0; i < 2; i++) {
			players[i]->currentHealth = players[i]->maxHealth;
			players[i]->greyHealth = players[i]->maxHealth;
		}
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

void Game::loadTime() {
	
	for (int i = 0; i <= 10; i++)
	{
		Texture * temp = new Texture();
		if (!temp->Load("./Assets/Textures/" + std::to_string(i) + ".png"))
		{
			std::cout << "Time Texture failed to load.\n";
			system("pause");
			exit(0);
		}
		time.push_back(temp);
	}
}

void Game::sortObjects(unsigned int scene) {
	if (scene == 3) {
		//go through list
		for (int i = 0; i < (int)(gameObjects.size() - 1); i++) {
			for (int j = i + 1; j < (int)gameObjects.size(); j++) {

				float dist1 = (float)(GameCamera.CameraTransform.GetTranslation() - gameObjects[i]->transform.GetTranslation()).length();
				float dist2 = (float)(GameCamera.CameraTransform.GetTranslation() - gameObjects[j]->transform.GetTranslation()).length();

				if ((dist1 < dist2 && gameObjects[i]->blending) || (gameObjects[i]->blending && !gameObjects[j]->blending)) {
					Object* temp = gameObjects[i];
					gameObjects[i] = gameObjects[j];
					gameObjects[j] = temp;
				}

			}
		}
	}
	else if (scene == 0){
		//go through list
		for (int i = 0; i < (int)(menuObjects.size() - 1); i++) {
			for (int j = i + 1; j < (int)menuObjects.size(); j++) {

				float dist1 = (float)(GameCamera.CameraTransform.GetTranslation() - menuObjects[i]->transform.GetTranslation()).length();
				float dist2 = (float)(GameCamera.CameraTransform.GetTranslation() - menuObjects[j]->transform.GetTranslation()).length();

				if ((dist1 < dist2 && menuObjects[i]->blending) || (menuObjects[i]->blending && !menuObjects[j]->blending)) {
					Object* temp = menuObjects[i];
					menuObjects[i] = menuObjects[j];
					menuObjects[j] = temp;
				}

			}
		}
	}
	else if (scene == 1) {
		//go through list
		for (int i = 0; i < (int)(cssObjects.size() - 1); i++) {
			for (int j = i + 1; j < (int)cssObjects.size(); j++) {

				float dist1 = (float)(GameCamera.CameraTransform.GetTranslation() - cssObjects[i]->transform.GetTranslation()).length();
				float dist2 = (float)(GameCamera.CameraTransform.GetTranslation() - cssObjects[j]->transform.GetTranslation()).length();

				if ((dist1 < dist2 && cssObjects[i]->blending) || (cssObjects[i]->blending && !cssObjects[j]->blending)) {
					Object* temp = cssObjects[i];
					cssObjects[i] = cssObjects[j];
					cssObjects[j] = temp;
				}

			}
		}
	}
	else if (scene == 2) {
		//go through list
		for (int i = 0; i < (int)(sssObjects.size() - 1); i++) {
			for (int j = i + 1; j < (int)sssObjects.size(); j++) {

				float dist1 = (float)(GameCamera.CameraTransform.GetTranslation() - sssObjects[i]->transform.GetTranslation()).length();
				float dist2 = (float)(GameCamera.CameraTransform.GetTranslation() - sssObjects[j]->transform.GetTranslation()).length();

				if ((dist1 < dist2 && sssObjects[i]->blending) || (sssObjects[i]->blending && !sssObjects[j]->blending)) {
					Object* temp = sssObjects[i];
					sssObjects[i] = sssObjects[j];
					sssObjects[j] = temp;
				}

			}
		}
	}
}

Object* Game::findObjects(unsigned int scene, std::string _name)
{
	if (scene == 3) {
		for (int i = 0; i < (int)gameObjects.size(); i++) {

			if (gameObjects[i]->name == _name)
				return gameObjects[i];

		}
	}
	else if (scene == 0){
		for (int i = 0; i < (int)menuObjects.size(); i++) {

			if (menuObjects[i]->name == _name)
				return menuObjects[i];

		}
	}
	else if (scene == 1) {
		for (int i = 0; i < (int)cssObjects.size(); i++) {

			if (cssObjects[i]->name == _name)
				return cssObjects[i];

		}
	}
	else if (scene == 2) {
		for (int i = 0; i < (int)sssObjects.size(); i++) {

			if (sssObjects[i]->name == _name)
				return sssObjects[i];

		}
	}
	return nullptr;
}

PointLightObj* Game::findLight(std::string _name)
{
	for (int i = 0; i < (int)pointLights.size(); i++) {

		if (pointLights[i]->name == _name)
			return pointLights[i];

	}
	return nullptr;
}