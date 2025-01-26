#pragma once
#define delta deltaTime()

void initDeltaTime();
void setDeltaTime();
float deltaTime();
bool timer(int number, float interval);
void resetTimer(int number);