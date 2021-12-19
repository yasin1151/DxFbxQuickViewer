#pragma once

#include "MouseInputEvent.h"

class MouseInput
{
	friend class Application;
public:
	MouseInput() = default;

	std::pair<int, int> GetPos();

private:
	void OnMouseMove(int x, int y);
	void OnMouseLeave();
	void OnMouseEnter();

	void OnLeftPressed(int x, int y);
	void OnLeftReleased(int x, int y);

	void OnRightPressed(int x, int y);
	void OnRightReleased(int x, int y);

	void OnWheelUp(int x, int y);
	void OnWheelDown(int x, int y);

private:
	int m_PosX;
	int m_PosY;
};
