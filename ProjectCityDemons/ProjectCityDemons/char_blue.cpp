#include "char_blue.h"

//new push
#define BASE_ANI_TOGGLE		true	//non-offensive animations
#define G_ATK_ANI_TOGGLE	true	//ground attacks
#define A_ATK_ANI_TOGGLE	true	//aerials
#define S_ATK_ANI_TOGGLE	true	//specials

charBlueDragon::charBlueDragon(const std::string& textureName) {
	type = 1;

	for (int i = 0; i < 40; i++) {
		aniSpeeds[i] = 7.0f;
	}
	aniSpeeds[ACTION_A_HIT] = 14.0f;
	aniSpeeds[ACTION_G_HIT] = 10.0f;
	aniSpeeds[ACTION_FALL] = 40.0f;
	aniSpeeds[ACTION_JUMP] = 4.0f;
	aniSpeeds[ACTION_JUMPB] = 4.0f;
	aniSpeeds[ACTION_JUMPF] = 4.0f;
	aniSpeeds[ACTION_G_METEOR] = 3.0f;//smaller = faster
	aniSpeeds[ACTION_A_METEOR_ALT_2] = 4.0f;
	aniSpeeds[ACTION_G_METEOR_ALT_2] = 5.0f;//smaller = faster
	aniSpeeds[ACTION_G_METEOR_ALT] = 6.0f;
	aniSpeeds[ACTION_G_BASIC_2] = 7.0f;
	aniSpeeds[ACTION_G_BASIC_3] = 5.0f;
	aniSpeeds[ACTION_G_CLEAR] = 5.0f;
	aniSpeeds[ACTION_G_CLEAR_ALT] = 4.0f;
	aniSpeeds[ACTION_A_METEOR] = 3.0f;
	aniSpeeds[ACTION_A_BASIC] = 8.0f;
	aniSpeeds[ACTION_A_CLEAR] = 6.0f;
	aniSpeeds[ACTION_A_CLEAR_ALT] = 4.0f;

	if (!(bodyTexture.Load(textureName))){
		std::cout << "Character Texture failed to load.\n";
		system("pause");
		exit(0);
	}
	if (!(hurtTexture.Load("./Assets/Textures/hurt.png"))){
		std::cout << "Character Texture failed to load.\n";
		system("pause");
		exit(0);
	}
	activeTexture = &bodyTexture;

	aniTimer = 0.f;
	index = 0;

	//======================================================//
						//ANIMATIONS
	//======================================================//

		///IDLE
	int length = 8;//9
	if (BASE_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c)//9
	{
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/Blue/_Idle/pose" + std::to_string(c));
		frame.push_back("./Assets/Models/Blue/_Idle/pose" + std::to_string((int)((c + 1) % length)));
		Mesh* idle = new Mesh();
		idle->LoadFromFile(frame);
		aniFrames[ACTION_IDLE].push_back(idle);
	}
	///WALK
	length = 1;
	if (BASE_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c){
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/Blue/_Walk/pose" + std::to_string(c));
		frame.push_back("./Assets/Models/Blue/_Walk/pose" + std::to_string((int)((c + 1) % length)));
		Mesh* walk = new Mesh();
		walk->LoadFromFile(frame);
		aniFrames[ACTION_WALKF].push_back(walk);
		Mesh* walk2 = new Mesh();
		walk2->LoadFromFile(frame);
		aniFrames[ACTION_WALKB].push_back(walk2);
	}
	///pre jump
	length = 2;
	if (BASE_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c){
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/Blue/_Squat/pose" + std::to_string(c));
		frame.push_back("./Assets/Models/Blue/_Squat/pose" + std::to_string((int)((c + 1) % length)));
		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_PREJUMP].push_back(jab);
	}
	///fall
	length = 2;
	if (BASE_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c){
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/Blue/_Fall/pose" + std::to_string(c));
		frame.push_back("./Assets/Models/Blue/_Fall/pose" + std::to_string((int)((c + 1) % length)));
		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_FALL].push_back(jab);
	}
	///jump
	length = 2;
	if (BASE_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c){
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/Blue/_Jump/pose" + std::to_string(c));
		frame.push_back("./Assets/Models/Blue/_Jump/pose" + std::to_string((int)((c + 1) % length)));

		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_JUMP].push_back(jab);
		Mesh* jab2 = new Mesh();
		jab2->LoadFromFile(frame);
		aniFrames[ACTION_JUMPF].push_back(jab2);
		Mesh* jab3 = new Mesh();
		jab3->LoadFromFile(frame);
		aniFrames[ACTION_JUMPB].push_back(jab3);
	}
	///hurt
	length = 3;
	if (BASE_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c){
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/Blue/_aHurt/pose" + std::to_string(c));
		frame.push_back("./Assets/Models/Blue/_aHurt/pose" + std::to_string((int)((c + 1) % length)));
		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_A_HIT].push_back(jab);
	}
	///hurt ground
	length = 4;
	if (BASE_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c){
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/Blue/_Hurt/pose" + std::to_string(c));
		frame.push_back("./Assets/Models/Blue/_Hurt/pose" + std::to_string((int)((c + 1) % length)));
		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_G_HIT].push_back(jab);
	}
	///launched
	length = 1;
	if (BASE_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c) {
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/Blue/_Launched/pose" + std::to_string(c));
		frame.push_back("./Assets/Models/Blue/_Launched/pose" + std::to_string((int)((c + 1) % length)));
		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_LAUNCHED_K].push_back(jab);
		Mesh* jab2 = new Mesh();
		jab2->LoadFromFile(frame);
		aniFrames[ACTION_LAUNCHED_B].push_back(jab2);
	}
	///bounce
	length = 2;
	if (BASE_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c) {
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/Blue/_BounceG/pose" + std::to_string(c));
		frame.push_back("./Assets/Models/Blue/_BounceG/pose" + std::to_string((int)((c + 1) % length)));
		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_BOUNCEG].push_back(jab);
	}
	///bounce
	length = 2;
	if (BASE_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c) {
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/Blue/_BounceW/pose" + std::to_string(c));
		frame.push_back("./Assets/Models/Blue/_BounceW/pose" + std::to_string((int)((c + 1) % length)));
		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_BOUNCEW].push_back(jab);
	}
	///knockdown
	length = 1;
	if (BASE_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c) {
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/Blue/_Knockdown/pose" + std::to_string(c));
		frame.push_back("./Assets/Models/Blue/_Knockdown/pose" + std::to_string((int)((c + 1) % length)));
		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_KNOCKDOWN].push_back(jab);
	}
	///getup
	length = 2;
	if (BASE_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c) {
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/Blue/_Squat/pose" + std::to_string(c));
		frame.push_back("./Assets/Models/Blue/_Squat/pose" + std::to_string((int)((c + 1) % length)));
		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_GETUP].push_back(jab);
	}
	///initial dash
	length = 1;
	if (BASE_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c){
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/Blue/_Dash/pose" + std::to_string(c));
		frame.push_back("./Assets/Models/Blue/_Dash/pose" + std::to_string((int)((c + 1) % length)));
		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_G_DASHF].push_back(jab);
		Mesh* jab2 = new Mesh();
		jab2->LoadFromFile(frame);
		aniFrames[ACTION_G_DASHB].push_back(jab2);
		Mesh* jab3 = new Mesh();
		jab3->LoadFromFile(frame);
		aniFrames[ACTION_A_DASHF].push_back(jab3);
		Mesh* jab4 = new Mesh();
		jab4->LoadFromFile(frame);
		aniFrames[ACTION_A_DASHB].push_back(jab4);
	}
	//==================================================================//
								//ATTACKS
	//==================================================================//
	///gBasic
	length = 3;
	if (G_ATK_ANI_TOGGLE == false) length = 1;
	for (int c = 0; c < length; ++c){
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/Blue/gBasic1/pose" + std::to_string(c));
		frame.push_back("./Assets/Models/Blue/gBasic1/pose" + std::to_string((int)((c + 1) % length)));
		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_G_BASIC].push_back(jab);
	}
	///gBasic2
	length = 3;
	if (G_ATK_ANI_TOGGLE == false) length = 1;
	for (int c = 0; c < length; ++c) {
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/Blue/gBasic2/pose" + std::to_string(c));
		frame.push_back("./Assets/Models/Blue/gBasic2/pose" + std::to_string((int)((c + 1) % length)));
		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_G_BASIC_2].push_back(jab);
	}
	///gBasic3
	length = 5;
	if (G_ATK_ANI_TOGGLE == false) length = 1;
	for (int c = 0; c < length; ++c) {
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/Blue/gBasic3/pose" + std::to_string(c));
		frame.push_back("./Assets/Models/Blue/gBasic3/pose" + std::to_string((int)((c + 1) % length)));
		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_G_BASIC_3].push_back(jab);
	}
	///gClear
	length = 5;
	if (G_ATK_ANI_TOGGLE == false) length = 1;
	for (int c = 0; c < length; ++c){
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/Blue/gBasic3/pose" + std::to_string(c) );
		frame.push_back("./Assets/Models/Blue/gBasic3/pose" + std::to_string((int)((c + 1) % length)) );
		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_G_CLEAR].push_back(jab);
	}
	///gMeteor
	length = 11;
	if (G_ATK_ANI_TOGGLE == false) length = 1;
	for (int c = 0; c < length; ++c){
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/Blue/gMeteor/pose" + std::to_string(c) );
		frame.push_back("./Assets/Models/Blue/gMeteor/pose" + std::to_string((int)((c + 1) % length)) );
		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_G_METEOR].push_back(jab);
	}
	///gBasicAlt
	length = 5;
	if (G_ATK_ANI_TOGGLE == false) length = 1;
	for (int c = 0; c < length; ++c) {
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/Blue/gBasic3/pose" + std::to_string(c));
		frame.push_back("./Assets/Models/Blue/gBasic3/pose" + std::to_string((int)((c + 1) % length)));
		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_G_BASIC_ALT].push_back(jab);
	}
	///gClearAlt
	length = 7;
	if (S_ATK_ANI_TOGGLE == false) length = 1;
	for (int c = 0; c < length; ++c) {
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/Blue/gClearAlt/pose" + std::to_string(c));
		frame.push_back("./Assets/Models/Blue/gClearAlt/pose" + std::to_string((int)((c + 1) % length)));
		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_G_CLEAR_ALT].push_back(jab);
	}
	///gMeteorAlt
	length = 6;
	if (S_ATK_ANI_TOGGLE == false) length = 1;
	for (int c = 0; c < length; ++c) {
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/Blue/gMeteorAlt1/pose" + std::to_string(c));
		frame.push_back("./Assets/Models/Blue/gMeteorAlt1/pose" + std::to_string((int)((c + 1) % length)));
		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_G_METEOR_ALT].push_back(jab);
	}
	///gMeteorAlt
	length = 9;
	if (S_ATK_ANI_TOGGLE == false) length = 1;
	for (int c = 0; c < length; ++c) {
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/Blue/gMeteorAlt2/pose" + std::to_string(c));
		frame.push_back("./Assets/Models/Blue/gMeteorAlt2/pose" + std::to_string((int)((c + 1) % length)));
		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_G_METEOR_ALT_2].push_back(jab);
	}

	//==================================================================//
								//AERIALS
	//==================================================================//

	///aBasic
	length = 5;
	if (A_ATK_ANI_TOGGLE == false) length = 1;
	for (int c = 0; c < length; ++c){
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/Blue/aBasic/pose" + std::to_string(c) );
		frame.push_back("./Assets/Models/Blue/aBasic/pose" + std::to_string((int)((c + 1) % length)) );
		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_A_BASIC].push_back(jab);
	}
	///aMeteor
	length = 9;
	if (A_ATK_ANI_TOGGLE == false) length = 1;
	for (int c = 0; c < length; ++c){
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/Blue/aMeteor/pose" + std::to_string(c) );
		frame.push_back("./Assets/Models/Blue/aMeteor/pose" + std::to_string((int)((c + 1) % length)) );
		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_A_METEOR].push_back(jab);
	}
	///aClear
	length = 6;
	if (A_ATK_ANI_TOGGLE == false) length = 1;
	for (int c = 0; c < length; ++c){
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/Blue/aClear/pose" + std::to_string(c) );
		frame.push_back("./Assets/Models/Blue/aClear/pose" + std::to_string((int)((c + 1) % length)) );
		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_A_CLEAR].push_back(jab);
	}
	///aBasicAlt
	length = 5;
	if (A_ATK_ANI_TOGGLE == false) length = 1;
	for (int c = 0; c < length; ++c) {
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/Blue/aBasic/pose" + std::to_string(c));
		frame.push_back("./Assets/Models/Blue/aBasic/pose" + std::to_string((int)((c + 1) % length)));
		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_A_BASIC_ALT].push_back(jab);
	}
	///aMeteorAlt1
	length = 7;
	if (S_ATK_ANI_TOGGLE == false) length = 1;
	for (int c = 0; c < length; ++c){
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/Blue/aMeteorAlt1/pose" + std::to_string(c));
		frame.push_back("./Assets/Models/Blue/aMeteorAlt1/pose" + std::to_string((int)((c + 1) % length)));

		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_A_METEOR_ALT].push_back(jab);
	}
	///aMeteorAlt2
	length = 6;
	if (S_ATK_ANI_TOGGLE == false) length = 1;
	for (int c = 0; c < length; ++c) {
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/Blue/aMeteorAlt2/pose" + std::to_string(c));
		frame.push_back("./Assets/Models/Blue/aMeteorAlt2/pose" + std::to_string((int)((c + 1) % length)));

		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_A_METEOR_ALT_2].push_back(jab);
	}
	///aClearAlt
	length = 7;
	if (S_ATK_ANI_TOGGLE == false) length = 1;
	for (int c = 0; c < length; ++c){
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/Blue/aClearAlt/pose" + std::to_string(c));
		frame.push_back("./Assets/Models/Blue/aClearAlt/pose" + std::to_string((int)((c + 1) % length)));
		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_A_CLEAR_ALT].push_back(jab);
	}

	//extra
	std::vector<std::string> file;
	file.push_back("./Assets/Models/Blue/_Idle/pose" + std::to_string(0) );
	body.LoadFromFile(file);

	//Set Physics
	position = glm::vec3(0, 0, 0);
	velocity = glm::vec3(0, 0, 0);
	acceleration = glm::vec3(0, 0, 0);
	force = glm::vec3(0, 0, 0);
	facingRight = true;
	//scaling
	scaleX = 3.5f;
	scaleY = 3.5f;
	scaleZ = 3.5f;

	//apply Scale && Rotation
	transform.Scale(glm::vec3(scaleX, scaleY, scaleZ));
	transform.RotateY(90);//turn to face side

	//==============================================================
	//					Set Attributes
	//==============================================================
	///Mass
	mass = 9;
	///Gravity Force on Character
	gravity = 0.29f;
	///Multiplier for directional influence while character is in hitstun
	diMultiplier = 0.1f;
	///Multiplier applied to max speed used to decide speed of dash
	dashSpeed = 0.4f;
	///max run speed
	maxSpeed = 0.3f;
	///force applied for running
	walkSpeed = 0.2f;//0.52f;
	///force appplied for directional movement in air
	airAccel = 0.2f;
	///upwards force for jump
	jumpForce = 0.4f;//0.44f;
	jumpForceX = 0.2f;//0.44f;
	///amount of frames jump last for
	jumpFrames = 6;//12;
	///run InitialDash length (in frames)
	dashLength = 4;
	///number of frames before character leaves ground after jump input
	prejumpLength = 3;
	///total number of air/double jumps
	airJumps = jumpsLeft = 1;
	///amount of frames character is stunned after being launched
	hitstun = 10;//10;
	///amount of frames character is launched for when hit
	hitframes = 14;
	bounceframes = 10;

	///Health Pool
	currentHealth = 1000;
	maxHealth = 1000;
	greyHealth = 1000;

	//set combo stuff
	comboCount = 0;
	comboTimer = 0;
	comboMaxTime = 120;//1 seconds times 60fps

	//hitbox visualization
	std::vector<std::string> hitBox;
	hitBox.push_back("./Assets/Models/Hitbox");
	boxMesh.LoadFromFile(hitBox);
	if (!(boxTexture.Load("./Assets/Textures/redclear.png"))){
		std::cout << "Knight Texture failed to load.\n";
		system("pause");
		exit(0);
	}
	if (!(shieldTexture.Load("./Assets/Textures/shield.png"))){
		std::cout << "Knight Texture failed to load.\n";
		system("pause");
		exit(0);
	}

	//hurtbox position initialization
	Hitbox *hurt1 = new Hitbox(glm::vec3(0.0f, 3.0f, 0.0f), 3.0f);
	hurtbox.push_back(hurt1);
	Hitbox *hurt2 = new Hitbox(glm::vec3(0.0f, 1.0f, 0.0f), 3.0f);
	hurtbox.push_back(hurt2);

	//Set Starting Action
	action = ACTION_FALL;
	Idle();
}

