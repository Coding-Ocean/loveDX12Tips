#include<windows.h>
#include"timer.h"

unsigned int PreTime = 0;
float DeltaTime = 0;
float ElapsedTime = 0;

void initDeltaTime()
{
	PreTime = timeGetTime();
	DeltaTime = 0;
}
void setDeltaTime()
{
	unsigned int  curTime = timeGetTime();
	DeltaTime = (curTime - PreTime) / 1000.0f;
	PreTime = curTime;
}
bool timer(float interval)
{
	ElapsedTime += DeltaTime;
	if (ElapsedTime >= interval) {
		ElapsedTime -= interval;
		return true;
	}
	return false;
}
float deltaTime()
{
	return DeltaTime;
}