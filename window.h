#pragma once
#include<Windows.h>

#define WINDOW true
#define NO_WINDOW false
#define width clientWidthF()
#define height clientHeightF()
#define delta deltaTime()
#define mouseWheel getMouseWheel()

void createWindow(LPCSTR windowTitle, int clientWidth, int clientHeight,
	bool windowed, int clientPosX, int clientPosY);
void showWindow();
bool quit();
void closeWindow();
//Žæ“¾Œn
HWND hwnd();
int msg_wparam();
int clientWidth();
int clientHeight();
float clientWidthF();
float clientHeightF();
float aspect();
int getMouseWheel();