//=============================================================//
//						GROUND ATTACKS
//=============================================================//

Transform charBlueDragon::gBasic()
{
	Transform result;
	//Called First Frame
	if (interuptable == true && action != ACTION_G_BASIC) {
		interuptable = false;
		action = ACTION_G_BASIC;
		activeFrames = (unsigned int)(aniSpeeds[ACTION_G_BASIC] * ((float)(aniFrames[ACTION_G_BASIC].size()) - 1.0f));
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
		cancel = false;
	}
	//Auto Finish
	if (action != ACTION_G_BASIC || currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		cancel = false;
		return Idle();
	}
	//Actions Per Frame
	switch (currentFrame) {
	case 2: {
		//spawn hitbox
		Hitbox *newAtk = new Hitbox(
			glm::vec3((-0.5f + (int)facingRight)*2.2f, 3.0f, 0.1f),///Position
			3.0f,						///Size
			0,							///Knockback (base kb + combo scale)
			4,							///Scale
			85,							///Angle
			4,							///LifeTime
			15,							///Damage
			glm::vec3((-0.5f + (int)facingRight)*0.9, 0, 0));		///Velocity
		newAtk->facingRight = facingRight;
		activeHitboxes.push_back(newAtk);
		break;
	}
	case 7://cancel into jab2
		cancel = true;
		break;
	default:
		break;
	}
	currentFrame++;
	return result;
}


