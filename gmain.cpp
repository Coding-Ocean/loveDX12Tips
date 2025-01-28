//ごった煮サンプル
#if 1
#include"framework.h"
void selectImgs(int* allImgs, int col, int row, int* imgs)
{
	//選択するキャラクタの列と行を決定
	static int charac = 0, charar = 1;//character column, character row
	if (isTrigger(KEY_D)) {
		++charac %= col / 3;
	}
	if (isTrigger(KEY_S)) {
		++charar %= row / 4;
	}
	if (isTrigger(KEY_A)) {
		--charac;
		if (charac < 0)charac += col / 3;
	}
	if (isTrigger(KEY_W)) {
		--charar;
		if (charar < 0)charar += row / 4;
	}
	//選択されたキャラをimgsにコピー
	int ofsc = 3 * charac, ofsr = 4 * charar;//offset column, offset row
	for (int r = 0; r < 4; r++) {
		for (int c = 0; c < 4; c++) {
			if (c <= 2) {
				imgs[4 * r + c] = allImgs[col * (r + ofsr) + (c + ofsc)];
			}
			else {
				//3列目は1列目の画像を使う
				imgs[4 * r + c] = allImgs[col * (r + ofsr) + (1 + ofsc)];
			}
		}
	}
}

void gmain()
{
	window("divide image", 640, 360, false);
	hideCursor();
    int cursor = loadImage("assets/cursor.png");
	int srcImg = loadImage("assets/characters.png");
	
	//srcImgの画像を分割してallImgsに画像番号を格納
	const int col = 12, row = 8, w = 48, h = 64;
	int allImgs[row * col];
	divideImage(srcImg, col, row, w, h, allImgs);

	//allImgsからキャラを選択してimgsにコピー
	int imgs[4 * 4];//最終的に４＊４の画像番号配列にする
	selectImgs(allImgs, col, row, imgs);
	int idx = 0;
	
	initDeltaTime();
	while (!quit())
	{
		setDeltaTime();
		getInputState();
		if (isTrigger(KEY_ESC)) closeWindow();

		//switch character
		if (isTrigger(KEY_D) || isTrigger(KEY_S) || isTrigger(KEY_A) || isTrigger(KEY_W)) {
			selectImgs(allImgs, col, row, imgs);
			idx = 0;
		}

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

		//math functions
		stroke(0.5f, 1.0f, 0.5f);
		mathSetAxis(width / 4 * 3, 242, 100);
		mathAxis();
		float ax = 1, ay = -0.5f;
		float bx = mathMouseX, by = mathMouseY;
		float radius = sqrtf(bx * bx + by * by)*0.2f;
		mathArrow(0, 0, ax, ay);
		mathArrow(0, 0, bx, by);
		mathArc(ax, ay, bx, by, radius);

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
		print("numLoadTextures:%d", numLoadTextures());
		print("numFontTextures:%d", numFontTextures());
		print("deltaTime:%.3f", delta);
		print("mouseX:%.2f", mouseX);
		print("mouseY:%.2f", mouseY);
		print("mathMouseX:%.2f", mathMouseX);
		print("mathMouseY:%.2f", mathMouseY);
		//present
		endMsaaRender();
	}
}
#endif