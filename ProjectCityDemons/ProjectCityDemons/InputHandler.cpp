#include "InputHandler.h"

InputHandler::InputHandler()
{
	ControllerSystem = new Input::XBoxInput();

	//default controlles
	setDefaultControlles(0);
	setDefaultControlles(1);
	setDefaultControlles(2);
	setDefaultControlles(3);

	ControllerSystem->SetTriggerButtonSens(_triggerSens);
	getButton(0, MyInputs::Basic);
	getButtonDown(0, MyInputs::Basic);
	getButtonUp(0, MyInputs::Basic);
}

InputHandler::~InputHandler()
{
	//
}

void InputHandler::update()
{
	// SET ALL INPUTS TO LAST INPUTS SPOT
	for (int i = 0; i < 4; i++) {
		if (ControllerSystem->GetConnected(i)) {
			//last frame for ButtonDown() & ButtonUp()
			m_buttonStatesLast[i][Input::Button::A] = false || ControllerSystem->m_buttonStates[i][Input::Button::A];
			m_buttonStatesLast[i][Input::Button::B] = false || ControllerSystem->m_buttonStates[i][Input::Button::B];
			m_buttonStatesLast[i][Input::Button::X] = false || ControllerSystem->m_buttonStates[i][Input::Button::X];
			m_buttonStatesLast[i][Input::Button::Y] = false || ControllerSystem->m_buttonStates[i][Input::Button::Y];
			m_buttonStatesLast[i][Input::Button::Start] = false || ControllerSystem->m_buttonStates[i][Input::Button::Start];
			m_buttonStatesLast[i][Input::Button::Select] = false || ControllerSystem->m_buttonStates[i][Input::Button::Select];
			m_buttonStatesLast[i][Input::Button::LB] = false || ControllerSystem->m_buttonStates[i][Input::Button::LB];
			m_buttonStatesLast[i][Input::Button::RB] = false || ControllerSystem->m_buttonStates[i][Input::Button::RB];
			m_buttonStatesLast[i][Input::Button::L3] = false || ControllerSystem->m_buttonStates[i][Input::Button::L3];
			m_buttonStatesLast[i][Input::Button::R3] = false || ControllerSystem->m_buttonStates[i][Input::Button::R3];
			m_buttonStatesLast[i][Input::Button::DPadDown] = false || ControllerSystem->m_buttonStates[i][Input::Button::DPadDown];
			m_buttonStatesLast[i][Input::Button::DPadUp] = false || ControllerSystem->m_buttonStates[i][Input::Button::DPadUp];
			m_buttonStatesLast[i][Input::Button::DPadRight] = false || ControllerSystem->m_buttonStates[i][Input::Button::DPadRight];
			m_buttonStatesLast[i][Input::Button::DPadLeft] = false || ControllerSystem->m_buttonStates[i][Input::Button::DPadLeft];
			m_leftSticksLast[i].xAxis = 0.0f + ControllerSystem->m_leftSticks[i].xAxis;
			m_leftSticksLast[i].yAxis = 0.0f + ControllerSystem->m_leftSticks[i].yAxis;
			m_rightSticksLast[i].xAxis = 0.0f + ControllerSystem->m_rightSticks[i].xAxis;
			m_rightSticksLast[i].yAxis = 0.0f + ControllerSystem->m_rightSticks[i].yAxis;
			m_triggersLast[i].first = 0.0f + ControllerSystem->m_triggers[i].first;
			m_triggersLast[i].second = 0.0f + ControllerSystem->m_triggers[i].second;

			//increment buffer time
			for (int j = 0; j < 11; j++) {/// must equal amount of MyInputs
				if (buttonBufferTime[i][j] <= bufferTime[i])
					buttonBufferTime[i][j]++;
			}
		}
	}

	ControllerSystem->DownloadPackets(4);
}

bool InputHandler::SetVibration(int _index, float _leftPower, float _rightPower)
{
	return ControllerSystem->SetVibration(_index, _leftPower, _rightPower);
}

