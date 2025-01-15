#pragma once

enum INPUT_CODE {
	KEY_A, KEY_D, KEY_W, KEY_S,
	KEY_ESC,
	MOUSE_LBUTTON,MOUSE_MBUTTON,MOUSE_RBUTTON,
	MOUSE_X, MOUSE_Y,
	NUM_CODE
};

void getInputState();
int isPress(INPUT_CODE i);
int isTrigger(INPUT_CODE i);
int isRelease(INPUT_CODE i);

void hideCursor();
void showCursor();
float getMouseX();
float getMouseY();
float getMouseVx();
float getMouseVy();
void setMousePos(float x, float y);

void createInput();
void destroyInput();

#define mouseX getMouseX()
#define mouseY getMouseY()


