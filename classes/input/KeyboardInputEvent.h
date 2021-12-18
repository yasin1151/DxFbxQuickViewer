#pragma once


enum class KeyboardPressState
{
	Press,
	Release
};

class KeyboardInputEvent
{
public:

	KeyboardInputEvent(KeyboardPressState pressState, unsigned char inputCode);

	bool IsPress() const;

	bool IsRelease() const;

	KeyboardPressState GetPressState() const;

	unsigned char GetInputCode() const;

private:
	KeyboardPressState m_PressState;
	unsigned char m_InputCode;
};