void InputHandler::setDefaultControlles(unsigned int index)
{
	_Basic[index] = Input::X;
	_Clear[index] = Input::Y;
	_Meteor[index] = Input::B;
	_Jump[index] = Input::A;
	_Shift1[index] = Input::LB;
	_Shift2[index] = Input::RB;
	_Dash1[index] = Input::LT;
	_Dash2[index] = Input::RT;
	_Start[index] = Input::Start;

	_stickSens[index] = 0.3f;
	_triggerSens[index] = 0.3f;

	hasDash2[index] = true;
	hasShift2[index] = true;
	tapJump[index] = true;
	pad[index] = true;

	//default amount of buffer frames
	bufferTime[index] = 5;

	for (int j = 0; j < 11; j++) {/// must equal amount of MyInputs
			buttonBufferTime[index][j] = bufferTime[index] + 1;
	}
}

//get key down at all
bool InputHandler::getButton(int index, MyInputs input)
{
	//check if controller is connected first
	if (ControllerSystem->GetConnected(index)) {
		//check inputs
		switch (input) {
		case MyInputs::Basic:
			return (ControllerSystem->GetButton(index, _Basic[index]));
			break;

		case MyInputs::Clear:
			return (ControllerSystem->GetButton(index, _Clear[index]));
			break;

		case MyInputs::Meteor:
			return (ControllerSystem->GetButton(index, _Meteor[index]));
			break;

		case MyInputs::Jump:
			if (ControllerSystem->GetButton(index, _Jump[index]))
				return true;
			else if (tapJump[index]) {
				//if stick is pressed past threshhold
				if (getLeftStick(index).y > _stickSens[index]) {
					return true;
				}
				//if dpad pressed
				else if (ControllerSystem->GetButton(index, Input::DPadUp)) {
					return true;
				}
			}
			else
				return false;
			break;

		case MyInputs::Shift:
			if (ControllerSystem->GetButton(index, _Shift1[index]))
				return true;
			else if (hasShift2[index]) {
				if (ControllerSystem->GetButton(index, _Shift2[index]))
					return true;
			}
			else
				return false;
			break;

		case MyInputs::Dash:
			if (ControllerSystem->GetButton(index, _Dash1[index]))
				return true;
			else if (hasDash2[index]) {
				if (ControllerSystem->GetButton(index, _Dash2[index]))
					return true;
			}
			else
				return false;
			break;

		case MyInputs::Start:
			return (ControllerSystem->GetButton(index, _Start[index]));
			break;

		case MyInputs::Up:
			//if stick is pressed past threshhold
			if (getLeftStick(index).y > _stickSens[index]) {
				return true;
			}
			//if dpad pressed
			else if (ControllerSystem->GetButton(index, Input::DPadUp)) {
				return true;
			}
			else
				return false;
			break;

		case MyInputs::Down:
			//if stick is pressed past threshhold
			if (getLeftStick(index).y < -1 * _stickSens[index]) {
				return true;
			}
			//if dpad pressed
			else if (ControllerSystem->GetButton(index, Input::DPadDown)) {
				return true;
			}
			else
				return false;
			break;

		case MyInputs::Left:
			//if stick is pressed past threshhold
			if (getLeftStick(index).x < -1 * _stickSens[index]) {
				return true;
			}
			//if dpad pressed
			else if (ControllerSystem->GetButton(index, Input::DPadLeft)) {
				return true;
			}
			else
				return false;
			break;

		case MyInputs::Right:
			//if stick is pressed past threshhold
			if (getLeftStick(index).x > _stickSens[index]) {
				return true;
			}
			//if dpad pressed
			else if (ControllerSystem->GetButton(index, Input::DPadRight)) {
				return true;
			}
			else
				return false;
			break;

		default:
			break;
		}
	}
	//if controller not connected, return false
	return false;
}

