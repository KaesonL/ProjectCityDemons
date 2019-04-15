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


#include "FmodWrapper.h"
#include "FMOD/inc/fmod.hpp"

using namespace std;

void FmodErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		cout << FMOD_ErrorString(result) << endl;
		system("pause");
	}
}

SoundEngine::~SoundEngine()
{
	if (init)
	{
		result = system->close();
		FmodErrorCheck(result);
		result = system->release();
		FmodErrorCheck(result);
		init = false;
	}
}

void SoundEngine::update()
{
	//update the listener
	result = system->set3DListenerAttributes(0, &listener.pos, &listener.vel, &listener.forward, &listener.up);
	FmodErrorCheck(result);

	//sounds get played
	result = system->update(); //update function, dont put in game loop more than once
	FmodErrorCheck(result);
}

bool SoundEngine::Init()
{
	//Create a System object and initialize.

	if(!init)
	{
		result = FMOD::System_Create(&system);
		FmodErrorCheck(result);

		if (result != FMOD_OK) return false;

		result = system->getVersion(&version);
		FmodErrorCheck(result);

		result = system->init(100, FMOD_INIT_NORMAL, extradriverdata);
		FmodErrorCheck(result);

		//Set the distance units
		result = system->set3DSettings(1.0, 1.0, 1.0f);
		FmodErrorCheck(result);

		init = true;
	}

	return true;
}

FMOD_RESULT Sound::result;
SoundEngine Sound::engine;

Sound::~Sound()
{
	if (init)
	{
		result = soundData->release();
		FmodErrorCheck(result);
	}
}

bool Sound::Load(const char * fileName, bool is3D, bool loop)
{
	if (!init)
	{
		engine.Init();

		this->is3D = is3D;
		FMOD_MODE mode = FMOD_DEFAULT;
		if (is3D) mode = mode | FMOD_3D;
		else mode = mode | FMOD_2D;

		if (loop) mode = mode | FMOD_LOOP_NORMAL;
		else mode = mode | FMOD_LOOP_OFF;

		result = engine.system->createSound(fileName, mode, 0, &soundData);
		FmodErrorCheck(result);
		if (result != FMOD_OK) return false;

		if (is3D)
		{
			result = soundData->set3DMinMaxDistance(0.5f, 500.0f);
			FmodErrorCheck(result);
		}

		init = true;
	}
	return true;


}

FMOD::Channel* Sound::Play(FMOD_VECTOR startPos, FMOD_VECTOR startVel, bool loop)
{
	if (init)
	{
		result = engine.system->playSound(soundData, 0, true, &channel); //changing channel example
		FmodErrorCheck(result);

		if (is3D)
		{
			result = channel->set3DAttributes(&startPos, &startVel);
			FmodErrorCheck(result);
		}

		if (loop)
		{
			//if we want it to loop
			channel->setMode(FMOD_LOOP_NORMAL);
		}
		else
		{
			channel->setMode(FMOD_LOOP_OFF);
		}
	

		result = channel->setPaused(false);
		FmodErrorCheck(result);
	}

	return channel;
}

//Better solution could be to set volume to zero then wait a bit then stop
void Sound::Stop(FMOD::Channel* thisChannel)
{
	if (thisChannel)
	{
		thisChannel->stop(); //This can cause audio pops
		thisChannel = NULL;
	}
}

void Sound::SetPosition(FMOD::Channel * thisChannel, FMOD_VECTOR newPos, FMOD_VECTOR newVel)
{
	//Check to make sure the channel is playing
	bool isPlaying;
	thisChannel->isPlaying(&isPlaying);
	if (isPlaying)
	{
		result = thisChannel->set3DAttributes(&newPos, &newVel);
		FmodErrorCheck(result);
	}
}

void Sound::SetRollOff(FMOD::Channel * thisChannel, bool linear, float min, float max)
{
	//Check to make sure the channel is playing
	bool isPlaying;
	thisChannel->isPlaying(&isPlaying);
	if (isPlaying)
	{
		result = thisChannel->set3DMinMaxDistance(min, max);
		FmodErrorCheck(result);

		if (linear)
			thisChannel->setMode(FMOD_3D_LINEARROLLOFF);
		else
			thisChannel->setMode(FMOD_3D_INVERSEROLLOFF);
	}
}
//Generates a random float between min and max
float Sound::Random(float min, float max)
{
	return (float(rand()) / float(RAND_MAX))*(max - min) + min;
}

ReverbManager::~ReverbManager()
{
	Clear();
}

void ReverbManager::AddNode(FMOD_VECTOR pos, float min, float max, FMOD_REVERB_PROPERTIES props)
{
	ReverbData tempData;

	tempData.pos = pos;
	tempData.min = min;
	tempData.max = max;

	Sound::engine.system->createReverb3D(&tempData.reverb);
	tempData.reverb->setProperties(&props);

	tempData.reverb->set3DAttributes(&pos, min, max);
	tempData.reverb->setActive(true);

	nodes.push_back(tempData);
}

void ReverbManager::Clear()
{
	for (unsigned int i = 0; i < nodes.size(); i++)
	{
		nodes[i].reverb->release();
	}
	nodes.clear();
}
