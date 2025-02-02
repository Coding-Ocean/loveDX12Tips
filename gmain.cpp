//画像切り取り、分割、描画サンプル
#if 1
#include"framework.h"
void gmain()
{
	window("Game", 640, 360);

	//original image
	int orgImg = loadImage("assets/character.png");
    int texWidth = textureWidth(orgImg);//描画位置を計算するために画像の幅を取得
    int texHeight = textureHeight(orgImg);//描画位置を計算するために画像の高さを取得

    //cut image
	int w = 48;//切り取る幅
    int h = 48;//切り取る高さ
	int left = w * 1;//定数は列
    int top = h * 3;//定数は行
    int cutImg = cutImage(orgImg, left, top, w, h);
	
    //divide image
	const int col = 3;
	const int row = 4;
	int divImg[col * row];
	divideImage(orgImg, col, row, w, h, divImg);

	//font size
    int size = 20;
	
	initDeltaTime();
	while (!quit())
	{
		setDeltaTime();
		getInputState();
		if (isTrigger(KEY_ESC)) closeWindow();

		//clear
		beginMsaaRender();
        
		//共通設定
		fontColor(1, 1, 1);
        fontSize(size);
		rectModeCorner();

		//original image
		float x = width / 4 - texWidth / 2;//画面幅1/4-画像幅/2に配置
        float y = (height - texHeight) / 2;//画像の上端が画面の中央に来るようにする
        image(orgImg, x, y);
        text(" originalImage", x, y - size);

        //cut image
		x = width / 2;
        image(cutImg, x, y);
        text(" cutImage", x, y - size);
        
        //divide image
		static int idx = 0;
        if (timer(0, 0.25f)) ++idx %= col*row;//0.25秒ごとに画像番号を切り替える
        x = width / 4 * 3;
        image(divImg[idx], x, y);
		text(" divideImage", x, y - size);

		//present
		endMsaaRender();
	}
}
#endif
//フォントを変えるサンプル
#if 0
#include"framework.h"
void gmain()
{
	window("Font", 1280, 720);
	setPrintInitX(330);
	fontSize(100);
	beginRender();
	for (int i = 0; i < 7; i++) {
		switch (i) {
		case 0:fontFace("Cooper Black", EN); break;
		case 1:fontFace("Comic Sans MS", EN); break;
		case 2:fontFace("Bauhaus 93", EN); break;
		case 3:fontFace("Kristen ITC", EN); break;
		case 4:fontFace("Harlow Solid Italic", EN); break;
		case 5:fontFace("Showcard Gothic", EN); break;
		case 6:fontFace("jokerman", EN); break;
		}
		fontColor(1, 1.0f / 6 * i, 1);
		print("%d.Coding Ocean", i);
	}
	endRender();
	while (!quit()) {
		getInputState();
		if (isTrigger(KEY_ESC))
			closeWindow();
	}
}
#endif
//インストールしていないフォントを使うサンプル
#if 0
#include<memory>
#include"framework.h"
void gmain()
{
	window("Font2", 1280, 720);

	//assetsフォルダ内に用意したフォントが使える（ループ中にいれてはいけない）
	USER_FONT uf("assets\\PixelMplus12-Regular.ttf");
	fontFace("PixelMplus12", JP);

	//文字列（全角文字前提）
	const char* str{ "白魔導士はエリクサーをなくして死んだ" };
	size_t strLen = strlen(str);
	auto dispStr = std::make_unique<char[]>(strLen + 1);//ループの中で表示する文字だけここにコピーする
	strLen /= 2;//全角文字数にする
	int dispCnt = 0;//表示全角文字数
	initDeltaTime();
	while (!quit()) {
		setDeltaTime();
		getInputState();
		if (isTrigger(KEY_ESC)) closeWindow();

		//表示する文字をdispCntの数だけstrからdispStrにコピーする
		if (dispCnt < strLen) {
			if (timer(0, 0.1f)) {
				++dispCnt;
				strncpy_s(dispStr.get(), dispCnt * 2 + 1, str, dispCnt * 2);
			}
		}
		else {
			//表示終了後、１秒待ってdispCntをリセット
			if (timer(1, 1.0f)) {
				dispCnt = 0;
			}
		}

		beginRender();
		//表示枠
		fill(0, 0, 1); stroke(1, 1, 1); strokeWeight(10);
		rect((width - 1200) / 2, (height - 200) / 2, 1200, 200);
		//文字列表示
		fontSize(70); fontColor(1, 1, 1); 
		text(dispStr.get(), 64, (height - 70) / 2);
		//カーソル表示
		cursor();
		endRender();
	}
}
#endif
//デカルト座標でベクトルを学ぶためのサンプル
#if 0
#include<cmath>
#include"framework.h"
#include"mathGraphic.h"
void gmain()
{
	window("Math", 640, 640, full);
	//axis
	//　原点の位置をスクリーン座標で指定する
	float ox = width / 2;
	float oy = height / 2;
	//　１とする大きさをドット数で指定する
	float scale = width / 6;
	//vector a
	float ax = 1.5f;
	float ay = 0;
	float al = sqrt(ax * ax + ay * ay);//length
	float anx = ax / al;//normalized x
	float any = ay / al;//normalized y
	//vector b
	float bx;
	float by;
	float bnx;//normalized x
	float bny;//normalized y
	//radian for rotate
	float rad = 0;
	//distance from vector tip to text
	float dist = 0.15f;
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
		//set math axis
		if (isPress(MOUSE_MBUTTON)) {
			ox += mouseVx;
			oy -= mouseVy;
		}
		scale += 10 * mouseWheel;
		if (scale <= 50)scale = 50;
		mathSetAxis(ox, oy, scale);
		//change vector a.
		if (isTrigger(MOUSE_LBUTTON)) {
			ax = mathMouseX;
			ay = mathMouseY;
			al = sqrt(ax * ax + ay * ay);//blをradiusとする
			anx = ax / al;
			any = ay / al;
		}
		//rotate vector b
		bnx = cos(rad);
		bny = sin(rad);
		rad += 90 * 3.1415926f / 180 * delta;
		bx = bnx * al;//vector aと同じ大きさにする
		by = bny * al;
		//circle
		mathStrokeWeight(0.05f);
		stroke(0.6f, 0.6f, 1.0f);
		mathCircle(0, 0, al * 2);
		//setup text font
		fontRectModeCenter();
		fontSize(25);
		fontColor(1, 1, 1);
		//arrow a
		stroke(1.0f, 0.5f, 0.5f);
		mathArrow(0, 0, ax, ay);
		mathText("a", ax + anx * dist, ay + any * dist);
		//arrow b
		stroke(1.0f, 1.0f, 0.6f);
		mathArrow(0, 0, bx, by);
		mathText("b", bx + bnx * dist, by + bny * dist);
		//arc
		stroke(0.5f, 0.5f, 0.5f);
		mathArc(ax, ay, bx, by, al / 6);
		//axis
		stroke(0.6f, 0.6f, 0.6f);
		mathAxis();
		//info
		fontSize(20);
		fontRectModeCorner();
		print("numConstants:%d", numConstants());
		print("deltaTime:%.3f", delta);
		print("mathMouseX:%.2f", mathMouseX);
		print("mathMouseY:%.2f", mathMouseY);
		//present
        cursor();
		endMsaaRender();
	}
}
#endif
//デカルト座標でグラフを表示するサンプル
#if 0
#include<cmath>
#include"framework.h"
#include"mathGraphic.h"
//2次関数
float quadratic(float x)
{
	return x * x;
}
//ゲームメイン
void gmain()
{
	window("Math", 720, 720, full);
	//原点の位置（スクリーン座標）
	float ox = width / 2;
	float oy = height / 2;
	//１とする大きさ。（ドット数）
	float scale = 50;
    //グラフに沿って動く円
    float diameter = 0.15f;//円の直径
	float minX = 0;//グラフのxの最小値
    float maxX = 0;//グラフのxの最大値
    float px = minX;//円のx座標
    float vx = 1.5f;//x座標の速度
	initDeltaTime();
	while (!quit())
	{
		//更新------------------------------------------------
		setDeltaTime();
		getInputState();
		if (isTrigger(KEY_ESC)) closeWindow();

		//描画------------------------------------------------
		beginMsaaRender();

		//rectでウィンドウの枠線を引く
		noFill();
		stroke(0.5f, 0.5f, 0.5f);
		strokeWeight(2);
        backgroundRect();

		//set math axis
		if (isPress(MOUSE_LBUTTON)) {
			ox += mouseVx;
			oy -= mouseVy;
		}
		scale += 10 * mouseWheel;
		if (scale <= 50)scale = 50;
		mathSetAxis(ox, oy, scale);

		//function graph and move circle
		minX = -ox / scale;
		maxX = (width - ox) / scale;
		px += vx * delta;
		if (px > maxX)px = minX;
		mathStrokeWeight(0.03f);

		fill(0, 0, 0, 0);
		stroke(1, 0.5f, 0.5f);
		mathGraph(sin);
		mathCircle(px, sin(px), diameter);

		stroke(0.5f, 0.5f, 1.0f);
		mathGraph(cos);
		mathCircle(px, cos(px), diameter);

		stroke(0.5f, 1, 0.5f);
		mathGraph(quadratic);
		mathCircle(px, quadratic(px), diameter);

		stroke(0.9f, 0.9f, 0.9f);
		mathAxis();

		//info
		fontSize(30);
        fontColor(1, 1, 0);
        print("左ボタンドラッグで移動、ホイールで拡大縮小");
		fontColor(0.5f, 0.5f, 0.5f);
		print("numConstants:%d", numConstants());
		print("deltaTime:%.3f", delta);
		print("mathMouseX:%.2f", mathMouseX);
		print("mathMouseY:%.2f", mathMouseY);

		//present
        cursor();
		endMsaaRender();
	}
}
#endif
//circle,rect,影font,msaaサンプル
#if 0
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
	window("divide image", 16 * 80, 9 * 80, full);
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
        circle(-200, height / 2+80, width-140);
		//rect
        fill(0.5f, 0.5f, 0.9f); stroke(1.0f, 1.0f, 1.0f); rectModeCenter();
        rect(width-100, height / 2+200, 700, 700, 0.05f);
		//image
		if (timer(0, 0.2f))++idx %= 16;
		float scale = 2;
		image(imgs[idx], width / 2, height / 2, 0, scale, scale);
		//math functions
		stroke(0.5f, 1.0f, 0.5f);
		mathSetAxis(width-120, 600, 100);
		mathAxis();
		float ax = 1, ay = -0.5f;
		float bx = mathMouseX, by = mathMouseY;
		float radius = sqrtf(bx * bx + by * by)*0.2f;
		strokeWeight(5);
		mathArrow(0, 0, ax, ay);
		mathArrow(0, 0, bx, by);
		mathArc(ax, ay, bx, by, 0.2f);
		fontColor(1, 1, 1);
		fontShadowColor(0.f, 0.f, 0.f, 1.f);
		text(mouseX, mouseY, "  (%.2f,%.2f)", mathMouseX,mathMouseY);

		//cursor
		cursor();
		//info
		fontSize(20);
		fontColor(1.f, 1.f, 1.f);
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
//circle調整用
#if 0
#include"framework.h"
void gmain()
{
	window("Game", 1920, 1080, full);

	float radius = 250;
	float fontSize = 50;
	initDeltaTime();
	while (!quit())
	{
		setDeltaTime();
		getInputState();
		if (isTrigger(KEY_ESC)) closeWindow();

		//clear
		beginMsaaRender();
		backgroundRect(0.9f, 0.8f, 0.8f);


		fill(1, 1, 1); strokeWeight(5);
		circle(width / 2, height / 2, radius * 2);

		if (isTrigger(KEY_W))radius += 1;
		if (isTrigger(KEY_S))radius -= 1;
		if (isPress(KEY_D))radius += 1;
		if (isPress(KEY_A))radius -= 1;

		rectModeCorner();
		::fontSize(fontSize);
		{
			float l = width / 2 - 125, t = height / 2 - 50, w = 100, h = 100;
			if (mouseX > l && mouseX < l + w && mouseY > t && mouseY < t + h)
			{
				fill(0.9f, 0.8f, 0.8f, 1);
				radius += 1;
			}
			else
			{
				fill(0.5f, 0.5f, 0.5f, 1);
			}
			strokeWeight(5);
			rect(l, t, w, h);
			text(l + (w - fontSize / 2 * 3) / 2, t + (h - fontSize) / 2, "Inc");
		}
		{
			float l = width / 2 + 25, t = height / 2 - 50, w = 100, h = 100;
			if (mouseX > l && mouseX < l + w && mouseY > t && mouseY < t + h)
			{
				fill(0.9f, 0.8f, 0.8f, 1);
				radius -= 1;
				if (radius < 1)radius = 1;
			}
			else
			{
				fill(0.5f, 0.5f, 0.5f, 1);
			}
			strokeWeight(5);
			rect(l, t, w, h);
			text(l + (w - fontSize / 2 * 3) / 2, t + (h - fontSize) / 2, "Dec");
		}

		//info
		fontColor(1.f, 1.f, 1.f, 1.0f);
		fontShadowColor(0.f, 0.f, 0.f, 1.0f);
		fontRectModeCorner();
		debugPrint();
		print("radius:%.2f", radius);

		//present
		cursor();
		endMsaaRender();
	}
}
#endif