#include<windows.h>
#include"timer.h"

unsigned int PreTime = 0;
float DeltaTime = 0;
float ElapsedTime[10] = {};

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
bool timer(int number, float interval)
{
	ElapsedTime[number] += DeltaTime;
	if (ElapsedTime[number] >= interval) {
		ElapsedTime[number] -= interval;
		return true;
	}
	return false;
}
void resetTimer(int number)
{
	ElapsedTime[number] = 0;
}
float deltaTime()
{
	return DeltaTime;
}