Transform charBlueDragon::gBasic2()
{
	Transform result;
	//Called First Frame
	if (interuptable == true && action != ACTION_G_BASIC_2) {
		interuptable = false;
		action = ACTION_G_BASIC_2;
		activeFrames = (unsigned int)(aniSpeeds[ACTION_G_BASIC_2] * ((float)(aniFrames[ACTION_G_BASIC_2].size()) - 1.0f));
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
		cancel = false;
	}
	//Auto Finish
	if (action != ACTION_G_BASIC_2 || currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		cancel = false;
		return Idle();
	}
	//Actions Per Frame
	switch (currentFrame) {
	case 4: {
		//spawn hitbox
		Hitbox *newAtk = new Hitbox(
			glm::vec3((-0.5f + (int)facingRight)*2.0f, 3.0f, 0.1f),///Position
			3.0f,						///Size
			0,	///Knockback (base kb + combo scale)
			4,							///Scaling
			85,							///Angle
			4,							///LifeTime
			15,							///Damage
			glm::vec3((-0.5f + (int)facingRight)*0.5, 0, 0));		///Velocity
		newAtk->facingRight = facingRight;
		activeHitboxes.push_back(newAtk);
		break;
	}
	case 7://cancel into jab3
		cancel = true;
		break;
	default:
		break;
	}
	currentFrame++;
	return result;
}

