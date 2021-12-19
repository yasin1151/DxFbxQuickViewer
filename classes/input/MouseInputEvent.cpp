#include "MouseInputEvent.h"

MouseInputEvent::MouseInputEvent(MouseInputEventType eventType, std::pair<int, int>& Pos)
	:m_EventType(eventType),
	m_PosX(Pos.first),
	m_PosY(Pos.second)
{
	
}

MouseInputEventType MouseInputEvent::GetType() const
{
	return m_EventType;
}

std::pair<int, int> MouseInputEvent::GetPos()
{
	return { m_PosX, m_PosY };
}

int MouseInputEvent::GetPosX()
{
	return m_PosX;
}

int MouseInputEvent::GetPosY()
{
	return m_PosY;
}
