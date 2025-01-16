#include"toWide.h"
#include"window.h"

static LPCSTR WindowTitle;
static int   ClientWidth;
static int   ClientHeight;
static int    ClientPosX;
static int    ClientPosY;
static float  Aspect;
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
void createWindow(LPCSTR windowTitle, int clientWidth, int clientHeight, 
	bool windowed, int clientPosX, int clientPosY)
{
	//グローバル変数初期化
	WindowTitle = windowTitle;

	if (windowed) {
		ClientWidth = clientWidth;
		ClientHeight = clientHeight;
		Aspect = (float)ClientWidth / ClientHeight;
		WindowStyle = WS_OVERLAPPEDWINDOW;
	}
	else {
		ClientWidth = GetSystemMetrics(SM_CXSCREEN);
		ClientHeight = GetSystemMetrics(SM_CYSCREEN);
		Aspect = (float)ClientWidth / ClientHeight;
		WindowStyle = WS_POPUP;
	}

	ClientPosX = (GetSystemMetrics(SM_CXSCREEN) - ClientWidth) / 2;//中央表示
	if (clientPosX >= 0)ClientPosX = clientPosX;
	ClientPosY = (GetSystemMetrics(SM_CYSCREEN) - ClientHeight) / 2;//中央表示
	if (clientPosY >= 0)ClientPosY = clientPosY;
	
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
	RECT windowRect = { 0, 0, ClientWidth, ClientHeight };
	AdjustWindowRect(&windowRect, WindowStyle, FALSE);
	int windowPosX = ClientPosX + windowRect.left;
	int windowPosY = ClientPosY + windowRect.top;
	int windowWidth = windowRect.right - windowRect.left;
	int windowHeight = windowRect.bottom - windowRect.top;
	//ウィンドウをつくる
	HWnd = CreateWindowEx(
		NULL,
		L"GAME_WINDOW",
		toWide(WindowTitle),
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
int clientWidth() 
{
	return ClientWidth;
}
int clientHeight() 
{
	return ClientHeight;
}
float clientWidthF()
{
	return (float)ClientWidth;
}
float clientHeightF()
{
	return (float)ClientHeight;
}
float aspect() 
{
	return Aspect;
}
int getMouseWheel()
{
	return MouseWheel;
}
