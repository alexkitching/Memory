#pragma once
#include <utility>

class Mouse
{
	friend class Window;
public:
	class Event
	{
	public:
		enum class Type
		{
			Invalid = -1,
			LBtnPress = 0,
			LBtnRelease,
			MBtnPress,
			MBtnRelease,
			RBtnPress,
			RBtnRelease,
			WheelUp,
			WheelDown,
			Move,
		};

		Event()
			:
		m_type(Type::Invalid),
		m_bLeftPressed(false),
		m_bMiddlePressed(false),
		m_bRightPressed(false),
		m_x(0),
		m_y(0)
		{
			
		}

		Event(Type a_type, const Mouse& a_parent)
			:
		m_type(a_type),
		m_bLeftPressed(a_parent.leftIsPressed),
		m_bMiddlePressed(a_parent.middleIsPressed),
		m_bRightPressed(a_parent.rightIsPressed),
		m_x(a_parent.x),
		m_y(a_parent.y)
		{
			
		}

		bool IsValid() const { return m_type != Type::Invalid; }
		Type GetType() const { return m_type; }

		std::pair<int, int> GetPosition() const { return std::make_pair(m_x, m_y); }
		int GetX() const { return m_x; }
		int GetY() const { return m_y; }

		bool LeftBtnPressed() const { return m_bLeftPressed; }
		bool MiddleBtnPressed() const { return m_bMiddlePressed; }
		bool RightBtnPressed() const { return m_bRightPressed; }
	private:
		          Type m_type;
		bool m_bLeftPressed;
		bool m_bMiddlePressed;
		bool m_bRightPressed;
		int m_x;
		int m_y;
	};

	Mouse();
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;

	std::pair<int,int> Position() const { }
};
