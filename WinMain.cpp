#include"graphic.h"
#include"input.h"

void gmain();

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ INT)
{
	timeBeginPeriod(1);

	gmain();

	timeEndPeriod(1);
	waitGPU();
	closeEventHandle();
	destroyInput();
	return msg_wparam();
}
