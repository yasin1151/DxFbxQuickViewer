#pragma once
#include <utility>

enum class MouseInputEventType
{
	L_PRESS,
	L_RELEASE,
	R_PRESS,
	R_RELEASE,
	WHEEL_UP,
	WHEEL_DOWN,
	MOVE,
	ENTER,
	LEAVE,
};

class MouseInputEvent
{
public:

	MouseInputEvent(MouseInputEventType eventType, std::pair<int, int>& Pos);

	MouseInputEventType GetType() const;

	std::pair<int, int> GetPos();

	int GetPosX();

	int GetPosY();

private:
	MouseInputEventType m_EventType;
	int m_PosX;
	int m_PosY;

};
