#include"graphic.h"

INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"2DGraphics", 1920, 1200, NO_WINDOW);

	//int img = loadImage("assets/image.png");

	float rad = 0;
	
	clearColor(0.9f, 0.9f, 0.9f);

	float ox = width / 2;
	float oy = height / 2;
	float diameter = 900;
	float len = diameter / 1.414f;
	float radius = len / 2;

	while (!quit())
	{
		float ratio = (cosf(rad) + 1.0f) * 0.5f;
		rad += 0.005f;
		beginRender();

		//stroke(0, 0, 0);
		//strokeWeight(90);
		//fill(1, 1, 1);
		//circle(ox, oy, diameter);
		//rect(ox, oy, len, len, rad);
		//line(ox, oy, ox + sinf(rad) * radius, oy - cosf(rad) * radius);
		//image("assets/penguin1.png", ox, oy, -rad, diameter / height, diameter / height);

		stroke(0,0,0);
		strokeWeight(3);
		int nx = 5;
		int ny = 5;
		for (int y = 0; y < nx; ++y) {
			for (int x = 0; x < ny; ++x) {
				if ((x + y) % 2) {
					fill(1.f, 1.f, 1.f,0);
				}
				else {
					fill(1.f, 1.f, 1.f,0);
				}
				float w = 100.0f;
				float h = 100.0f;
				float ofstX = (width - w * nx) / 2 + w/2;
				float ofstY = (height - h * ny) / 2 + h/2;
				float px = ofstX + w * x;
				float py = ofstY +h * y;
				rect(px, py, w, h, rad);
				fill(1, 0, 0);
				circle(px, py, w*0.2f);
			}
		}

		fill(1,1,1);
		image("assets/penguin1.png", width - 100, height / 2, rad * 2, 0.3f, 0.3f);
		image("assets/penguin1.png", width-200, height/2, rad * 2,0.3f,0.3f);

		int size = 60;
		fontSize(size);
		std::string str = "色即是空。空即是色。";//←漢字にしてください
		fill(0.f, 0.f, 0.f);
		text(str.c_str(), (width - size*0.5f * str.size()) / 2, (height - size));
		
		fontSize(40);
		fill(0, 0, 0);
		print("numLoadTextures:%u", numLoadTextures());
		print("numFontTextures:%u", numFontTextures());
		print("numConstants:%u", numConstants());
		print("rad:%.2f", rad);

		endRender();
	}

	waitGPU();
	closeEventHandle();
}