Transform charBlueDragon::gBasic3()
{
	Transform result;
	//Called First Frame
	if (interuptable == true && action != ACTION_G_BASIC_3) {
		interuptable = false;
		action = ACTION_G_BASIC_3;
		activeFrames = (unsigned int)(aniSpeeds[ACTION_G_BASIC_3] * ((float)(aniFrames[ACTION_G_BASIC_3].size()) - 1.0f));
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
		cancel = false;
	}
	//Auto Finish
	if (action != ACTION_G_BASIC_3 || currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		cancel = false;
		return Idle();
	}
	//Actions Per Frame
	switch (currentFrame) {
	case 8: {
		//spawn hitbox
		Hitbox *newAtk = new Hitbox(
			glm::vec3((-0.5f + (int)facingRight)*1.7f, 3.2f, 0.1f),///Position
			3.0f,						///Size
			15,							///Knockback (base kb + combo scale)
			1,							///Scaling
			20,							///Angle
			5,							///LifeTime
			20,							///Damage
			glm::vec3((-0.5f + (int)facingRight)*1.3, 0.2f, 0));		///Velocity
		newAtk->facingRight = facingRight;
		activeHitboxes.push_back(newAtk);
		break;
	}
	default:
		break;
	}
	currentFrame++;
	return result;
}

