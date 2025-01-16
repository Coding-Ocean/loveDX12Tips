#include"framework.h"

void window(LPCSTR windowTitle, int clientWidth, int clientHeight, 
	bool windowed, int numDescriptors, int clientPosX, int clientPosY)
{
	createWindow(windowTitle, clientWidth, clientHeight, windowed, clientPosX, clientPosY);
	createGraphic(clientWidth, clientHeight, windowed, numDescriptors);
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
