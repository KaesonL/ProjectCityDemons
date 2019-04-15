/*==============================================================================
3D Example
Copyright (c), Firelight Technologies Pty, Ltd 2004-2018.
This example shows how to basic 3D positioning of sounds.

Brent Cowan Mar. 6, 2019

Zachary Allen February 18, 2019
- Added the basic FMOD implementation
John Wang March 1, 2019
- Added the StopPlaying function
Gil Robern - March 11, 2019
- Combined the functionality from the different tutorials into one wrapper

==============================================================================*/

#pragma once	
#pragma comment(lib, "FMOD/lib/fmod_vc.lib")

#include <Windows.h>
#include <iostream>
#include <vector>

#include "FMOD/inc/fmod.hpp"
#include"FMOD/inc/fmod_errors.h"



void FmodErrorCheck(FMOD_RESULT result);
struct Listener
{
	FMOD_VECTOR pos = { 0.0f, 0.0f, -1.0f };
	FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
	//listeners head, how its rotated
	FMOD_VECTOR forward = { 0.0f, 0.0f, 1.0f };
	FMOD_VECTOR up = { 0.0f, 1.0f, 0.0f };
};

//Sound engine class
class SoundEngine
{
public:
	~SoundEngine();
	bool Init();
	void update();
	
	FMOD::System *system;
	Listener listener;

private:
	unsigned int version;
	void *extradriverdata = 0;
	bool init = false;
	FMOD_RESULT result;

};
//Sound Class
class Sound
{
public:
	~Sound();
	bool Load(const char* fileName, bool is3D = false, bool loop = false);
	static void Stop(FMOD::Channel* thisChannel);	//stops sound from playing
	FMOD::Channel* Play(FMOD_VECTOR startPos = { 0.0f, 0.0f, 0.0f }, FMOD_VECTOR startVel = { 0.0f, 0.0f, 0.0f }, bool loop = false);
	static void SetPosition(FMOD::Channel* thisChannel, FMOD_VECTOR newPos, FMOD_VECTOR newVel = {0.0f, 0.0f, 0.0f});
	static void SetRollOff(FMOD::Channel * thisChannel, bool linear, float min, float max);

	float Random(float min, float max);

	FMOD::Sound *soundData;
	FMOD::Sound	*gameSound;
	FMOD_VECTOR pos = { 0.0f, 0.0f, 0.0f };
	FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };

	static SoundEngine engine;

private:
	FMOD::Channel *channel = 0; //channel is the sound that is playing, ref this if u want to move etc. 
	bool init = false;
	static FMOD_RESULT result;
	bool is3D;
};

struct ReverbData
{
	FMOD::Reverb3D *reverb;
	FMOD_VECTOR pos = { 0.0f, 0.0f, 0.0f };
	float min = 10.0f;
	float max = 70.0f;
};

class ReverbManager
{
public:
	~ReverbManager();
	void AddNode(FMOD_VECTOR pos, float min, float max, FMOD_REVERB_PROPERTIES props);
	void Clear();
	//Add a get node function
	//Add a remove node function
	std::vector<ReverbData> nodes;

};