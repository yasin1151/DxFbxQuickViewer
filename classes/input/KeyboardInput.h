#pragma once


class KeyboardInput
{
	friend class Application;
public:
	KeyboardInput() = default;

private:
	void OnKeyPressed(unsigned char inputCode);

	void OnKeyReleased(unsigned char inputCode);

};
