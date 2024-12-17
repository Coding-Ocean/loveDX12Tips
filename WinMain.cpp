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

		stroke(0, 0, 0);
		strokeWeight(3);
		fill(1, 1, 1,0);
		rectModeCorner();
		for (int y = 0; y < 3; ++y) {
			for (int x = 0; x < 3; ++x) {
				//if ((x + y) % 2) {
				//	fill(0.f, 0.4f, 0.f);
				//}
				//else {
				//	fill(0.f, 0.f, 0.f);
				//}
				rect(200.0f * x, 200.0f * y, 200, 200, rad);
			}
		}

		stroke(1, 0, 0);
		line(0, 0, 1280, cosf(rad)*720);
		line(0, 720, 1280, 0);

		fill(1, 0.3f, 0.7f);
		image(img, 0, 0, rad * 0,cosf(rad),cosf(rad));
		fill(0, 0.7f, 0);
		image("assets/penguin1.png", 900, 360, rad * 2);
		fill(0, 0.7f, 1);
		image("assets/penguin1.png", 900 + 100, 360, rad * 2);


		fontSize(200); fill(0.2f, 0.2f, 0.2f);
		text("F‘¦¥‹ó", (1280 - 200 * 4) / 2, (720 - 200) );
		fontSize(40);  fill(0, 1, 1);
		print("numTextures:%u", numLoadTextures());
		print("numConstants:%u", numConstants());
		print("rad:%.2f", rad);

		endRender();


	}

	waitGPU();
	closeEventHandle();
}
