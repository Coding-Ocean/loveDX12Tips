#include"graphic.h"

void gmain()
{
	//window関数内で「MSAAレンダーターゲット」をつくっている
	window("DeltaTime", 1600, 900);

	float rad = 0;
	float interval = 0;
	int n = 5;
	int inc = 1;
	float elapsed = 0;
	float toRad = 3.1415926f / 180;

	initDeltaTime();
	while (!quit())
	{
		//更新------------------------------------------------
		setDeltaTime();
		//1秒間に回転する角度をラジアンに変換
		rad += 90.0f * toRad * delta;
		//経過時間
		elapsed += delta;
		//指定秒ごとにｎを変更
		if (timer(1.0f)) {
			if (n < 1 || n > 4) {
				inc *= -1;
			}
			n += inc;
		}

		//描画------------------------------------------------
		beginMsaaRender();
		
		//rect, circle
		stroke(0, 0, 0);
		strokeWeight(5);
		for (int y = 0; y < n; ++y) {
			for (int x = 0; x < n; ++x) {
				if ((x + y) % 2) {
					fill(1, 1, 1, 0);
				}
				else {
					fill(1, 1, 1, 1);
				}
				float w = 80.0f;
				float h = 80.0f;
				float ofstX = (width - w * n) / 2 + w / 2;
				float ofstY = (height - h * n) / 2 + h / 2;
				float px = ofstX + w * x;
				float py = ofstY + h * y;
				rect(px, py, w, h, rad);
				fill(1, 1, 0.4f);
				circle(px, py, w * 0.5f);
			}
		}

		//image ファイル名重複しても大丈夫です
		imageColor(1, 0.5f, 0.5f);
		for (int i = 0; i < n; i++) {
			image("assets/penguin1.png", width - 500 + 100 * i, height / 2, rad, 0.3f, 0.3f);
		}

		//arrow
		float sx = width / 6;
		float sy = height / 2;
		float ex = sx + cosf(rad) * 200;
		float ey = sy - sinf(rad) * 200;
		float arrowLen = 40, arrowRad = 30;
		stroke(0.25f, 0.5f, 1);
		strokeWeight(10);
		arrow(sx, sy, ex, ey, arrowLen, arrowRad);
		fontRectModeCenter();
		fontSize(50);
		fontColor(0.f, 0.f, 0.f);
		text("a", ex + cosf(rad) * 22, ey - sinf(rad) * 22);

		//text 漢字
		int size = 100;
		fontSize(size);
		fontColor(0.f, 0.6f, 0.f);
		fontRectModeCorner();
		std::string str = "色即是空、空即是色";//←漢字にしてください
		text(str.c_str(), (width - size * 0.5f * str.size()) / 2, (height - size));

		//print
		fontSize(50);
		fontColor(0.f, 0.f, 0.f);
		print("numLoadTextures:%u", numLoadTextures());
		print("numFontTextures:%u", numFontTextures());
		print("numConstants:%u", numConstants());
		print("elapsed:%.2f", elapsed);
		endMsaaRender();
	}
}
