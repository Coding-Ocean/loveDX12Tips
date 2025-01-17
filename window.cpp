#include"toWide.h"
#include"window.h"

static float  BaseWidth;
static float  BaseHeight;
static float  ClientWidth;
static float  ClientHeight;
static bool   Centered;//中央表示(プレゼン用。たぶん僕しか使わない)
static DWORD  WindowStyle;
static HWND   HWnd;
static MSG    Msg;
static int    MouseWheel;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_MOUSEWHEEL:
		MouseWheel = GET_WHEEL_DELTA_WPARAM(wp) / WHEEL_DELTA;
		return 0;
	default:
		return DefWindowProc(hwnd, msg, wp, lp);
	}
}
void createWindow(LPCSTR windowTitle, float baseWidth, float baseHeight, bool windowed, bool centered)
{
	//グローバル変数設定
	BaseWidth = baseWidth;
	BaseHeight = baseHeight;
	if (windowed) {
		ClientWidth = baseWidth;
		ClientHeight = baseHeight;
		WindowStyle = WS_OVERLAPPEDWINDOW;
	}
	else {
		ClientWidth = (float)GetSystemMetrics(SM_CXSCREEN);
		ClientHeight = (float)GetSystemMetrics(SM_CYSCREEN);
		WindowStyle = WS_POPUP;
	}
	Centered = centered;

	//ウィンドウクラス登録
	WNDCLASSEX windowClass = {};
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WndProc;
	windowClass.hInstance = GetModuleHandle(0);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	windowClass.lpszClassName = L"GAME_WINDOW";
	RegisterClassEx(&windowClass);
	//表示位置、ウィンドウの大きさ調整
	RECT windowRect = { 0, 0, (LONG)ClientWidth, (LONG)ClientHeight };
	AdjustWindowRect(&windowRect, WindowStyle, FALSE);
	int clientPosX = (GetSystemMetrics(SM_CXSCREEN) - (int)ClientWidth) / 2;//中央表示
	int clientPosY = (GetSystemMetrics(SM_CYSCREEN) - (int)ClientHeight) / 2;//中央表示
	int windowPosX = clientPosX + windowRect.left;
	int windowPosY = clientPosY + windowRect.top;
	int windowWidth = windowRect.right - windowRect.left;
	int windowHeight = windowRect.bottom - windowRect.top;
	//ウィンドウをつくる
	HWnd = CreateWindowEx(
		NULL,
		L"GAME_WINDOW",
		toWide(windowTitle),
		WindowStyle,
		windowPosX,
		windowPosY,
		windowWidth,
		windowHeight,
		NULL,		//親ウィンドウなし
		NULL,		//メニューなし
		GetModuleHandle(0),
		NULL);		//複数ウィンドウなし
}
void showWindow()
{
	ShowWindow(HWnd, SW_SHOW);
}
bool quit()
{
	MouseWheel = 0;//ここで初期化したくないのだが、仕方ない

	while (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)) {
		if (Msg.message == WM_QUIT)return true;
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	return false;
}
//ウィンドウを閉じるメッセージを出す
void closeWindow()
{
	PostMessage(HWnd, WM_CLOSE, 0, 0);
}
//取得系
HWND hwnd() 
{
	return HWnd;
}
int msg_wparam()
{
	return (int)Msg.wParam;
}
float clientWidth() 
{
	return ClientWidth;
}
float clientHeight() 
{
	return ClientHeight;
}
float baseWidth()
{
	return BaseWidth;
}
float baseHeight()
{
	return BaseHeight;
}
bool centered()
{ 
	return Centered; 
}
int getMouseWheel()
{
	return MouseWheel;
}
