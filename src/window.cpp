#include "window.h"
#include "d3dinit.h"

D3DBase* D3DWindow::pD3D = nullptr;

// Method to secure that there is only one window instance
D3DWindow::D3DWindow(WINDOW_PARAMS& wndParam)
{
	LPCWSTR className = L"phys-sim-window";

	// Fill out structure with window class data
	WNDCLASS wc = {};
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = wndParam.hInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = className;

	// Try to create class
	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass FAILED", 0, 0);
	}

	// Registered class is ready to use. Create window and strore HWND
	mhWnd = CreateWindow(
		className,
		wndParam.windowTitle,
		WS_OVERLAPPEDWINDOW,
		wndParam.x,
		wndParam.y,
		wndParam.width,
		wndParam.height,
		0,
		0,
		wndParam.hInstance,
		0);

	// Check if window creation failed
	if (mhWnd == 0)
	{
		MessageBox(0, L"Create Window FAILED", 0, 0);
	}
}

// Returns HWND for an object
HWND D3DWindow::GetWindowHandle() const { return mhWnd; }

// Show the window and start the message loop
void D3DWindow::ShowD3DWindow(int show, D3DBase* pRenderer)
{
	pD3D = pRenderer;

	if (!pD3D)
	{
		MessageBox(0, L"Invalid DirectX 12 instance.", 0, 0);
		return;
	}

	// Finally, when window is created show it. After this point
	// the message loop in WndProc will begin, and we need a
	// working and non-empty instance of D3DBase.
	// Calling it here guarantees that.
	ShowWindow(mhWnd, show);
	UpdateWindow(mhWnd);
}

// Process window messages, passes arguments to d3d_base static class
LRESULT CALLBACK D3DWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return pD3D->MsgProc(hWnd, msg, wParam, lParam);
}