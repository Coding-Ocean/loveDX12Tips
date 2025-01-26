#define A_
#ifdef A_
#include"framework.h"

void gmain()
{
#if 0
	window("divide image", 640, 320, win);
#else
	window("divide image", 640, 320, full);
#endif
	hideCursor();
    int cursor = loadImage("assets/cursor.png");
    
	//画像を分割してdstImgsに画像番号を格納
	int srcImg = loadImage("assets/characters.png");
	const int row = 8, col = 12;
	int dstImgs[row * col];
	int w = 48, h = 64;
	divideImage(srcImg, row, col, w, h, dstImgs);

	//dstImgsからキャラを選択してimgsにコピー
    int charac = 0, charar = 0;//character column, character row
	int ofsc = 3 * charac, ofsr = 4 * charar;
	int imgs[16];
	for (int r = 0; r < 4; r++) {
		int c;
		for (c = 0; c < 3; c++) {
			imgs[4 * r + c] = dstImgs[col * (r + ofsr) + (c + ofsc)];
		}
		imgs[4 * r + c] = dstImgs[col * (r + ofsr) + (1 + ofsc)];
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