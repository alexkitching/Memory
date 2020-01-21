#include "Mouse.h"
#include "Windows.h"

Mouse::Mouse()
:
m_X(0),
m_Y(0),
m_WheelDeltaAccum(0),
m_bLeftPressed(false),
m_bMiddlePressed(false),
m_bRightPressed(false),
m_bIsInWindow(false)
{
}

Mouse::Event Mouse::ReadEvent()
{
	// Read Mouse Event from Buffer
	if(m_EventBuffer.empty() == false)
	{
		const Mouse::Event event = m_EventBuffer.front();
		m_EventBuffer.pop();
		return event;
	}
	
	return Mouse::Event(); // Invalid Event
}

void Mouse::Flush()
{
	m_EventBuffer = std::queue<Mouse::Event>();
}

void Mouse::OnMouseMove(int a_posX, int a_posY)
{
	m_X = a_posX;
	m_Y = a_posY;

	m_EventBuffer.push(Event(Event::Type::Move, *this));
	TrimBuffer();
}

void Mouse::OnLeftPressed(int a_posX, int a_posY)
{
	m_X = a_posX;
	m_Y = a_posY;
	m_bLeftPressed = true;

	m_EventBuffer.push(Event(Event::Type::LBtnPress, *this));
	TrimBuffer();
}

void Mouse::OnLeftReleased(int a_posX, int a_posY)
{
	m_X = a_posX;
	m_Y = a_posY;
	m_bLeftPressed = false;

	m_EventBuffer.push(Event(Event::Type::LBtnRelease, *this));
	TrimBuffer();
}

void Mouse::OnMiddlePressed(int a_posX, int a_posY)
{
	m_X = a_posX;
	m_Y = a_posY;
	m_bMiddlePressed = true;

	m_EventBuffer.push(Event(Event::Type::MBtnPress, *this));
	TrimBuffer();
}

void Mouse::OnMiddleReleased(int a_posX, int a_posY)
{
	m_X = a_posX;
	m_Y = a_posY;
	m_bMiddlePressed = false;

	m_EventBuffer.push(Event(Event::Type::MBtnRelease, *this));
	TrimBuffer();
}

void Mouse::OnRightPressed(int a_posX, int a_posY)
{
	m_X = a_posX;
	m_Y = a_posY;
	m_bRightPressed = true;

	m_EventBuffer.push(Event(Event::Type::RBtnPress, *this));
	TrimBuffer();
}

void Mouse::OnRightReleased(int a_posX, int a_posY)
{
	m_X = a_posX;
	m_Y = a_posY;
	m_bRightPressed = false;

	m_EventBuffer.push(Event(Event::Type::RBtnRelease, *this));
	TrimBuffer();
}

void Mouse::OnWheelUp(int a_posX, int a_posY)
{
	m_X = a_posX;
	m_Y = a_posY;
	m_EventBuffer.push(Event(Event::Type::WheelUp, *this));
	TrimBuffer();
}

void Mouse::OnWheelDown(int a_posX, int a_posY)
{
	m_X = a_posX;
	m_Y = a_posY;
	m_EventBuffer.push(Event(Event::Type::WheelDown, *this));
	TrimBuffer();
}

void Mouse::OnWheelDelta(int a_posX, int a_posY, int a_delta)
{
	m_WheelDeltaAccum += a_delta; // Accumulate Delta

	while(m_WheelDeltaAccum >= WHEEL_DELTA) // Actionable Vertical Scroll
	{
		m_WheelDeltaAccum -= WHEEL_DELTA;
		OnWheelUp(a_posX, a_posY);
	}

	while(m_WheelDeltaAccum <= -WHEEL_DELTA)
	{
		m_WheelDeltaAccum += WHEEL_DELTA;
		OnWheelDown(a_posX, a_posY);
	}
}

void Mouse::OnMouseEnter()
{
	m_bIsInWindow = true;
	m_EventBuffer.push(Event(Event::Type::Enter, *this));
	TrimBuffer();
}

void Mouse::OnMouseLeave()
{
	m_bIsInWindow = false;
	m_EventBuffer.push(Event(Event::Type::Leave, *this));
	TrimBuffer();
}

void Mouse::TrimBuffer()
{
	while(m_EventBuffer.size() > s_kBuffSize)
	{
		m_EventBuffer.pop();
	}
}
