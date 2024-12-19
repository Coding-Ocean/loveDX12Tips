#include"graphic.h"

INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"2DGraphics", 1600, 900);
	
	clearColor(0.9f, 0.9f, 0.9f);

	float rad = 0;
	int cnt = 0;
	int n = 5;
	int inc = 1;

	while (!quit())
	{
		//更新------------------------------------------------
		//float ratio = cosf(rad) * 0.5f + 0.5f;
		rad += 0.005f;

		//描画------------------------------------------------
		beginRender();

		//四角と丸
		if (++cnt % 60 == 0) {
			if (n < 1 || n > 4) {
				inc *= -1; 
			}
			n += inc;
		}
		stroke(0, 0, 0);
		strokeWeight(3);
		for (int y = 0; y < n; ++y) {
			for (int x = 0; x < n; ++x) {
				if ((x + y) % 2) {
					fill(1.f, 1.f, 1.f,0);
				}
				else {
					fill(1.f, 1.f, 1.f,0);
				}
				float w = 100.0f;
				float h = 100.0f;
				float ofstX = (width - w * n) / 2 + w/2;
				float ofstY = (height - h * n) / 2 + h/2;
				float px = ofstX + w * x;
				float py = ofstY +h * y;
				rect(px, py, w, h, rad);
				fill(1, 1, 0);
				circle(px, py, w*0.3f);
			}
		}

		//image ファイル名重複しても大丈夫です
		fill(1, 0, 0);
		for (int i = 0; i < n; i++) {
			image("assets/penguin1.png", width - 500 + 100*i, height / 2, rad * 2, 0.3f, 0.3f);
		}

		//arrow
		float ox = width / 6;
		float oy = height / 2;
		float ex = ox + cosf(rad) * 200;
		float ey = oy - sinf(rad) * 200;
		stroke(0, 0, 1);
		strokeWeight(5);
		arrow(ox, oy, ex, ey);
		fontRectModeCenter();
		fontSize(50);
		fill(0, 0, 0);
		text("ａ", ex + cosf(rad) * 22, ey - sinf(rad) * 22);

		//text 漢字
		int size = 100;
		fontSize(size);
		std::string str = "色即是空";//←漢字にしてください
		fill(0.f, 0.f, 0.f);
		fontRectModeCorner();
		text(str.c_str(), (width - size*0.5f * str.size()) / 2, (height - size));

		//print
		fontSize(40);
		print("numLoadTextures:%u", numLoadTextures());
		print("numFontTextures:%u", numFontTextures());
		print("numConstants:%u", numConstants());
		print("n:%d", n);
		print("dn:%d", inc);

		endRender();
	}

	waitGPU();
	closeEventHandle();
}
