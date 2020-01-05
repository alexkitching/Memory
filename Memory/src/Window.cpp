#include "Window.h"
#include <sstream>
#include "resource.h"
#include "Debug.h"

#define LOG_MESSAGES 0

Window::WindowClassDef Window::WindowClassDef::s_ClassDef;
WindowsMessageMap Window::s_MessageMap;

Window::WindowClassDef::WindowClassDef()
	:
m_hInstance(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc = {0};
	wc.cbSize = sizeof(wc); // Default Size
	wc.style = CS_OWNDC; // Unique Device Context Per Window
	wc.lpfnWndProc = HandleMessageSetup; // Default Windows Handle
	wc.cbClsExtra = 0; // No Extra Data 
	wc.cbWndExtra = 0; // No Extra Data Per Window
	wc.hInstance = m_hInstance;
	wc.hIcon = static_cast<HICON>(LoadImage(m_hInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 32, 32, 0));
	wc.hIconSm = static_cast<HICON>(LoadImage(m_hInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 16, 16, 0));
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = s_classDefName;
	

	RegisterClassEx(&wc);
}

Window::WindowClassDef::~WindowClassDef()
{
	UnregisterClass(s_classDefName, m_hInstance);
}


const char* Window::WindowClassDef::Name()
{
	return s_classDefName;
}

HINSTANCE Window::WindowClassDef::GetInstance()
{
	return s_ClassDef.m_hInstance;
}

const char* Window::MissingRendererException::GetType() const
{
	return "[MissingRendererException] Window has no renderer";
}

Window::Window(int a_width, int a_height, const char* a_name)
	:
GetKeyboard(m_keyboard),
GetMouse(m_mouse),
m_name(a_name),
m_width(a_width),
m_height(a_height)
{
	RECT rect;
	rect.left = 100;
	rect.right = a_width + rect.left;
	rect.top = 100;
	rect.bottom = a_height + rect.top;

	if(AdjustWindowRect(&rect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == false) // Ensures Client Area is correct width/height
	{
		throw WND_Last_Except();
	}
	// Create Window and Get HWND
	m_hWnd = CreateWindow(
							WindowClassDef::Name(), // Class Name
							a_name, // Window Name
							WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, // Style
							CW_USEDEFAULT, CW_USEDEFAULT, // Position
							rect.right - rect.left, rect.bottom - rect.top, // Width / Height
							nullptr, nullptr, WindowClassDef::GetInstance(), this
						);

	if(m_hWnd == nullptr)
	{
		throw WND_Last_Except();
	}

	ShowWindow(m_hWnd, SW_SHOWDEFAULT);

	
}

Window::~Window()
{
	DestroyWindow(m_hWnd);
}

bool Window::Initialise()
{
	// Create Renderer
	m_pRenderer = std::make_unique<D3DRenderer>(m_hWnd);

	m_IMGUIInterface.Initialise(this);

	return m_pRenderer != nullptr;
}

D3DRenderer& Window::GetRenderer()
{
	if(m_pRenderer == nullptr)
	{
		throw WND_Missing_Renderer_Except();
	}
	
	return *m_pRenderer;
}

IMGUIInterface& Window::GetIMGUI()
{
	return m_IMGUIInterface;
}

void Window::SetTitle(const std::string a_titleStr) const
{
	if(SetWindowText(m_hWnd, a_titleStr.c_str()) == false)
	{
		throw WND_Last_Except();
	}
}

std::optional<int> Window::ProcessMessages()
{
	MSG msg;

	while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if(msg.message == WM_QUIT) // Quit Check
		{
			return (int)msg.wParam; // Return Optional int
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	// Return empty optional
	return {};
}

LRESULT CALLBACK Window::HandleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
#if LOG_MESSAGES
	OutputDebugString(s_MessageMap(msg, lParam, wParam).c_str());
#endif

	if(msg == WM_NCCREATE) // Create Message
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);

		// Set WinAPI-managed user data to store ptr to window class
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMessage)); // Set our actual procedure

		return pWnd->HandleMessageInternal(hWnd, msg, wParam, lParam);
	}
	// Default handle other messages
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMessageInternal(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMessageInternal(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
#if LOG_MESSAGES
	OutputDebugString(s_MessageMap(msg, lParam, wParam).c_str());
#endif
	switch (msg)
	{
	case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
		// Keyboard Events Start
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			if(!(lParam & 0x40000000) || m_keyboard.AutoRepeatEnabled()) // Hex Representation of binary bit 30
			{
				m_keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
			}
			break;
		}
	case WM_KEYUP:
	case WM_SYSKEYUP:
		{
			m_keyboard.OnKeyReleased(static_cast<unsigned char>(wParam));
			break;
		}
	case WM_CHAR:
		{
			m_keyboard.OnChar(static_cast<unsigned char>(wParam));
			break;
		}
		// Keyboard Events End
		// Mouse Events Start
	case WM_LBUTTONDOWN:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			m_mouse.OnLeftPressed(pt.x, pt.y);
			break;
		}
	case WM_LBUTTONUP:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			m_mouse.OnLeftReleased(pt.x, pt.y);
			break;
		}
	case WM_MBUTTONDOWN:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			m_mouse.OnMiddlePressed(pt.x, pt.y);
			break;
		}
	case WM_MBUTTONUP:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			m_mouse.OnMiddleReleased(pt.x, pt.y);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			m_mouse.OnRightPressed(pt.x, pt.y);
			break;
		}
		case WM_RBUTTONUP:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			m_mouse.OnRightReleased(pt.x, pt.y);
			break;
		}
	case WM_MOUSEWHEEL:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			const short wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);

			m_mouse.OnWheelDelta(pt.x, pt.y, wheelDelta);
			break;
		}
	case WM_MOUSEMOVE:
		{
			const POINTS pt = MAKEPOINTS(lParam);

			if(pt.x >= 0 && pt.x < m_width &&
				pt.y >= 0 && pt.y < m_height) // Within Client Region
			{
				m_mouse.OnMouseMove(pt.x, pt.y);
				if(m_mouse.IsInWindow() == false)
				{
					SetCapture(m_hWnd);
					m_mouse.OnMouseEnter();
				}
			}
			else // Not In Client Region
			{
				if(wParam & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)) // Button is still pressed
				{
					m_mouse.OnMouseMove(pt.x, pt.y);
				}
				else
				{
					ReleaseCapture();
					m_mouse.OnMouseLeave();
				}
			}
			break;
		}
	case WM_KILLFOCUS: // Clear Keyboard State on Kill Focus
		{
			m_keyboard.ClearState();
			break;
		}
	default:
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

Window::WindowException::WindowException(int a_line, const char* a_file, HRESULT a_hr)
	:
Exception(a_line, a_file),
m_hresult(a_hr)
{
}

const char* Window::WindowException::what() const
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorString() << std::endl
		<< GetOriginalString();

	m_whatBuffer = oss.str();

	return m_whatBuffer.c_str();
}

std::string Window::WindowException::GetErrorString() const
{
	return TranslateHResult(m_hresult);
}
