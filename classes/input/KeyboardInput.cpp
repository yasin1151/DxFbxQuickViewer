#include "KeyboardInput.h"

#include "log/easylogging++.h"

bool KeyboardInput::KeyIsPressed(unsigned char inputCode)
{
	return m_KeyStates[inputCode];
}

void KeyboardInput::OnKeyPressed(unsigned char inputCode)
{
	// LOG(INFO) << "OnKeyPressed :" << int(inputCode);
	m_KeyStates[inputCode] = true;
}

void KeyboardInput::OnKeyReleased(unsigned char inputCode)
{
	// LOG(INFO) << "OnKeyReleased :" << int(inputCode);
	m_KeyStates[inputCode] = false;
}

void KeyboardInput::OnChar(unsigned char character)
{
	// LOG(INFO) << "OnChar :" << int(character);
}

void KeyboardInput::OnKillFocus()
{
	m_KeyStates.reset();

}