Transform charBlueDragon::gMeteor()
{
	Transform result;
	//Called First Frame
	if (interuptable == true && action != ACTION_G_METEOR) {
		interuptable = false;
		action = ACTION_G_METEOR;
		activeFrames = (unsigned int)(aniSpeeds[ACTION_G_METEOR] * ((float)(aniFrames[ACTION_G_METEOR].size()) - 1.0f));
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
	}
	//Auto Finish
	if (action != ACTION_G_METEOR || currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		return Idle();
	}
	//Actions Per Frame
	switch (currentFrame) {
	case 8: {
		//spawn hitbox
		Hitbox *newAtk = new Hitbox(
			glm::vec3((-0.5f + (int)facingRight)*5.5f, 2.8f, 0.1f),///Position
			4.0f,						///Size
			7,							///Knockback (base kb + combo scale)
			1,							///Scaling
			65,							///Angle
			6,							///LifeTime
			15,							///Damage
			glm::vec3((-0.5f + (int)facingRight)*0.5f, 0.4f, 0));		///Velocity
		newAtk->facingRight = facingRight;
		activeHitboxes.push_back(newAtk);
		break;
	}
	case 18: {
		//spawn hitbox
		Hitbox *newAtk = new Hitbox(
			glm::vec3((-0.5f + (int)facingRight)*7.4f, 5.8f, 0.1f),///Position
			4.0f,						///Size
			20,							///Knockback (base kb + combo scale)
			0,							///Scaling
			-85,							///Angle
			4,							///LifeTime
			15,							///Damage
			glm::vec3(0, -1.2f, 0));		///Velocity
		newAtk->facingRight = facingRight;
		activeHitboxes.push_back(newAtk);
		break;
	}
	default:
		break;
	}
	currentFrame++;
	return result;
}

Transform charBlueDragon::gClear()
{
	Transform result;
	//Called First Frame
	if (interuptable == true && action != ACTION_G_CLEAR) {
		interuptable = false;
		action = ACTION_G_CLEAR;
		activeFrames = (unsigned int)(aniSpeeds[ACTION_G_CLEAR] * ((float)(aniFrames[ACTION_G_CLEAR].size()) - 1.0f));
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
	}
	//Auto Finish
	if (action != ACTION_G_CLEAR || currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		return Idle();
	}
	//Actions Per Frame
	switch (currentFrame) {
	case 5: {
		//spawn hitbox
		Hitbox *newAtk = new Hitbox(
			glm::vec3((-0.5f + (int)facingRight)*1.7f, 3.2f, 0.1f),///Position
			3.0f,						///Size
			11,							///Knockback (base kb + combo scale)
			1,							///Scaling
			45,							///Angle
			5,							///LifeTime
			40,							///Damage
			glm::vec3((-0.5f + (int)facingRight)*1.3, 0.2f, 0));		///Velocity
		newAtk->facingRight = facingRight;
		activeHitboxes.push_back(newAtk);
		break;
	}
	default:
		break;
	}
	currentFrame++;
	return result;
}

//=============================================================//
//						GROUND ALTS
//=============================================================//

