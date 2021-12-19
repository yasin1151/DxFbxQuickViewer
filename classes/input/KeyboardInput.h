#pragma once
#include <bitset>

class KeyboardInput
{
	friend class Application;
public:
	KeyboardInput() = default;

	bool KeyIsPressed(unsigned char inputCode);

private:
	void OnKeyPressed(unsigned char inputCode);

	void OnKeyReleased(unsigned char inputCode);

	void OnChar(unsigned char character);

	void OnKillFocus();

private:

	static constexpr unsigned int KEY_CODE_SIZE = 256;
	std::bitset<KEY_CODE_SIZE> m_KeyStates;

};
