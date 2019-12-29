#pragma once

#include "Win.h"
#include "WindowsMessageMap.h"
#include "Exception.h"
#include "Keyboard.h"
#include "Mouse.h"

class Window
{
private:
	// Class Definition
	class WindowClassDef
	{
	public:
		static const char* Name();
		static HINSTANCE GetInstance();

		WindowClassDef(const WindowClassDef&) = delete;
		WindowClassDef& operator=(const WindowClassDef&) = delete;
	private:
		WindowClassDef();
		~WindowClassDef();
		
		// Definition Name;
		inline static const char* s_classDefName = TEXT(WindowClassDef);
		// Static Instance
		static WindowClassDef s_ClassDef;

		HINSTANCE m_hInstance;
	};

	// Window Exception
	class WindowException : public Exception
	{
	public:
		WindowException(int a_line, const char* a_file, HRESULT a_hr);

		const char* what() const override;

		virtual const char* GetType() const override { return TEXT(WindowException); }
		HRESULT GetErrorCode() const { return m_hresult; }
		std::string GetErrorString() const;
	private:
		HRESULT m_hresult;
	};

public:
	Window(int a_width, int a_height, const char* a_name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	Keyboard& GetKeyboard;
	Mouse& GetMouse;
	std::string GetName() const { return m_name; }

	void SetTitle(const std::string a_titleStr) const;
private:
	static LRESULT CALLBACK HandleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMessageInternal(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	static WindowsMessageMap s_MessageMap;

	std::string m_name;
	int m_width;
	int m_height;
	HWND m_hWnd;
	Keyboard m_keyboard;
	Mouse m_mouse;
};

#define WND_Except(hr) Window::WindowException(__LINE__, __FILE__, hr)
#define WND_Last_Except() Window::WindowException(__LINE__, __FILE__, GetLastError() )