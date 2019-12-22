#include "Win.h"
#include "WindowsMessageMap.h"
#include "WindowsKeyCode.h"
#include "Window.h"
#include "Exception.h"

void ProcessKeyMessage(HWND hWnd, WinKeyCode a_key, bool bUpDown)
{
}

void ProcessCharMsg(HWND hWnd, char a_char)
{
	static std::string title;
	title.push_back(a_char);
	SetWindowText(hWnd, title.c_str());
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static WindowsMessageMap mm;
	OutputDebugString(mm(msg, lParam, wParam).c_str());

	struct Position
	{
		short X;
		short Y;
	};

	Position* pos;
	std::string posText;

	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(69);
		break;
	case WM_KEYDOWN:
		ProcessKeyMessage(hWnd, (WinKeyCode)wParam, false);
		break;
	case WM_KEYUP:
		ProcessKeyMessage(hWnd, (WinKeyCode)wParam, true);
		break;
	case WM_CHAR:
		ProcessCharMsg(hWnd, (char)wParam);
		break;
	case WM_LBUTTONDOWN:
		pos = (Position*)&lParam;
		POINTS points = MAKEPOINTS(lParam);
		posText.append("X:");
		posText.append(std::to_string(pos->X));
		posText.append(" Y:");
		posText.append(std::to_string(pos->Y));

		SetWindowText(hWnd, posText.c_str());
		break;
	default: 
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInstance, LPSTR lpCmdLine, int nCmdShow)
{
	try
	{
		Window MainWindow(640,480, "Alex's Window");
		Window SubWindow(100, 100, "SubWindow");

		// Message Pump
		MSG msg;
		BOOL gResult;
		while((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
		{
			TranslateMessage(&msg); // Generate WM_Char Messages
			DispatchMessage(&msg);

			if(MainWindow.Kbd.KeyPressed(Alt))
			{
				MessageBox(nullptr, "You pressed space!", "Well done!", MB_OK);
			}
		}

		if(gResult == -1) // Error Code
		{
			return -1;
		}

		// Return exit code
		return msg.wParam;	
	}
	catch(const Exception& e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch(const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "std::Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch(...)
	{
		MessageBox(nullptr, "No Details Available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION );
	}
	return 0;
}