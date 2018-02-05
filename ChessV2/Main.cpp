#include "GameUI.h"
#include <windowsx.h>
#include <Tchar.h>
#include <string>

#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"") 

using namespace std;

HWND mHwnd;
HINSTANCE hinstance;
GameUI* gameUI;
bool isFirstTime=true;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(msg==WM_CLOSE)
	{
		DestroyWindow(hwnd);
	}
	else if(msg==WM_DESTROY)
	{
		PostQuitMessage(0);
	}
	else if(msg==WM_CREATE)
	{
		gameUI->createUI(hwnd, hinstance);
	}
	else if(msg==WM_PAINT)
	{
		gameUI->refreshDisplay();
	}
	else if(msg==WM_COMMAND)
	{
		if(wParam==1)
		{
			gameUI->singlePlayerClicked();
		}
		else if(wParam==2)
		{
			gameUI->multiPlayerClicked();
		}
		else if(wParam==11)
		{
			gameUI->startNewGame();
		}
		else if(wParam==12)
		{
			gameUI->toggleDisplay();
		}
		else if(wParam==14)
		{
			gameUI->noPlayerClicked();
		}
		else if(wParam==15)
		{
			gameUI->abortGame();
		}
	}
	else if(msg==WM_LBUTTONDOWN)
	{
		gameUI->mouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}
	else if(msg==WM_LBUTTONUP)
	{
		gameUI->mouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}
	else if(msg==WM_MOUSEMOVE)
	{
		gameUI->mouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}
	else if(msg==WM_CTLCOLORSTATIC)
	{
		return 0;
	}
	else
	{
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	MSG Msg;
	hinstance = hInstance;
	gameUI = new GameUI();

	//Step 1: Registering the Window Class
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = _T("myWindowClass");
	wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, _T("Window Registration Failed!"), _T("Error!"), MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// Step 2: Creating the Window
	mHwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		_T("myWindowClass"),
		_T("Chess V2"),
		(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZE),
		CW_USEDEFAULT, CW_USEDEFAULT, GameUI::WIDTH+20, GameUI::HEIGHT+42,
		NULL, NULL, hInstance, NULL);

	if (mHwnd == NULL)
	{
		MessageBox(NULL, _T("Window Creation Failed!"), _T("Error!"), MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
	ShowWindow(mHwnd, nCmdShow);
	gameUI->updateHandlers(mHwnd, hinstance);
	// Step 3: The Message Loop
	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	delete gameUI;
	return Msg.wParam;
}
