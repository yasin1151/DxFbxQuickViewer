#include "KeyboardInputEvent.h"

KeyboardInputEvent::KeyboardInputEvent(KeyboardPressState pressState, unsigned char inputCode)
	: m_PressState(pressState), m_InputCode(inputCode)
{
}

bool KeyboardInputEvent::IsPress() const
{
	return m_PressState == KeyboardPressState::Press;
}

bool KeyboardInputEvent::IsRelease() const
{
	return m_PressState == KeyboardPressState::Release;
}

KeyboardPressState KeyboardInputEvent::GetPressState() const
{
	return m_PressState;
}

unsigned char KeyboardInputEvent::GetInputCode() const
{
	return m_InputCode;
}

