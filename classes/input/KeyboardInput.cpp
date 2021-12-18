#include "KeyboardInput.h"

#include "log/easylogging++.h"

void KeyboardInput::OnKeyPressed(unsigned char inputCode)
{
	LOG(INFO) << "OnKeyPressed :" << int(inputCode);
}

void KeyboardInput::OnKeyReleased(unsigned char inputCode)
{
	LOG(INFO) << "OnKeyReleased :" << int(inputCode);
}
