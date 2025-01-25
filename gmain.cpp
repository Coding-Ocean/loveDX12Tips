#define A_
#ifdef A_
#include<cmath>
#include"framework.h"
#include"mathGraphic.h"
void gmain()
{
#if 1
	window("cutImage", 1280, 720, win);
#else
	window("Math", 1280, 720, full);
	//フルスクリーンモードで、
	//指定した幅と高さのアスペクト比と
	//ディスプレイ設定のアスペクト比が違う場合
	//実際のカーソル位置と使用するカーソル位置がずれるので、隠さないといけない。
	//数値的には問題ない。
	hideCursor();
#endif
	//cursor image
	int img1 = loadImage("assets/test2.png");
	int img2 = cutImage(img1, 48 * 0, 48 * 0, 48, 48);
	int img3 = cutImage(img1, 48 * 1, 48 * 0, 48, 48);
	int img4 = cutImage(img1, 48 * 2, 48 * 0, 48, 48);
	//int imgs[3];
	//int idx = 0;
	//int vdx = 1;
	initDeltaTime();
	while (!quit())
	{
		setDeltaTime();
		getInputState();
		if (isTrigger(KEY_ESC)) closeWindow();
		//clear
		beginMsaaRender();
		//フルスクリーンの時、rectでウィンドウの枠線を引く
		noFill();//今回は塗りつぶさないが、もちろんfillで塗りつぶしてもよい。
		stroke(0.5f, 0.5f, 0.5f);
		strokeWeight(2);
		rectModeCorner();
		rect(0, 0, width, height);
		//circle
        fill(0.9f, 0.5f, 0.5f);
		stroke(1.0f, 1.0f, 1.0f);
        strokeWeight(10);
        circle(80, height / 2, 500);
		//rect
        fill(0.5f, 0.5f, 0.9f);
        stroke(1.0f, 1.0f, 1.0f);
        rect(width / 2, height / 2, 500, 500, 0.5f);
		//image
		rectModeCenter();
		float scale = 1;
		image(img2, width / 2 - 48 * scale, height / 2, 0, scale, scale);
		image(img3, width / 2, height / 2, 0, scale, scale);
		image(img4, width / 2 + 48 * scale, height / 2, 0, scale, scale);
		//info
		fontSize(20);
		fontColor(0.5f, 0.5f, 0.5f);
		fontRectModeCorner();
		print("numConstants:%d", numConstants());
		print("numTextures:%d", numLoadTextures());
		print("deltaTime:%.3f", delta);
		print("mouseX:%.2f", mouseX);
		print("mouseY:%.2f", mouseY);
		//present
		endMsaaRender();
	}
}
#endif