Transform charBlueDragon::gBasicAlt()
{
	Transform result;
	//Called First Frame
	if (interuptable == true && action != ACTION_G_BASIC_ALT) {
		interuptable = false;
		action = ACTION_G_BASIC_ALT;
		activeFrames = (unsigned int)(aniSpeeds[ACTION_G_BASIC_ALT] * ((float)(aniFrames[ACTION_G_BASIC_ALT].size()) - 1.0f));
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
	}
	//Auto Finish
	if (action != ACTION_G_BASIC_ALT || currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		return Idle();
	}
	//Actions Per Frame
	switch (currentFrame) {
	case 6: {
		//spawn hitbox
		Hitbox *newAtk = new Hitbox(
			glm::vec3((-0.5f + (int)facingRight)*0.2f, 0.7f, 0.1f),///Position
			2.7f,						///Size
			6,							///Knockback (base kb + combo scale)
			1,							///Scaling
			85,							///Angle
			5,							///LifeTime
			25,							///Damage
			glm::vec3(0, 0, 0));		///Velocity
		newAtk->facingRight = facingRight;
		activeHitboxes.push_back(newAtk);
		break;
	}
	default:
		break;
	}
	currentFrame++;
	return result;
}

Transform charBlueDragon::gMeteorAlt()
{
	Transform result;
	//Called First Frame
	if (interuptable == true && action != ACTION_G_METEOR_ALT) {
		interuptable = false;
		action = ACTION_G_METEOR_ALT;
		activeFrames = (unsigned int)(aniSpeeds[ACTION_G_METEOR_ALT] * ((float)(aniFrames[ACTION_G_METEOR_ALT].size()) - 1.0f));
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
		cancel = false;
	}
	//Auto Finish
	if (action != ACTION_G_METEOR_ALT || currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		cancel = false;
		return Idle();
	}
	//Actions Per Frame
	switch (currentFrame) {
	case 6:
		cancel = true;
		break;
	case 31:
		cancel = false;
		break;
	default:
		break;
	}
	currentFrame++;
	return result;
}

Transform charBlueDragon::gMeteorAlt2()
{
	Transform result;
	//Called First Frame
	if (interuptable == true && action != ACTION_G_METEOR_ALT_2) {
		interuptable = false;
		action = ACTION_G_METEOR_ALT_2;
		activeFrames = (unsigned int)(aniSpeeds[ACTION_G_METEOR_ALT_2] * ((float)(aniFrames[ACTION_G_METEOR_ALT_2].size()) - 1.0f));
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
	}
	//Auto Finish
	if (action != ACTION_G_METEOR_ALT_2 || currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		return Idle();
	}
	//Actions Per Frame
	switch (currentFrame) {
	case 5: {
		//spawn hitbox
		Hitbox *newAtk = new Hitbox(
			glm::vec3((-0.5f + (int)facingRight)*2.0f, 1.0f, 0.1f),///Position
			3.2f,						///Size
			20,							///Knockback (base kb + combo scale)
			0,							///Scaling
			-85,							///Angle
			3,							///LifeTime
			15,							///Damage
			glm::vec3((-0.5f + (int)facingRight), 0.2f, 0));		///Velocity
		newAtk->facingRight = facingRight;
		activeHitboxes.push_back(newAtk);
		break;
	}
	case 24: {
		//spawn hitbox
		Hitbox *newAtk = new Hitbox(
			glm::vec3((-0.5f + (int)facingRight)*3.5f, 3.5f, 0.1f),///Position
			4.0f,						///Size
			30,							///Knockback (base kb + combo scale)
			1,							///Scaling
			45,							///Angle
			4,							///LifeTime
			20,							///Damage
			glm::vec3((-0.5f + (int)facingRight), 0.2f, 0));		///Velocity
		newAtk->facingRight = facingRight;
		activeHitboxes.push_back(newAtk);
		break;
	}
	default:
		break;
	}
	currentFrame++;
	return result;
}

Transform charBlueDragon::gClearAlt()
{
	Transform result;
	//Called First Frame
	if (interuptable == true && action != ACTION_G_CLEAR_ALT) {
		interuptable = false;
		action = ACTION_G_CLEAR_ALT;
		activeFrames = (unsigned int)(aniSpeeds[ACTION_G_CLEAR_ALT] * ((float)(aniFrames[ACTION_G_CLEAR_ALT].size()) - 1.0f));
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
	}
	//Auto Finish
	if (action != ACTION_G_CLEAR_ALT || currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		return Fall();
	}
	//Actions Per Frame
	switch (currentFrame) {

	case 8:
	case 10: {//multihit
		///spawn hitbox
		Hitbox *newAtk = new Hitbox(
			glm::vec3((-0.5f + (int)facingRight)*1.5f, 2.5f, 0.1f),///Position
			3.5f,						///Size
			6,							///Knockback (base kb + combo scale)
			0,							///Scaling
			85,							///Angle
			2,							///LifeTime
			20,							///Damage
			glm::vec3(0, 0, 0));		///Velocity
		newAtk->facingRight = facingRight;
		activeHitboxes.push_back(newAtk);
		velocity.y = 0.4f;
		velocity.x += 0.2f * (-0.5f + (int)(facingRight));
		break;
	}
	case 12: {//finisher
		///spawn hitbox
		Hitbox *newAtk = new Hitbox(
			glm::vec3((-0.5f + (int)facingRight)*1.5f, 2.5f, 0.1f),///Position
			4.0f,						///Size
			10,							///Power
			0,							///Scaling
			75,							///Angle
			3,							///LifeTime
			40,							///Damage
			glm::vec3(0, 0, 0));		///Velocity
		newAtk->facingRight = facingRight;
		activeHitboxes.push_back(newAtk);
		velocity.y = 0.4f;
		velocity.x += 0.2f * (-0.5f + (int)(facingRight));
		break;
	}
	default:
		break;
	}
	currentFrame++;
	return result;
}

