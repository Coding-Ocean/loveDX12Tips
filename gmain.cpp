#define A_
#ifdef A_
//math系関数を使用しないでつくる。
//デカルト座標で考え、スクリーン座標に変換して描画する。
#include<cmath>
#include"framework.h"
#include"mathGraphic.h"
void gmain()
{
#if 1
	window("Math", 128*6, 72*6, win);
#else
	window("Math", 128, 72, full);
	//フルスクリーンモードで、
	//指定した幅と高さのアスペクト比と
	//ディスプレイ設定のアスペクト比が違う場合
	//実際のカーソル位置と使用するカーソル位置がずれるので、隠さないといけない。
	//数値的には問題ない。
	hideCursor();
#endif
	//cursor image
	int img = loadImage("assets/cursor.png");
	int img1 = loadImage("assets/test2.png");
	int img2 = cutImage(img1, 48 * 0, 48 * 0, 48, 48);
	int img3 = cutImage(img1, 48 * 1, 48 * 0, 48, 48);
	int img4 = cutImage(img1, 48 * 2, 48 * 0, 48, 48);
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
		//rect(0, 0, width, height);
		//image
		rectModeCenter();
		float scale = 4;
		image(img2, width / 2 - 48 * scale, height / 2, 0, scale, scale);
		image(img3, width / 2, height / 2, 0, scale, scale);
		image(img4, width / 2 + 48 * scale, height / 2, 0, scale, scale);
		//cursor
		rectModeCorner();
		image(img, mouseX, mouseY);
		//info
		fontSize(20);
		fontColor(0.5f, 0.5f, 0.5f);
		fontRectModeCorner();
		print("numConstants:%d", numConstants());
		print("deltaTime:%.3f", delta);
		print("mouseX:%.2f", mouseX);
		print("mouseY:%.2f", mouseY);
		//present
		endMsaaRender();
	}
}
#endif