#include "Keyboard.h"

bool Keyboard::KeyPressed(WinKeyCode a_key) const
{
	return m_keyStates[a_key];
}

Keyboard::Event Keyboard::ReadKey()
{
	if(m_keyEventBuffer.empty() == false)
	{
		const Event e = m_keyEventBuffer.front();
		m_keyEventBuffer.pop();
		return e;
	}

	return Event();
}

bool Keyboard::KeyEmpty() const
{
	return m_keyEventBuffer.empty();
}

void Keyboard::ClearKey()
{
	m_keyEventBuffer = std::queue<Event>();
}

char Keyboard::ReadChar()
{
	if(m_charBuffer.empty() == false)
	{
		const char code = m_charBuffer.front();
		m_charBuffer.pop();
		return code;
	}

	return 0;
}

bool Keyboard::CharEmpty() const
{
	return m_charBuffer.empty();
}

void Keyboard::ClearChar()
{
	m_charBuffer = std::queue<unsigned char>();
}

void Keyboard::Flush()
{
	ClearChar();
	ClearKey();
}


void Keyboard::OnKeyPressed(unsigned char a_keyCode)
{
	m_keyStates[a_keyCode] = true;
	m_keyEventBuffer.push(Event(Event::Type::Press, a_keyCode));
	TrimBuffer(m_keyEventBuffer);
}

void Keyboard::OnKeyReleased(unsigned char a_keyCode)
{
	m_keyStates[a_keyCode] = false;
	m_keyEventBuffer.push(Event(Event::Type::Release, a_keyCode));
	TrimBuffer(m_keyEventBuffer);
}

void Keyboard::OnChar(unsigned char a_char)
{
	m_charBuffer.push(a_char);
	TrimBuffer(m_charBuffer);
}

void Keyboard::ClearState()
{
	m_keyStates.reset();
}