//key pressed
bool InputHandler::getButtonDown(int index, MyInputs input)
{
	bool val = false;

	//check if controller is connected first
	if (ControllerSystem->GetConnected(index)) {
		//check inputs
		switch (input) {
		case MyInputs::Basic:
			val = (ControllerSystem->GetButton(index, _Basic[index]) && !m_buttonStatesLast[index][_Basic[index]]);
			break;

		case MyInputs::Clear:
			val = (ControllerSystem->GetButton(index, _Clear[index]) && !m_buttonStatesLast[index][_Clear[index]]);
			break;

		case MyInputs::Meteor:
			val = (ControllerSystem->GetButton(index, _Meteor[index]) && !m_buttonStatesLast[index][_Meteor[index]]);
			break;

		case MyInputs::Jump:
			if (ControllerSystem->GetButton(index, _Jump[index]) && !m_buttonStatesLast[index][_Jump[index]])
				val = true;
			else if (tapJump[index]) {
				//if stick is pressed past threshhold
				if (getLeftStick(index).y >= _stickSens[index] && getLeftStickLast(index).y < _stickSens[index]) {
					val = true;
				}
				//if dpad pressed
				else if (ControllerSystem->GetButton(index, Input::DPadUp) && !m_buttonStatesLast[index][Input::DPadUp]) {
					val = true;
				}
			}
			else
				val = false;
			break;

		case MyInputs::Shift:
			if (ControllerSystem->GetButton(index, _Shift1[index]) && !m_buttonStatesLast[index][_Shift1[index]])
				val = true;
			else if (hasShift2[index]) {
				if (ControllerSystem->GetButton(index, _Shift2[index]) && !m_buttonStatesLast[index][_Shift2[index]])
					val = true;
			}
			else
				val = false;
			break;

		case MyInputs::Dash:
			if (ControllerSystem->GetButton(index, _Dash1[index]) && !m_buttonStatesLast[index][_Dash1[index]])
				val = true;
			else if (hasDash2[index]) {
				if (ControllerSystem->GetButton(index, _Dash2[index]) && !m_buttonStatesLast[index][_Dash2[index]])
					val = true;
			}
			else
				val = false;
			break;

		case MyInputs::Start:
			val = (ControllerSystem->GetButton(index, _Start[index]) && !m_buttonStatesLast[index][_Start[index]]);
			break;

		case MyInputs::Up:
			//if stick is pressed past threshhold
			if (getLeftStick(index).x >= _stickSens[index] && getLeftStickLast(index).x < _stickSens[index]) {
				val = true;
			}
			//if dpad pressed
			else if (ControllerSystem->GetButton(index, Input::DPadUp) && !m_buttonStatesLast[index][Input::DPadUp]) {
				val = true;
			}
			else
				val = false;
			break;

		case MyInputs::Down:
			//if stick is pressed past threshhold
			if (getLeftStick(index).x <= (-1 * _stickSens[index]) && getLeftStickLast(index).x > (-1 * _stickSens[index])) {
				val = true;
			}
			//if dpad pressed
			else if (ControllerSystem->GetButton(index, Input::DPadDown) && !m_buttonStatesLast[index][Input::DPadDown]) {
				val = true;
			}
			else
				val = false;
			break;

		case MyInputs::Left:
			//if stick is pressed past threshhold
			if (getLeftStick(index).x <= (-1.0f * _stickSens[index]) && getLeftStickLast(index).x > (-1.0f * _stickSens[index])) {
				val = true;
			}
			//if dpad pressed
			else if (ControllerSystem->GetButton(index, Input::DPadLeft) && !m_buttonStatesLast[index][Input::DPadLeft]) {
				val = true;
			}
			else
				val = false;
			break;

		case MyInputs::Right:
			//if stick is pressed past threshhold
			if (getLeftStick(index).x >= _stickSens[index] && getLeftStickLast(index).x < _stickSens[index]) {
				val = true;
			}
			//if dpad pressed
			else if (ControllerSystem->GetButton(index, Input::DPadRight) && !m_buttonStatesLast[index][Input::DPadRight]) {
				val = true;
			}
			else
				val = false;
			break;

		default:
			val = false;
			break;
		}

		//pressed this frame
		if (val == true) {
			buttonBufferTime[index][input] = 0;
		}
		//press was buffered
		else if (buttonBufferTime[index][input] <= bufferTime[index]) {
			//catch buffered press
			val = true;
			//reset timer so buffer only coutned once
			//buttonBufferTime[index][input] = bufferTime[index] + 1;
		}
	}

	return val;
}

