#include"graphic.h"

void gmain();

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ INT)
{
	timeBeginPeriod(0);

	gmain();

	timeEndPeriod(0);
	waitGPU();
	closeEventHandle();
	return msg_wparam();
}
