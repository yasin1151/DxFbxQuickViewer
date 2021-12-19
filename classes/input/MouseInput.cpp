#include "MouseInput.h"


std::pair<int, int> MouseInput::GetPos()
{
	return { m_PosX, m_PosY }; 
}