//key pressed
bool InputHandler::getButtonUp(int index, MyInputs input)
{
	//check if controller is connected first
	if (ControllerSystem->GetConnected(index)) {
		//check inputs
		switch (input) {
		case MyInputs::Basic:
			return (!ControllerSystem->GetButton(index, _Basic[index]) && m_buttonStatesLast[index][_Basic[index]]);
			break;

		case MyInputs::Clear:
			return (!ControllerSystem->GetButton(index, _Clear[index]) && m_buttonStatesLast[index][_Clear[index]]);
			break;

		case MyInputs::Meteor:
			return (!ControllerSystem->GetButton(index, _Meteor[index]) && m_buttonStatesLast[index][_Meteor[index]]);
			break;

		case MyInputs::Jump:
			if (!ControllerSystem->GetButton(index, _Jump[index]) && m_buttonStatesLast[index][_Jump[index]])
				return true;
			else if (tapJump[index]) {
				//if stick is pressed past threshhold
				if (getLeftStick(index).y < _stickSens[index] && getLeftStickLast(index).y > _stickSens[index]) {
					return true;
				}
				//if dpad pressed
				else if (!ControllerSystem->GetButton(index, Input::DPadUp) && m_buttonStatesLast[index][Input::DPadUp]) {
					return true;
				}
			}
			else
				return false;
			break;

		case MyInputs::Shift:
			if (!ControllerSystem->GetButton(index, _Shift1[index]) && m_buttonStatesLast[index][_Shift1[index]])
				return true;
			else if (hasShift2[index]) {
				if (!ControllerSystem->GetButton(index, _Shift2[index]) && m_buttonStatesLast[index][_Shift2[index]])
					return true;
			}
			else
				return false;
			break;

		case MyInputs::Dash:
			if (!ControllerSystem->GetButton(index, _Dash1[index]) && m_buttonStatesLast[index][_Dash1[index]])
				return true;
			else if (hasDash2[index]) {
				if (!ControllerSystem->GetButton(index, _Dash2[index]) && m_buttonStatesLast[index][_Dash2[index]])
					return true;
			}
			else
				return false;
			break;

		case MyInputs::Start:
			return (!ControllerSystem->GetButton(index, _Start[index]) && m_buttonStatesLast[index][_Start[index]]);
			break;

		case MyInputs::Up:
			//if stick is pressed past threshhold
			if (getLeftStick(index).x < _stickSens[index] && getLeftStickLast(index).x > _stickSens[index]) {
				return true;
			}
			//if dpad pressed
			else if (!ControllerSystem->GetButton(index, Input::DPadUp) && m_buttonStatesLast[index][Input::DPadUp]) {
				return true;
			}
			else
				return false;
			break;

		case MyInputs::Down:
			//if stick is pressed past threshhold
			if (getLeftStick(index).x > (-1 * _stickSens[index]) && getLeftStickLast(index).x <= (-1 * _stickSens[index])) {
				return true;
			}
			//if dpad pressed
			else if (!ControllerSystem->GetButton(index, Input::DPadDown) && m_buttonStatesLast[index][Input::DPadDown]) {
				return true;
			}
			else
				return false;
			break;

		case MyInputs::Left:
			//if stick is pressed past threshhold
			if (getLeftStick(index).x > (-1.0f * _stickSens[index]) && getLeftStickLast(index).x <= (-1.0f * _stickSens[index])) {
				return true;
			}
			//if dpad pressed
			else if (!ControllerSystem->GetButton(index, Input::DPadLeft) && m_buttonStatesLast[index][Input::DPadLeft]) {
				return true;
			}
			else
				return false;
			break;

		case MyInputs::Right:
			//if stick is pressed past threshhold
			if (getLeftStick(index).x < _stickSens[index] && getLeftStickLast(index).x >= _stickSens[index]) {
				return true;
			}
			//if dpad pressed
			else if (!ControllerSystem->GetButton(index, Input::DPadRight) && m_buttonStatesLast[index][Input::DPadRight]) {
				return true;
			}
			else
				return false;
			break;

		default:
			return false;
			break;
		}
	}
	//if controller not connected, return false
	return false;
}

glm::vec2 InputHandler::getLeftStick(int index)
{
	Input::Stick lStick, rStick;
	ControllerSystem->GetSticks(index, lStick, rStick);
	return glm::vec2(lStick.xAxis, lStick.yAxis);
}

glm::vec2 InputHandler::getRightStick(int index)
{
	Input::Stick lStick, rStick;
	ControllerSystem->GetSticks(index, lStick, rStick);
	return glm::vec2(rStick.xAxis, rStick.yAxis);
}

glm::vec2 InputHandler::getLeftStickLast(int index)
{
	return glm::vec2(m_leftSticksLast[index].xAxis, m_leftSticksLast[index].yAxis);
}

glm::vec2 InputHandler::getRightStickLast(int index)
{
	return glm::vec2(m_rightSticksLast[index].xAxis, m_rightSticksLast[index].yAxis);
}
