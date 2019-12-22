#include "Window.h"
#include <sstream>
#include "resource.h"

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

Window::Window(int a_width, int a_height, const char* a_name)
	:
Kbd(m_keyboard),
m_name(a_name),
m_width(a_width),
m_height(a_height)
{
	RECT rect;
	rect.left = 100;
	rect.right = a_width + rect.left;
	rect.top = 100;
	rect.bottom = a_height + rect.top;

	if(FAILED(AdjustWindowRect(&rect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE))) // Ensures Client Area is correct width/height
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

LRESULT CALLBACK Window::HandleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	OutputDebugString(s_MessageMap(msg, lParam, wParam).c_str());

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
	OutputDebugString(s_MessageMap(msg, lParam, wParam).c_str());

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
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_MOUSEMOVE:
		{
			
		}
	case WM_KILLFOCUS: // Clear Keyboard State on Kill Focus
		{
			m_keyboard.ClearState();
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