//=============================================================//
//						AIR ATTACKS
//=============================================================//

Transform charBlueDragon::aBasic()
{
	Transform result;
	//Called First Frame
	if (interuptable == true && action != ACTION_A_BASIC) {
		interuptable = false;
		action = ACTION_A_BASIC;
		activeFrames = (unsigned int)(aniSpeeds[ACTION_A_BASIC] * ((float)(aniFrames[ACTION_A_BASIC].size()) - 1.0f));
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
	}
	//Auto Finish
	if (action != ACTION_A_BASIC || currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		return Fall();
	}
	//Actions Per Frame
	switch (currentFrame) {
	case 6: {
		//spawn hitbox
		Hitbox *newAtk = new Hitbox(
			glm::vec3((-0.5f + (int)facingRight) * 3.9, 1.9f, 0.1f),///Position
			3.9f,						///Size
			1,							///Power
			5,							///Scaling
			75,							///Angle
			15,							///LifeTime
			20,							///Damage
			glm::vec3(0, 0, 0));		///Velocity
		newAtk->facingRight = facingRight;
		activeHitboxes.push_back(newAtk);
		break;
	}
	default:
		break;
	}
	currentFrame++;
	return result;
}

Transform charBlueDragon::aMeteor()
{
	Transform result;
	//Called First Frame
	if (interuptable == true && action != ACTION_A_METEOR) {
		interuptable = false;
		action = ACTION_A_METEOR;
		activeFrames = (unsigned int)(aniSpeeds[ACTION_A_METEOR] * ((float)(aniFrames[ACTION_A_METEOR].size()) - 1.0f));
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
	}
	//Auto Finish
	if (action != ACTION_A_METEOR || currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		return Fall();
	}
	//Actions Per Frame
	switch (currentFrame) {
	case 15: {
		//spawn hitbox
		Hitbox *newAtk = new Hitbox(
			glm::vec3((-0.5f + (int)facingRight) * 3, 1.7f, 0.1f),///Position
			4.0f,						///Size
			11,							///Power
			1,							///Scaling
			-90.0f,							///Angle
			4,							///LifeTime
			25,							///Damage
			glm::vec3(0, 0, 0));		///Velocity
		newAtk->facingRight = facingRight;
		activeHitboxes.push_back(newAtk);
		break;
	}
	default:
		break;
	}
	currentFrame++;
	return result;
}

Transform charBlueDragon::aClear()
{
	Transform result;
	//Called First Frame
	if (interuptable == true && action != ACTION_A_CLEAR) {
		interuptable = false;
		action = ACTION_A_CLEAR;
		activeFrames = (unsigned int)(aniSpeeds[ACTION_A_CLEAR] * ((float)(aniFrames[ACTION_A_CLEAR].size()) - 1.0f));
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
	}
	//Auto Finish
	if (action != ACTION_A_CLEAR || currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		return Fall();
	}
	//Actions Per Frame
	switch (currentFrame) {
	case 11: {
		//spawn hitbox
		Hitbox *newAtk = new Hitbox(
			glm::vec3((-0.5f + (int)facingRight) * 3.5, 2.4f, 0.1f),///Position
			3.9f,						///Size
			11,							///Power
			1,							///Scaling
			20,							///Angle
			6,							///LifeTime
			30,							///Damage
			glm::vec3(0, 0, 0));		///Velocity
		newAtk->facingRight = facingRight;
		activeHitboxes.push_back(newAtk);
		break;
	}
	default:
		break;
	}
	currentFrame++;
	return result;
}

//=============================================================//
//							AIR ALTS
//=============================================================//

