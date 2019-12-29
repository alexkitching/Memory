#pragma once
#include <utility>
#include <queue>

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
			Enter,
			Leave
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
		m_bLeftPressed(a_parent.m_bLeftPressed),
		m_bMiddlePressed(a_parent.m_bMiddlePressed),
		m_bRightPressed(a_parent.m_bRightPressed),
		m_x(a_parent.m_X),
		m_y(a_parent.m_Y)
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

	// State Info
	std::pair<int, int> Position() const { return { m_X, m_Y }; }
	int GetXPos() const { return m_X; }
	int GetYPos() const { return m_Y; }
	bool LeftButtonPressed() const { return m_bLeftPressed; }
	bool MiddleButtonPressed() const { return m_bMiddlePressed; }
	bool RightButtonPressed() const { return m_bRightPressed; }
	bool IsInWindow() const { return m_bIsInWindow;  }

	// Event System
	Mouse::Event ReadEvent();
	bool IsEmpty() const { return m_EventBuffer.empty();  }
	void Flush();

private:
	void OnMouseMove(int a_posX, int a_posY);
	void OnLeftPressed(int a_posX, int a_posY);
	void OnLeftReleased(int a_posX, int a_posY);
	void OnMiddlePressed(int a_posX, int a_posY);
	void OnMiddleReleased(int a_posX, int a_posY);
	void OnRightPressed(int a_posX, int a_posY);
	void OnRightReleased(int a_posX, int a_posY);
	void OnWheelUp(int a_posX, int a_posY);
	void OnWheelDown(int a_posX, int a_posY);
	void OnWheelDelta(int a_posX, int a_posY, int a_delta);
	void OnMouseEnter();
	void OnMouseLeave();

	void TrimBuffer();

	static constexpr unsigned int s_kBuffSize = 16u;
	int m_X;
	int m_Y;
	int m_WheelDeltaAccum;
	bool m_bLeftPressed;
	bool m_bMiddlePressed;
	bool m_bRightPressed;
	bool m_bIsInWindow;

	std::queue<Event> m_EventBuffer;
};
