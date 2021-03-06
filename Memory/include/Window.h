#pragma once

#include "Win.h"

#include "Exception.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "D3DRenderer.h"
#include "IMGUIInterface.h"
#include <memory>
#include <optional>

//------------
// Description
//--------------
// MS Window Class containing primary window callback functions, dimensions and mouse/keyboard input via recieved windows messages.
//------------

class Window
{
private:
	// Static Class Definition
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

public:
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

	class MissingRendererException : public Exception
	{
	public:
		MissingRendererException(int a_line, const char* a_file) : Exception(a_line, a_file) {}
		const char* GetType() const override;
	};
	
	Window(int a_width, int a_height, const char* a_name);
	~Window();
	
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	
	bool Initialise();

	// Mouse Keyboard Public References
	Keyboard& GetKeyboard;
	Mouse& GetMouse;
	
	D3DRenderer& GetRenderer() const;
	IMGUIInterface& GetIMGUI();
	
	std::string GetName() const { return m_name; }
	HWND GetHandle() const {return m_hWnd;}

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

	void SetTitle(const std::string a_titleStr) const;
	static std::optional<int> ProcessMessages(); // Processes Messages, Returns exit code on recieving quit message
private:
	static LRESULT CALLBACK HandleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMessageInternal(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	std::string m_name;
	int m_width;
	int m_height;
	HWND m_hWnd;
	Keyboard m_keyboard;
	Mouse m_mouse;
	std::unique_ptr<D3DRenderer> m_pRenderer;
	IMGUIInterface m_IMGUIInterface;
};

#define WND_Except(hr) Window::WindowException(__LINE__, __FILE__, hr)
#define WND_Last_Except() Window::WindowException(__LINE__, __FILE__, GetLastError() )
#define WND_Missing_Renderer_Except() Window::MissingRendererException(__LINE__, __FILE__)