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

class Overlay {

public:
	Overlay() {
		HPShader = new ShaderProgram();
		DefShader = new ShaderProgram();

		// Load P1 HP Bars
		P1_BarHP = new Object("./Assets/Models/UI_Object", "./Assets/Textures/Bar0.png", "P1_BarHP");
		P1_BarHP->setScale(280.0f);
		P1_BarHP->RotateY(90.0f);
		P1_BarHP->setPosition(glm::vec3(-535, 370, -0.1));
		P1_BarOverlay = new Object("./Assets/Models/UI_Object", "./Assets/Textures/Overlay0.png", "P1_BarOverlay", true);
		P1_BarOverlay->setScale(280.0f);
		P1_BarOverlay->RotateY(90.0f);
		P1_BarOverlay->setPosition(glm::vec3(-535, 370, -0.2));
		P1_BarBackground = new Object("./Assets/Models/UI_Object", "./Assets/Textures/HudBack0.png", "P1_BarBackground", true);
		P1_BarBackground->setScale(280.0f);
		P1_BarBackground->RotateY(90.0f);
		P1_BarBackground->setPosition(glm::vec3(-535, 370, 0));
		//P1_CharPortrait = new Object("./Assets/Models/basicCourtHigh", "./Assets/Textures/basicCourt.png", "P1_CharPortrait");
		//P1_CharPortrait->setScale(280.0f);
		//P1_CharPortrait->RotateY(90.0f);
		//P1_CharPortrait->setPosition(glm::vec3(-535, 370, 0));
		//P1_CharName = new Object("./Assets/Models/basicCourtHigh", "./Assets/Textures/basicCourt.png", "P1_CharName");
		//P1_CharName->setScale(280.0f);
		//P1_CharName->RotateY(90.0f);
		//P1_CharName->setPosition(glm::vec3(-535, 370, 0));

		// Load P2 HP Bars
		P2_BarHP = new Object("./Assets/Models/UI_Object", "./Assets/Textures/Bar1.png", "P2_BarHP");
		P2_BarHP->setScale(280.0f);
		P2_BarHP->RotateY(90.0f);
		P2_BarHP->setPosition(glm::vec3(535, 370, 0.2));
		P2_BarOverlay = new Object("./Assets/Models/UI_Object", "./Assets/Textures/Overlay1.png", "P2_BarOverlay", true);
		P2_BarOverlay->setScale(280.0f);
		P2_BarOverlay->RotateY(90.0f);
		P2_BarOverlay->setPosition(glm::vec3(535, 370, 0.1));
		P2_BarBackground = new Object("./Assets/Models/UI_Object", "./Assets/Textures/HudBack1.png", "P2_BarBackground", true);
		P2_BarBackground->setScale(280.0f);
		P2_BarBackground->RotateY(90.0f);
		P2_BarBackground->setPosition(glm::vec3(535, 370, 0.3));
		//P2_CharPortrait = new Object("./Assets/Models/basicCourtHigh", "./Assets/Textures/basicCourt.png", "P2_CharPortrait");
		//P2_CharPortrait->setScale(280.0f);
		//P2_CharPortrait->RotateY(90.0f);
		//P2_CharPortrait->setPosition(glm::vec3(535, 370, 0));
		//P2_CharName = new Object("./Assets/Models/basicCourtHigh", "./Assets/Textures/basicCourt.png", "P2_CharName");
		//P2_CharName->setScale(280.0f);
		//P2_CharName->RotateY(90.0f);
		//P2_CharName->setPosition(glm::vec3(535, 370, 0));
	
		// Load Timer Number
		for (int i = 0; i <= 10; i++){
			Texture * temp = new Texture();
			if (!temp->Load("./Assets/Textures/" + std::to_string(i) + ".png"))
			{
				std::cout << "Time Texture failed to load.\n";
				system("pause");
				exit(0);
			}
			TimerNumbers.push_back(temp);
		}

		// Load Timer Mesh
		std::vector<std::string> hud1;
		hud1.push_back("./Assets/Models/UI_Object");
		TimerNumberMesh.LoadFromFile(hud1);

		// Set Timer Tens Location
		TimerTensTransform = Transform::Identity();
		TimerTensTransform.Scale(80.0f);
		TimerTensTransform.RotateY(90.0f);
		TimerTensTransform.RotateX(-90.0f);
		TimerTensTransform.Translate(glm::vec3(20, 450, 0));

		// Set Timer Ones Location
		TimerOnesTransform = Transform::Identity();
		TimerOnesTransform.Scale(80.0f);
		TimerOnesTransform.RotateY(90.0f);
		TimerOnesTransform.RotateX(-90.0f);
		TimerOnesTransform.Translate(glm::vec3(135, 450, 0.1f));

		// Load Shaders
		if (!HPShader->Load("./Assets/Shaders/StaticGeometry.vert", "./Assets/Shaders/HPShader.frag")) {
			std::cout << "GBP Shaders failed to initialize.\n";
			system("pause");
			exit(0);
		}
		if (!DefShader->Load("./Assets/Shaders/StaticGeometry.vert", "./Assets/Shaders/GBufferPass.frag")) {
			std::cout << "GBP Shaders failed to initialize.\n";
			system("pause");
			exit(0);
		}

		// Load Projection Matrix
		hudProjection = Transform::OrthographicProjection((float)1920 * -0.5f, (float)1920 * 0.5f, (float)1080 * 0.5f, (float)1080 * -0.5f, -10.0f, 100.0f);
	
	}
	~Overlay() {

	}

