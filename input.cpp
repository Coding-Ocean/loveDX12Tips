#include "window.h"
#include "input.h"

//直前と現在の「キーやボタンの状態」を記憶する。
short InputState[2][NUM_CODE] = {};
//上の２次元配列を指す添え字（０または１のどちらか）
short Now = 0;
//ウィンドウハンドル。ScreenToClient、ClientToScreenに必要
static HWND HWnd;
//マウスカーソル表示・非表示につかう
int CursorCounter = 0;

//現在の入力状態をセットする
void getInputState()
{
	//直前と現在を指す添え字を切り替える
	Now = 1 - Now;
	//入力状態を記憶しておく
	InputState[Now][KEY_A] = GetAsyncKeyState('A') < 0;
	InputState[Now][KEY_D] = GetAsyncKeyState('D') < 0;
	InputState[Now][KEY_W] = GetAsyncKeyState('W') < 0;
	InputState[Now][KEY_S] = GetAsyncKeyState('S') < 0;
	InputState[Now][KEY_ESC] = GetAsyncKeyState(VK_ESCAPE) < 0;
	InputState[Now][MOUSE_LBUTTON] = GetAsyncKeyState(VK_LBUTTON) < 0;
	InputState[Now][MOUSE_MBUTTON] = GetAsyncKeyState(VK_MBUTTON) < 0;
	InputState[Now][MOUSE_RBUTTON] = GetAsyncKeyState(VK_RBUTTON) < 0;
	//マウス座標を記憶しておく
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(HWnd, &pt);
	short x = static_cast<short>(pt.x * baseWidth() / clientWidth());
	short y = static_cast<short>(pt.y * baseHeight() / clientHeight());
	short w = static_cast<short>(baseWidth());
	short h = static_cast<short>(baseHeight());
	if (x < 0)
		x = 0;
	else if (x > w)
		x = w;
	if (y < 0)
		y = 0;
	else if (y > h)
		y = h;
	InputState[Now][MOUSE_X] = x;
	InputState[Now][MOUSE_Y] = y;
}
//指定したキーやボタンが押されているか
int isPress(INPUT_CODE i)
{
	return InputState[Now][i];
}
//指定したキーやボタンが押された瞬間か
int isTrigger(INPUT_CODE i)
{
	if (InputState[!Now][i] == 0 &&	InputState[Now][i] == 1)
		return 1;
	return 0;
}
//指定したキーやボタンが離された瞬間か
int isRelease(INPUT_CODE i)
{
	if (InputState[!Now][i] == 1 &&	InputState[Now][i] == 0)
		return 1;
	return 0;
}
//マウスカーソルを隠す
void hideCursor()
{
	if (CursorCounter >= 0)
		CursorCounter = ShowCursor(FALSE);
}
//マウスカーソルを表示する
void showCursor()
{
	if (CursorCounter < 0)
		CursorCounter = ShowCursor(TRUE);
}
//マウス座標Xを返す
float getMouseX()
{
	return (float)InputState[Now][MOUSE_X];
}
//マウス座標Yを返す
float getMouseY()
{
	return (float)InputState[Now][MOUSE_Y];
}
//マウス移動ベクトルを返す
float getMouseVx()
{
	return float( -InputState[!Now][MOUSE_X] + InputState[Now][MOUSE_X]);
}
//マウス移動ベクトルを返す
float getMouseVy()
{
	return float(InputState[!Now][MOUSE_Y] - InputState[Now][MOUSE_Y]);
}
//マウス位置をセット
void setMousePos(float x, float y)
{
	InputState[Now][MOUSE_X] = (short)x;
	InputState[Now][MOUSE_Y] = (short)y;

	POINT pt{ (short)x, (short)y };
	ClientToScreen(HWnd, &pt);
	SetCursorPos(pt.x, pt.y);
}

//インプット初期化
void createInput()
{
	HWnd = hwnd();
	hideCursor();
	//ウィンドウ中央にマウスをセット
	//RECT rect;
	//GetClientRect(HWnd, &rect);
	//setMousePos(rect.right / 2.0f, rect.bottom / 2.0f);
}
void destroyInput()
{
	showCursor();
}

