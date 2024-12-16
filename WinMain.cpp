#include"graphic.h"

INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"2DGraphics", 1280, 720);

	int img = loadImage("assets/image.png");

	float rad = 0;
	
	clearColor(1, 0.8f, 0);

	while (!quit())
	{
		rad += 0.01f;

		beginRender();

		for (int y = 0; y < 5; ++y) {
			for (int x = 0; x < 5; ++x) {
				if ((x + y) % 2) {
					fill(0.f, 0.4f, 0.f);
				}
				else {
					fill(0.f, 0.f, 0.f);
				}
				rect(100.0f * x + 50, 100.0f * y + 50, 100, 100, rad);
			}
		}

		fill(1, 0.3f, 0.7f);
		image(img, 900 - 100, 256, rad * 2, 0.4f, 0.4f);
		fill(0, 0.7f, 0);
		image("assets/penguin1.png", 900, 256, rad * 2, 0.3f, 0.3f);
		fill(0, 0.7f, 1);
		image("assets/penguin1.png", 900 + 100, 256, rad * 2, 0.4f, 0.4f);

		fontSize(200);
		fill(0, 0, 0);
		text("F‘¦¥‹ó", (1280 - 200 * 4) / 2, (720 - 200) );
		fontSize(40);
		fill(0, 1, 1);
		print("numTextures:%d", numLoadTextures());
		print("rad:%.2f", rad);

		endRender();
	}

	waitGPU();
	closeEventHandle();
}
