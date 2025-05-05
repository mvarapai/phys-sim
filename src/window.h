#pragma once

#include <Windows.h>
#include <corecrt_wstring.h>

#include "d3dinit.h"


struct WINDOW_PARAMS					// Parameter structure to pass at creation
{
	HINSTANCE hInstance;
	LPCWSTR windowTitle;
	int x;
	int y;
	int width;
	int height;
};

struct D3DWindow {						// Class that initializes and operates the window

private:
	HWND mhWnd = nullptr;				// Window handle. Make it contain pointer to this class

	static D3DBase* pD3D;
public:
	D3DWindow() = delete;
	D3DWindow(D3DWindow& other) = delete;
	D3DWindow& operator=(D3DWindow& rhs) = delete;

	D3DWindow(WINDOW_PARAMS& wndParam);	// Create the window by passing parameters
	HWND GetWindowHandle() const;		// Access HWND via this class

	void ShowD3DWindow(int show, D3DBase* pApp);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