Transform charBlueDragon::aBasicAlt()
{
	Transform result;
	//Called First Frame
	if (interuptable == true && action != ACTION_A_BASIC_ALT) {
		interuptable = false;
		action = ACTION_A_BASIC_ALT;
		activeFrames = (unsigned int)(aniSpeeds[ACTION_A_BASIC_ALT] * ((float)(aniFrames[ACTION_A_BASIC_ALT].size()) - 1.0f));
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
	}
	//Auto Finish
	if (action != ACTION_A_BASIC_ALT || currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		return Fall();
	}
	//Actions Per Frame
	switch (currentFrame) {
	case 8: {
		//spawn hitbox
		Hitbox *newAtk = new Hitbox(
			glm::vec3(0.05f, 1.0f, 0.1f),///Position
			2.7f,						///Size
			12,							///Power
			0.5,							///Scaling
			60,							///Angle
			15,							///LifeTime
			0,							///Damage
			glm::vec3(0, 0, 0));		///Velocity
		newAtk->facingRight = facingRight;
		activeHitboxes.push_back(newAtk);
		break;
	}
	default:
		break;
	}
	currentFrame++;
	return result;
}
Transform charBlueDragon::aMeteorAlt()
{
	Transform result;
	//Called First Frame
	if (interuptable == true && action != ACTION_A_METEOR_ALT) {
		interuptable = false;
		action = ACTION_A_METEOR_ALT;
		activeFrames = (unsigned int)(aniSpeeds[ACTION_A_METEOR_ALT] * ((float)(aniFrames[ACTION_A_METEOR_ALT].size()) - 1.0f));
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
	}
	//Auto Finish
	if (action != ACTION_A_METEOR_ALT || currentFrame >= activeFrames || position.y == floor) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		return Idle();
	}
	//Actions Per Frame
	switch (currentFrame) {
	case 2: {
		//spawn hitbox
		Hitbox *newAtk = new Hitbox(
			glm::vec3((-0.5f + (int)facingRight) * 2.5f, 1.7f, 0.1f),///Position
			3.5f,						///Size
			11,							///Power
			1,							///Scaling
			-80.0f,							///Angle
			20,							///LifeTime
			30,							///Damage
			glm::vec3(0, 0, 0));		///Velocity
		newAtk->facingRight = facingRight;
		activeHitboxes.push_back(newAtk);
		break;
	}
	default:
		velocity.y = -0.5f;
		velocity.x += 0.3f * (-0.5f + (int)(facingRight));
		break;
	}
	currentFrame++;
	return result;
}


Transform charBlueDragon::aMeteorAlt2()
{
	Transform result;
	//Called First Frame
	if (interuptable == true && action != ACTION_A_METEOR_ALT_2) {
		interuptable = false;
		action = ACTION_A_METEOR_ALT_2;
		activeFrames = (unsigned int)(aniSpeeds[ACTION_A_METEOR_ALT_2] * ((float)(aniFrames[ACTION_A_METEOR_ALT_2].size()) - 1.0f));
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
		cancel = false;
	}
	//Auto Finish
	if (action != ACTION_A_METEOR_ALT_2 || currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		cancel = false;
		return Fall();
	}
	//Actions Per Frame
	switch (currentFrame) {
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
		velocity.y = 0.4f;
		break;
	default:
		break;
	}
	currentFrame++;
	return result;
}

Transform charBlueDragon::aClearAlt()
{
	Transform result;
	//Called First Frame
	if (interuptable == true && action != ACTION_A_CLEAR_ALT) {
		interuptable = false;
		action = ACTION_A_CLEAR_ALT;
		activeFrames = (unsigned int)(aniSpeeds[ACTION_A_CLEAR_ALT] * ((float)(aniFrames[ACTION_A_CLEAR_ALT].size()) - 1.0f));
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
	}
	//Auto Finish
	if (action != ACTION_A_CLEAR_ALT || currentFrame >= activeFrames) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		return Fall();
	}
	//Actions Per Frame
	switch (currentFrame) {

	case 8:
	case 10: {//multihit
		///spawn hitbox
		Hitbox *newAtk = new Hitbox(
			glm::vec3((-0.5f + (int)facingRight)*1.5f, 2.5f, 0.1f),///Position
			3.5f,						///Size
			5,							///Power
			0,							///Scaling
			75,							///Angle
			2,							///LifeTime
			20,							///Damage
			glm::vec3(0, 0, 0));		///Velocity
		newAtk->facingRight = facingRight;
		activeHitboxes.push_back(newAtk);
		velocity.y = 0.4f;
		velocity.x += 0.2f * (-0.5f + (int)(facingRight));
		break;
	}
	case 12: {//finisher
		///spawn hitbox
		Hitbox *newAtk = new Hitbox(
			glm::vec3((-0.5f + (int)facingRight)*1.5f, 2.5f, 0.1f),///Position
			4.0f,						///Size
			10,							///Power
			0,							///Scaling
			75,							///Angle
			3,							///LifeTime
			40,							///Damage
			glm::vec3(0, 0, 0));		///Velocity
		newAtk->facingRight = facingRight;
		activeHitboxes.push_back(newAtk);
		velocity.y = 0.4f;
		velocity.x += 0.2f * (-0.5f + (int)(facingRight));
		break;
	}
	default:
		break;
	}
	currentFrame++;
	return result;
}
