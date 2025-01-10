#include"graphic.h"

int GameMain()
{
	window("2DGraphics", 900, 900);
	clearColor(0.8f, 0.8f, 0.8f);

	float rad = 0;

	while (!quit())
	{
		rad += 0.01f;

		beginMsaaRender();
		//beginRender();

		strokeWeight(3);
		fill(1, 1, 1);
		rect(width/2, height/2, 300,300,sinf(rad)*0.3f);
		
		fill(1, 0, 0);
		circle(width/2, height/2, rad*10);

		endMsaaRender();
		//endRender();
	}

	waitGPU();
	closeEventHandle();
	return msg_wparam();
}
