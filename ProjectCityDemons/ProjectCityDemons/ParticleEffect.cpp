#include "ParticleEffect.h"
#include "Random.h"

#define BUFFER_OFFSET(i) ((char *)0 + (i))

ParticleEffect::ParticleEffect()
{
	Gravity = 0.0f;
	Mass = 2.0f;
}

ParticleEffect::~ParticleEffect()
{
	if (_VAO != GL_NONE)
	{
		glDeleteVertexArrays(1, &_VAO);
	}

	if (_VBO_Position != GL_NONE)
	{
		glDeleteVertexArrays(1, &_VBO_Position);
	}

	if (_VBO_Size != GL_NONE)
	{
		glDeleteVertexArrays(1, &_VBO_Size);
	}

	if (_VBO_Alpha != GL_NONE)
	{
		glDeleteVertexArrays(1, &_VBO_Alpha);
	}

	if (_Particles.Positions != nullptr)
	{
		delete[] _Particles.Positions;
		delete[] _Particles.Velocities;
		delete[] _Particles.Alpha;
		delete[] _Particles.Ages;
		delete[] _Particles.Lifetimes;
		delete[] _Particles.Size;
	}
}

bool ParticleEffect::Init(const std::string &textureFile, unsigned int maxParticles, unsigned int rate)
{
	if (!_Texture.Load(textureFile))
	{
		std::cout << "Could not open the Texture File." << std::endl;
		return false;
	}

	_MaxParticles = maxParticles;
	_Rate = rate;

	_Particles.Positions = new glm::vec3[_MaxParticles];
	_Particles.Velocities = new glm::vec3[_MaxParticles];
	_Particles.Alpha = new float[_MaxParticles];
	_Particles.Ages = new float[_MaxParticles];
	_Particles.Lifetimes = new float[_MaxParticles];
	_Particles.Size = new float[_MaxParticles];
	_Particles.frequency = new int[_MaxParticles];

	//Setup OpenGL Memory
	glGenVertexArrays(1, &_VAO);
	glGenBuffers(1, &_VBO_Position);
	glGenBuffers(1, &_VBO_Size);
	glGenBuffers(1, &_VBO_Alpha);


	glBindVertexArray(_VAO);

	glEnableVertexAttribArray(0);	//Vertex
	glEnableVertexAttribArray(1);	//Size
	glEnableVertexAttribArray(2);	//Alpha

	glBindBuffer(GL_ARRAY_BUFFER, _VBO_Position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * _MaxParticles, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glBindBuffer(GL_ARRAY_BUFFER, _VBO_Size);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _MaxParticles, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer((GLuint)1, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));


	glBindBuffer(GL_ARRAY_BUFFER, _VBO_Alpha);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _MaxParticles, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer((GLuint)2, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	glBindVertexArray(GL_NONE);

	return true;
}

bool ParticleEffect::PartiParse(const std::string & ParseFile, const std::string &textureFile)
{
	ifstream input;
	input.open(ParseFile);

	if (!input) {
		std::cout << "Could not open the Parse File." << std::endl;
		return false;
	}
	std::string inputLine;

	while (!input.eof()) {
		getline(input, inputLine);

		//empty
		if (inputLine.empty()) continue;

		//a - Max Particles
		if (!inputLine.find("a")) {
			sscanf_s(inputLine.c_str(), "a%i", &_MaxParticles);
			continue;
		}
		//b - Rate
		if (!inputLine.find("b")) {
			sscanf_s(inputLine.c_str(), "b%i", &_Rate);
			continue;
		}
		//c - Spawner time
		if (!inputLine.find("c")) {
			sscanf_s(inputLine.c_str(), "c%f", &savedSpawnerTime);
			continue;
		}
		//d - Gravity
		if (!inputLine.find("d")) {
			sscanf_s(inputLine.c_str(), "d%f", &Gravity);
			continue;
		}
		//e - Mass
		if (!inputLine.find("e")) {
			sscanf_s(inputLine.c_str(), "e%f", &Mass);
			continue;
		}
		//f - Range Pos
		if (!inputLine.find("f")) {
			sscanf_s(inputLine.c_str(), "f%f,%f/%f,%f/%f,%f", &RangeX.x, &RangeX.y, &RangeY.x, &RangeY.y, &RangeZ.x, &RangeZ.y);
			continue;
		}
		//g - Range Vel
		if (!inputLine.find("g")) {
			sscanf_s(inputLine.c_str(), "g%f,%f", &RangeVelocity.x, &RangeVelocity.y);
			continue;
		}
		//h - Range Lifetime
		if (!inputLine.find("h")) {
			sscanf_s(inputLine.c_str(), "h%f,%f", &RangeLifetime.x, &RangeLifetime.y);
			continue;
		}
		//i - Lerp Alpha
		if (!inputLine.find("i")) {
			sscanf_s(inputLine.c_str(), "i%f,%f", &LerpAlpha.x, &LerpAlpha.y);
			continue;
		}
		//j - Lerp Size
		if (!inputLine.find("j")) {
			sscanf_s(inputLine.c_str(), "j%f,%f", &LerpSize.x,&LerpSize.y);
			continue;
		}
		//k - Innitial Vel
		if (!inputLine.find("k")) {
			sscanf_s(inputLine.c_str(), "k%f,%f/%f,%f", &InitialXRange.x, &InitialXRange.y, &InitialYRange.x, &InitialYRange.y);
			continue;
		}
		//l - Noise
		if (!inputLine.find("l")) {
			noiseOn = true;
			sscanf_s(inputLine.c_str(), "l%f,%i", &noiseStrength, &noiseFrequency);
			continue;
		}
		if (!inputLine.find("m")) {
			circleSpawner = true;
			sscanf_s(inputLine.c_str(), "m%f,%f, %f", &circleRadius, &ringRadius, &ringWidth);
			std::cout << circleRadius << "," << ringRadius;
			continue;
		}
	}
	Init(textureFile, _MaxParticles, _Rate);

	return true;
}

void ParticleEffect::Update(float elapsed)
{
	//auto end = chrono::steady_clock::now();
	//auto start = chrono::steady_clock::now();

	int NumToSpawn = (int)_Rate;
	if (spawnerTime < 0.0f) {
		Playing = false;
	}
	spawnerTime -= elapsed;

		/// Create new particles ///
	while (
		//We have not reached the particle cap and...
		_NumCurrentParticles < _MaxParticles && 
		//We have more particles to generate this frame...
		NumToSpawn > 0 &&
		//We have not reached the time limit to keep spawning...
		Playing
		)
	{
		_Particles.Alpha[_NumCurrentParticles] = RandomRangef(LerpAlpha.x, LerpAlpha.y);
		_Particles.Ages[_NumCurrentParticles] = 0.0f;
		_Particles.Lifetimes[_NumCurrentParticles] = RandomRangef(RangeLifetime.x, RangeLifetime.y);
		_Particles.Size[_NumCurrentParticles] = RandomRangef(LerpSize.x, LerpSize.y);
		//Missing .Set which is what the video uses
		//_Particles.Positions[_NumCurrentParticles] = vec3((RandomRangef(RangeX.x, RangeX.y), RandomRangef(RangeY.x, RangeY.y), RandomRangef(RangeZ.x, RangeZ.y)));
		_Particles.Positions[_NumCurrentParticles].x = RandomRangef(RangeX.x, RangeX.y);
		_Particles.Positions[_NumCurrentParticles].y = RandomRangef(RangeY.x, RangeY.y);
		_Particles.Positions[_NumCurrentParticles].z = 0.0f;

		//custom spawners:
		if (circleSpawner) {
			glm::vec3 offset = glm::vec3(RandomRangef(-1.0f, 1.0f), RandomRangef(-1.0f, 1.0f), 0.0f);
			offset = glm::normalize(offset);
			_Particles.Positions[_NumCurrentParticles] += offset * RandomRangef(ringRadius, circleRadius) * glm::vec3(ringWidth,1.0f,0.0f);

		}


		//send the particle in a random direction, with a velocity between our range
		//Missing .Set which is what the video uses
		//_Particles.Velocities[_NumCurrentParticles] = vec3((RandomRangef(-1.0f, 1.0f), RandomRangef(-1.0f, 1.0f), RandomRangef(-1.0f, 1.0f)));
		_Particles.Velocities[_NumCurrentParticles].x = RandomRangef(InitialXRange.x, InitialXRange.y);
		_Particles.Velocities[_NumCurrentParticles].y = RandomRangef(InitialYRange.x, InitialYRange.y);
		_Particles.Velocities[_NumCurrentParticles].z = 0.0f;
		_Particles.Velocities[_NumCurrentParticles] = glm::normalize(_Particles.Velocities[_NumCurrentParticles]);
		_Particles.Velocities[_NumCurrentParticles] *= RandomRangef(RangeVelocity.x, RangeVelocity.y);

		_Particles.frequency[_NumCurrentParticles] = noiseFrequency;

		//counters...
		_NumCurrentParticles++;
		NumToSpawn--;
	}
	
	if (_NumCurrentParticles > 0) {
		/// Update existing particles ///s
		for (unsigned i = 0; i < _NumCurrentParticles; i++)
		{
			_Particles.Ages[i] += elapsed;
			_Particles.frequency[i]++;

			//Explanation of this is on Week 9 video at time 5:30 (maybe)
			if (_Particles.Ages[i] > _Particles.Lifetimes[i])
			{
				//remove the particle by replacing it with the one at the top of the stack
				_Particles.Alpha[i] = _Particles.Alpha[_NumCurrentParticles - 1];
				_Particles.Ages[i] = _Particles.Ages[_NumCurrentParticles - 1];
				_Particles.Lifetimes[i] = _Particles.Lifetimes[_NumCurrentParticles - 1];
				_Particles.Positions[i] = _Particles.Positions[_NumCurrentParticles - 1];
				_Particles.Size[i] = _Particles.Size[_NumCurrentParticles - 1];
				_Particles.Velocities[i] = _Particles.Velocities[_NumCurrentParticles - 1];
				_NumCurrentParticles--;
				continue;
			}

			//noise
			if (_Particles.frequency[i] >= noiseFrequency && noiseOn)
			{
					_Particles.Velocities[i] += glm::vec3(RandomRangef(-1.0f, 1.0f), RandomRangef(-1.0f, 1.0f),0) * noiseStrength;
				_Particles.frequency[i] = 0;
			}

			//adds feild weight if exists
			if (mainField.used) {
				glm::vec2 totalWeight = mainField.totalWeight(_Particles.Positions[i]);
				_Particles.Velocities[i] *= 0.97f;												//remember to add drag ######################################
				_Particles.Velocities[i] += glm::vec3(totalWeight, 0.0f);
			}

			//physics update
			force = glm::vec3(0, 0 - Gravity, 0);
			acceleration = force / Mass;
			_Particles.Velocities[i] += acceleration;
			_Particles.Positions[i] += _Particles.Velocities[i] * elapsed;

			float interp = _Particles.Ages[i] / _Particles.Lifetimes[i];

			_Particles.Alpha[i] = glm::mix(LerpAlpha.x, LerpAlpha.y, interp);
			_Particles.Size[i] = glm::mix(LerpSize.x, LerpSize.y, interp);




		}
		//end = chrono::steady_clock::now();

		//cout << chrono::duration_cast<chrono::milliseconds>(end - start).count() << ":";

		//Update OpenGL on the changes

		glBindBuffer(GL_ARRAY_BUFFER, _VBO_Position);
		//Call this because glBufferData will reallocate the entire array :O
		//This will update the data in an existing array, update a small subset as you need to
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * _NumCurrentParticles, &_Particles.Positions[0]);

		glBindBuffer(GL_ARRAY_BUFFER, _VBO_Size);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * _NumCurrentParticles, &_Particles.Size[0]);

		glBindBuffer(GL_ARRAY_BUFFER, _VBO_Alpha);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * _NumCurrentParticles, &_Particles.Alpha[0]);

		glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	}
	//end = chrono::steady_clock::now();

	//cout << chrono::duration_cast<chrono::milliseconds>(end - start).count() << ":x";
}

void ParticleEffect::Render()
{
	if (_NumCurrentParticles == 0)
	{
		return;
	}

	_Texture.Bind();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//Depth mask disables/enables the ability for a render to write to the depth buffer
	glDepthMask(GL_FALSE);

	glBindVertexArray(_VAO);
	glDrawArrays(GL_POINTS, 0, _NumCurrentParticles); //Thousands of particles can be drawn in one call! 
	glBindVertexArray(GL_NONE);

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	_Texture.UnBind();
}

void ParticleEffect::Reset()
{
	for (unsigned i = 0; i < _NumCurrentParticles; i++)
	{
		_Particles.Ages[i] = 5.0f;
	}
}

void ParticleEffect::Spawn(float time, bool reset)
{
	if (reset) {
		Reset();
	}
	Playing = true;
	spawnerTime = time;
	
}

void ParticleEffect::Spawn()
{
	Playing = true;
	spawnerTime = savedSpawnerTime;
}