	void to3DMode() {
		//restore projection matrix
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();//restore state
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();//restore state

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);
		glEnable(GL_LIGHTING);
		glDisable(GL_BLEND);
	}

	void to2DMode() {
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_FALSE);  // disable writes to Z-Buffer
		glDisable(GL_DEPTH_TEST);  // disable depth-testing
		glDisable(GL_CULL_FACE);
		glDisable(GL_LIGHTING);

		//new projection
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();//save old state
		glLoadIdentity();//reset
		//gluOrtho2D(0.0, 1.0, 0.0, 1.0);//create ortho
		gluOrtho2D((float)1920 * -0.5f, (float)1920 * 0.5f, (float)1080 * -0.5f, (float)1080 * 0.5f);//create ortho
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();//save old state
		glLoadIdentity();//reset
	}

	void draw(Character* players[2], float totatGameTime) {

		to2DMode();

		// Draw Backgrounds
		DefShader->Bind();
		DefShader->SendUniformMat4("uView", Transform::Identity().GetInverse().data, true);
		DefShader->SendUniformMat4("uProj", hudProjection.data, true);
		P1_BarBackground->draw(*DefShader, 0.0f);
		P2_BarBackground->draw(*DefShader, 0.0f);

		// Draw Bars
		drawBars(players);

		// Draw Overlays
		DefShader->Bind();
		DefShader->SendUniformMat4("uView", Transform::Identity().GetInverse().data, true);
		DefShader->SendUniformMat4("uProj", hudProjection.data, true);
		P1_BarOverlay->draw(*DefShader, 0.0f);
		P2_BarOverlay->draw(*DefShader, 0.0f);

		// Draw Timer
		int timer = 100 - (int)totatGameTime;
		if (timer < 0) timer = 0;
		int secT = (int)(timer / 10);
		int secO = (int)timer % 10;
		if (secT > 9) secT = 9;
		if (secO > 9) secO = 9;

		//Draw Time
		glEnable(GL_BLEND);
		///sec tens
		DefShader->SendUniformMat4("uModel", TimerTensTransform.data, true);
		TimerNumbers[secT]->Bind();
		glBindVertexArray(TimerNumberMesh.VAO);
		glDrawArrays(GL_TRIANGLES, 0, TimerNumberMesh.GetNumVertices());
		///sec ones
		DefShader->SendUniformMat4("uModel", TimerOnesTransform.data, true);
		TimerNumbers[secO]->Bind();
		glBindVertexArray(TimerNumberMesh.VAO);
		glDrawArrays(GL_TRIANGLES, 0, TimerNumberMesh.GetNumVertices());

		DefShader->UnBind();
		TimerNumbers[secO]->UnBind();

		to3DMode();
	}

	void drawBars(Character* players[2]) {

		glEnable(GL_BLEND);

		HPShader->Bind();
		HPShader->SendUniformMat4("uView", Transform::Identity().GetInverse().data, true);
		HPShader->SendUniformMat4("uProj", hudProjection.data, true);

		// Draw P1 Bar
		HPShader->SendUniformMat4("uModel", P1_BarHP->transform.data, true);
		HPShader->SendUniform("uMax", players[0]->maxHealth);
		HPShader->SendUniform("uPlayer", 0);//0 right side, 1,left
		///draw grey
		HPShader->SendUniform("uHP", players[0]->greyHealth);
		HPShader->SendUniform("uGrey", true);
		P1_BarHP->texture.Bind();
		glBindVertexArray(P1_BarHP->body.VAO);
		glDrawArrays(GL_TRIANGLES, 0, P1_BarHP->body.GetNumVertices());
		///draw hp
		HPShader->SendUniform("uHP", players[0]->currentHealth);
		HPShader->SendUniform("uGrey", false);
		glBindVertexArray(P1_BarHP->body.VAO);
		glDrawArrays(GL_TRIANGLES, 0, P1_BarHP->body.GetNumVertices());

		///unbind
		P1_BarHP->texture.UnBind();

		// Draw P2 bar
		HPShader->SendUniformMat4("uModel", P2_BarHP->transform.data, true);
		HPShader->SendUniform("uMax", players[1]->maxHealth);
		HPShader->SendUniform("uPlayer", 1);//0 right side, 1,left
		///draw grey
		HPShader->SendUniform("uHP", players[1]->greyHealth);
		HPShader->SendUniform("uGrey", true);
		P2_BarHP->texture.Bind();
		glBindVertexArray(P2_BarHP->body.VAO);
		glDrawArrays(GL_TRIANGLES, 0, P2_BarHP->body.GetNumVertices());
		///draw hp
		HPShader->SendUniform("uHP", players[1]->currentHealth);
		HPShader->SendUniform("uGrey", false);
		glBindVertexArray(P2_BarHP->body.VAO);
		glDrawArrays(GL_TRIANGLES, 0, P2_BarHP->body.GetNumVertices());

		///unbind
		P2_BarHP->texture.UnBind();
		HPShader->UnBind();
	}

private:
	Transform hudProjection;

	// Entities
	Object* P1_BarHP;
	Object* P1_BarOverlay;
	Object* P1_BarBackground;
	//Object* P1_CharPortrait;
	//Object* P1_CharName;

	Object* P2_BarHP;
	Object* P2_BarOverlay;
	Object* P2_BarBackground;
	//Object* P2_CharPortrait;
	//Object* P2_CharName;

	// Timer
	vector<Texture*> TimerNumbers;
	Mesh TimerNumberMesh;
	Transform TimerTensTransform;
	Transform TimerOnesTransform;

	// HUD Shaders
	ShaderProgram* DefShader;
	ShaderProgram* HPShader;

};