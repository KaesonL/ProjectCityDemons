#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "Texture.h"
#include "Transform.h"
#include <fstream>
#include "VectorField.h"

//debugging
#include <chrono>
#include <iostream>

using namespace std;

struct ParticleList
{
	glm::vec3 *Positions = nullptr;
	glm::vec3 *Velocities = nullptr;
	float *Size = nullptr;
	float *Alpha = nullptr;
	float *Ages = nullptr;
	float *Lifetimes = nullptr;
	int *frequency = nullptr;
};

class ParticleEffect
{
public:
	ParticleEffect();
	~ParticleEffect();

	bool Init(const std::string &textureFile, unsigned int maxParticles, unsigned int rate);
	//use this instead of init when you want to parse from Partimaker
	bool PartiParse(const std::string &ParseFile, const std::string &textureFile);

	void Update(float elapsed);
	void Render();
	void Reset();
	/*
	Spawn: Function used to begin spawning particles
	@time: Time for particles to spawn
	@reset: if previous particles get deleted upon play
	*/
	void Spawn(float time, bool reset = false);
	void Spawn();

	Transform transform;

	float spawnerTime = 1.0f;
	float savedSpawnerTime = 1.0f;
	glm::vec2 RangeX = glm::vec2(-10.0f, 10.0f);
	glm::vec2 RangeY = glm::vec2(0.0f, 10.0f);
	glm::vec2 RangeZ = glm::vec2(-10.0f, 10.0f);
	glm::vec2 RangeVelocity = glm::vec2(0.1f, 1.0f);
	glm::vec2 RangeLifetime = glm::vec2(7.0f, 9.5f);
	//Lerp variables are dynamic based on lifetime of particle
	glm::vec2 LerpAlpha = glm::vec2(0.5f, 0.0f);
	glm::vec2 LerpSize = glm::vec2(0.0f, 1.0f);
	glm::vec2 InitialXRange = glm::vec2(1.0f, -1.0f);
	glm::vec2 InitialYRange = glm::vec2(1.0f, -1.0f);
	float circleRadius = 0.0f;
	float ringRadius = 0.0f;
	bool circleSpawner = false;
	float ringWidth = 1.0f;

	//noise
	float noiseStrength = 0.0f;
	int noiseFrequency = 0;
	bool noiseOn = false;

	bool HaveGravity = false;
	bool Playing = false;
	float Mass;
	float Gravity = 0.0f;
	glm::vec3 force;
	glm::vec3 acceleration;
	
	VectorField mainField;

private:
	ParticleList _Particles;
	Texture _Texture;

	int _Rate = 0;
	unsigned int _MaxParticles = 0;
	unsigned int _NumCurrentParticles = 0;

	GLuint _VAO = 0;
	GLuint _VBO_Position = 0;
	GLuint _VBO_Size = 0;
	GLuint _VBO_Alpha = 0;


};
