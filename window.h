#pragma once
#include<Windows.h>

#define WINDOW true
#define NO_WINDOW false
#define width baseWidth()
#define height baseHeight()
#define mouseWheel getMouseWheel()

void createWindow(LPCSTR windowTitle, float clientWidth, float clientHeight,
	bool windowed, bool centered);
void showWindow();
bool quit();
void closeWindow();
//Žæ“¾Œn
HWND hwnd();
int msg_wparam();
float baseWidth();
float baseHeight();
float clientWidth();
float clientHeight();
bool centered();
int getMouseWheel();
