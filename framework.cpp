#include"framework.h"

void window(LPCSTR windowTitle, float baseWidth, float baseHeight, 
	bool windowed, int numDescriptors, bool centered)
{
	createWindow(windowTitle, baseWidth, baseHeight, windowed, centered);
	createGraphic(numDescriptors);
	createInput();
	showWindow();
}

void gmain();

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ INT)
{
	timeBeginPeriod(1);

	gmain();

	waitGPU();
	closeEventHandle();
	destroyInput();
	timeEndPeriod(1);
	return msg_wparam();
}
