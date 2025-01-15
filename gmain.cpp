
#include"graphic.h"
#include"input.h"

void gmain()
{
	window("Input", 1280, 720);

	float rad = 0;
	float interval = 0;
	int n = 5;
	int inc = 1;
	float elapsed = 0;
	float toRad = 3.1415926f / 180;

	float px = width / 2;
	float py = height / 2;
	float scale = 0.3f;
	setMousePos(400, height / 2);


	float deltatime;
	unsigned long long int cnt = 0;
	initDeltaTime();
	while (!quit())
	{
		//更新------------------------------------------------
		setDeltaTime();
		getInputState();
		if (isTrigger(KEY_ESC))closeWindow();
		//経過時間
		elapsed += delta;
		cnt++;
		deltatime = elapsed / cnt;
		//描画------------------------------------------------
		beginMsaaRender();

		//arrow
		float sx = width / 6;
		float sy = height / 2;
		float ex = mouseX;
		float ey = mouseY;
		float dx = ex - sx;
		float dy = ey - sy;
		float len = sqrtf(dx * dx + dy * dy);
		dx /= len;
		dy /= len;
		float arrowLen = 20, arrowRad = 30;
		stroke(0.25f, 0.5f, 1);
		strokeWeight(10);
		arrow(sx, sy, sx+dx*300, sy+dy*300, arrowLen, arrowRad);
		fontRectModeCenter();
		fontSize(50);
		fontColor(0.f, 0.f, 0.f);
		text("a", ex + dx * 24, ey + dy * 24);

		//image ファイル名重複しても大丈夫です
		//imageColor(1, 0.5f, 0.5f);
		if (isPress(KEY_D))px += 400 * deltatime;// (elapsed / cnt);
		if (isPress(KEY_A))px -= 400 * delta;
		if (isPress(MOUSE_LBUTTON))rad += 90.0f * toRad * deltatime;
		if (isPress(MOUSE_RBUTTON))rad -= 90.0f * toRad * delta;
		//マウスホィールはgraphic.cpp
		scale += 0.1f * mouseWheel;
		image("assets/penguin1.png", px, py, rad, scale, scale);

		//text 漢字
		//int size = 100;
		//fontSize(size);
		//fontColor(0.f, 0.6f, 0.f);
		fontRectModeCorner();
		//std::string str = "色即是空、空即是色";//←漢字にしてください
		//text(str.c_str(), (width - size * 0.5f * str.size()) / 2, (height - size));

		//print
		//fontSize(50);
		//fontColor(0.f, 0.f, 0.f);
		//print("numLoadTextures:%u", numLoadTextures());
		//print("numFontTextures:%u", numFontTextures());
		//print("numConstants:%u", numConstants());
		print("deltatime:%f", elapsed / cnt);
		print("deltatime:%f", delta);
		endMsaaRender();
	}
}
