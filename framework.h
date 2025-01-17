#pragma once
#include"window.h"
#include"graphic.h"
#include"input.h"
#include"timer.h"

#define win true
#define full false

void window(LPCSTR windowTitle, float baseWidth, float baseHeight,
	bool windowed = true, int numDescriptors = 2000, bool centered=true);
