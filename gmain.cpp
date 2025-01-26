#define A_
#ifdef A_
#include"framework.h"

void gmain()
{
#if 0
	window("divide image", 640, 360, win);
#else
	window("divide image", 640, 360, full);
#endif
	hideCursor();
    int cursor = loadImage("assets/cursor.png");
    
	//画像を分割してdstImgsに画像番号を格納
	int srcImg = loadImage("assets/characters.png");
	const int col = 12, row = 8, w = 48, h = 64;
	int dstImgs[row * col];
	divideImage(srcImg, col, row, w, h, dstImgs);

	//dstImgsからキャラを選択してimgsにコピー
    int charac = 0, charar = 0;//character column, character row
    int ofsc = 3 * charac, ofsr = 4 * charar;//offset column, offset row
    int imgs[4 * 4];//最終的に４＊４の画像番号配列にする
	for (int r = 0; r < 4; r++) {
		for (int c = 0; c < 4; c++) {
			if (c <= 2) {
				imgs[4 * r + c] = dstImgs[col * (r + ofsr) + (c + ofsc)];
			}
			else {
                //3列目は1列目の画像を使う
				imgs[4 * r + c] = dstImgs[col * (r + ofsr) + (1 + ofsc)];
			}
		}
	}
	int idx = 0;
	
	initDeltaTime();
	while (!quit())
	{
		setDeltaTime();
		getInputState();
		if (isTrigger(KEY_ESC)) closeWindow();
		//clear
		beginMsaaRender();
		//background rect
		fill(1.0f, 1.0f, 0.55f); noStroke(); rectModeCorner();
		rect(0, 0, width, height);
		//circle
        fill(0.9f, 0.5f, 0.5f);	stroke(1.0f, 1.0f, 1.0f); strokeWeight(10);
        circle(-80, height / 2+10, width-140);
		//rect
        fill(0.5f, 0.5f, 0.9f); stroke(1.0f, 1.0f, 1.0f); rectModeCenter();
        rect(width+50, height / 2+200, 500, 500, 0.5f);

		//image
		if (timer(0, 0.2f))++idx %= 16;
		float scale = 2;
		image(imgs[idx], width / 2, height / 2, 0, scale, scale);

		//cursor
        rectModeCorner();
        image(cursor, mouseX, mouseY);
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