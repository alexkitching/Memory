#pragma once
#include "WindowsKeyCode.h"
#include <queue>
#include <bitset>

class Keyboard
{
	friend class Window;
public:
	struct Event
	{
		enum class Type
		{
			Invalid = -1,
			Press = 0,
			Release,
		};

		Event()
			:
		m_Type(Type::Invalid),
		m_Code(0u)
		{
			
		}

		Event(Type a_type, unsigned char a_char)
			:
		m_Type(a_type),
		m_Code(a_char)
		{
			
		}
		~Event() {}

		bool IsPress() const { return m_Type == Type::Press; }
		bool IsRelease() const { return m_Type == Type::Release; }
		bool IsValid() const { return m_Type != Type::Invalid; }

		unsigned char GetCode() const { return m_Code; }

	private:
		Type m_Type;
		unsigned char m_Code;
	};

	Keyboard() : m_bAutoRepeatEnabled(false) {}
	~Keyboard() {}
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;

	// Key Event
	bool KeyPressed(WinKeyCode a_key) const;
	Event ReadKey();
	bool KeyEmpty() const;
	void ClearKey();

	// Char Event
	char ReadChar();
	bool CharEmpty() const;
	void ClearChar();
	void Flush();

	void EnableAutoRepeat() { m_bAutoRepeatEnabled = true; }
	void DisableAutoRepeat() { m_bAutoRepeatEnabled = false; }
	bool AutoRepeatEnabled() const { return m_bAutoRepeatEnabled; }

	static const unsigned int s_kNumKeys = 256u;

private:
	void OnKeyPressed(unsigned char a_keyCode);
	void OnKeyReleased(unsigned char a_keyCode);
	void OnChar(unsigned char a_char);
	void ClearState();
	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer);

	static const unsigned int s_kBufSize = 16u;
	bool m_bAutoRepeatEnabled;
	std::bitset<s_kNumKeys> m_keyStates;
	std::queue<Event> m_keyEventBuffer;
	std::queue<unsigned char> m_charBuffer;
};

template<typename T>
inline void Keyboard::TrimBuffer(std::queue<T>& buffer)
{
	while(buffer.size() > s_kBufSize)
	{
		buffer.pop();
	}
}
