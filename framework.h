#pragma once
#include"window.h"
#include"graphic.h"
#include"input.h"
#include"timer.h"

void window(LPCSTR windowTitle, int clientWidth, int clientHeight,
	bool windowed=true, int numDescriptors=2000, int clientPosX=-1, int clientPosY=-1);

