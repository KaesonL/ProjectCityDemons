#pragma once
#include "controller.h"
#include <glm/glm.hpp>

/*
enum Button
{
	A = 0x0, B = 0x1, X = 0x2, Y = 0x3,
	RB = 0x4, LB = 0x5, R3 = 0x6, L3 = 0x7,
	Start = 0x8, Select = 0x9,
	DPadLeft = 0xA, DPadRight = 0xB, DPadUp = 0xC, DPadDown = 0xD
};
*/

enum MyInputs
{
	Basic = 0x0, Clear = 0x1, Meteor = 0x2,
	Jump = 0x3,
	Shift = 0x4, Dash = 0x5,
	Start = 0x6,
	Left = 0x7, Right = 0x8, Up = 0x9, Down = 0xA
};

class InputHandler{

public:

	InputHandler();
	~InputHandler();

	//mapable inputs id
	Input::Button _Basic[4];
	Input::Button _Clear[4];
	Input::Button _Meteor[4];
	Input::Button _Jump[4];
	Input::Button _Shift1[4];
	Input::Button _Shift2[4];
	Input::Button _Dash1[4];
	Input::Button _Dash2[4];
	Input::Button _Start[4];

	float _stickSens[4];
	float _triggerSens[4];

	bool hasDash2[4];
	bool hasShift2[4];
	bool tapJump[4];
	bool pad[4];

	void update();

	bool SetVibration(int _index, float _leftPower, float _rightPower);
	
	void setDefaultControlles(unsigned int index);

	//get key down at all
	bool getButton(int index, MyInputs input);
	//key pressed
	bool getButtonDown(int index, MyInputs input);
	//key released
	bool getButtonUp(int index, MyInputs input);
	//analog
	glm::vec2 getLeftStick(int index);
	glm::vec2 getRightStick(int index);
	glm::vec2 getLeftStickLast(int index);
	glm::vec2 getRightStickLast(int index);
	

	
private:

	Input::XBoxInput* ControllerSystem;

	bool m_buttonStatesLast[4][11];
	std::pair<float, float> m_triggersLast[4];
	Input::Stick m_leftSticksLast[4];
	Input::Stick m_rightSticksLast[4];

	//frames since last buttonDown() for each button
	unsigned int buttonBufferTime[4][16];
	//amount of frames of buffer allowed
	unsigned int bufferTime[4];
};