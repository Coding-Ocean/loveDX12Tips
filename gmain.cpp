//画像切り取り、分割、描画サンプル
#if 0
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
#include"framework.h"
void gmain()
{
	window("Math", 1080, 1080, full);
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
		mathStrokeWeight(0.025f);
		stroke(0.6f, 0.6f, 1.0f);
		mathCircle(0, 0, al * 2);
		//setup text font
		fontRectModeCenter();
		fontSize(40);
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
		mathStrokeWeight(0.01f);
		stroke(0.6f, 0.6f, 0.6f);
		mathAxis();
		//info
		fontSize(40);
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
#include"framework.h"
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
	static int charac = 1, charar = 1;//character column, character row
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
		mathStrokeWeight(0.02f);
		mathSetAxis(width-300, height/2+62, 100);
		mathAxis();
		mathStrokeWeight(0.04f);
		float ax = mathMouseX, ay = mathMouseY;
		mathArrow(0, 0, ax, ay);
		mathText(ax, ay, "  (%.2f,%.2f)", ax, ay);

		//cursor
		cursor();
		//info
		fontSize(20);
		fontColor(1.f, 1.f, 1.f);
		fontShadowColor(0.f, 0.f, 0.f, 1.f);
		fontRectModeCorner();
		debugPrint();
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
//円と線分の当たり判定
#if 0
#include"framework.h"
#include"float2.h"
void gmain()
{
	window("Math", 1600, 900, win, 300);
	//スクリーン中央座標
	float cx = width / 2;
	float cy = height / 2;
	//マウスの初期位置
	setMousePos(cx-50, cy-100);
	//線分の始点 s
	float2 s(cx - 200, cy + 100);
	//線分の終点 e
	float2 e(cx + 200, cy - 100);
	//円の中心点 p。ループ中でマウス位置をセットする
	float2 p;
	float radius = 50;
	//メインループ
	while (!quit())
	{
		getInputState();
		if (isTrigger(KEY_ESC)) closeWindow();

		//clear
		beginMsaaRender();

		//------------------------------------
		//円の中心点と線分までの最短距離を求める
		p.set(mouseX, mouseY);
		float2 b = p - s;
		float2 a = e - s;//segment vector a
		float t = dot(a, b) / dot(a, a);
		if (t < 0)t = 0;//min 0
		else if (t > 1)t = 1;//max 1
		float distance = (b - t * a).mag();
		//触れていたら色を変える
		if (distance <= radius)
			stroke(1, .5f, .5f);
		else
			stroke(1, 1, 1);
		strokeWeight(3);
		noFill();
		circle(p.x, p.y, radius * 2-3);
		line(s.x, s.y, e.x, e.y);

		//距離の視覚化
		static int sw = 1;
		if (isTrigger(KEY_A))sw = 1 - sw;
		if(sw){
			//「円の中心点」から「線分上の最も近い点」の位置q
			float2 q = s + t * a;
			stroke(1, 1, .5f);
			line(p.x, p.y, q.x, q.y);
			//点s,e,pを文字で表す
			fontColor(.5f, .5f, .5f);
			fontRectModeCenter();
			float2 ofst = (e - s).normalize()*25;
			text("s", s.x - ofst.x, s.y - ofst.y);
			text("e", e.x + ofst.x, e.y + ofst.y);
			text("p", p.x, p.y);
		}
		//------------------------------------
		
		//info
		fontRectModeCorner();
		fontSize(50);
		fontColor(1, 1, 1);
		print("radius=%.1f", radius);
		fontColor(1, 1, .5f);
		print("distance=%.1f", distance);
		
		//present
		endMsaaRender();
	}
}
#endif
//円と四角形の当たり判定
#if 0
#include"framework.h"
#include"float2.h"

bool circle_segment
(
	float2 p,//円の中心
	float radius,
	float2 s,//線分の始点
	float2 e//線分の終点
)
{
	float2 b = p - s;
	float2 a = e - s;//segment vector a
	float t = a.dot(b) / a.dot(a);
	if (t < 0)t = 0;//min 0
	else if (t > 1)t = 1;//max 1
	float distance = (b - t * a).mag();
	if (distance <= radius) {
		return true;
	}
	return false;
}

bool circle_rect
(
	float2 p,//円の中心 
	float radius, 
	float2* vtx//四角形の頂点配列
)
{
	int cnt = 0;
	for (int i = 0; i < 4; ++i) {
		int j = (i + 1) % 4;
		if (circle_segment(p, radius, vtx[i], vtx[j])) {
			//辺に触れている
			return true;
		}
		if ((vtx[j] - vtx[i]).crossZ(p - vtx[i]) > 0) {
			if (++cnt == 4) {
				//全ての辺の内側にいる
				return true;
			}
		}
	}
	return false;
}

void gmain()
{
	window("Math", 1270, 720, win, 300);
	//スクリーンの中央座標
	float cx = width / 2;//center x
	float cy = height / 2;//center y
	//マウスの初期位置
	setMousePos(cx - 50, cy - 100);
	//四角形の頂点
	float2 vtx[4];
	vtx[0].set(cx - 100, cy - 0);
	vtx[1].set(cx + 100, cy - 50);
	vtx[2].set(cx + 90, cy + 50);
	vtx[3].set(cx - 90, cy + 50);
	//円の中心点 p。ループ中でマウス位置をセットする
	float2 p;
	float radius = 30;
	//メインループ
	while (!quit())
	{
		getInputState();
		if (isTrigger(KEY_ESC)) closeWindow();

		//clear
		beginMsaaRender();

		//------------------------------------
		p.set(mouseX, mouseY);
		//触れていたら色を変える
		if (circle_rect(p, radius, vtx))
			stroke(1, 0.3f, 0.3f);
		else
			stroke(1, 1, 1);
		strokeWeight(3);
		noFill();
		circle(p.x, p.y, radius * 2 - 3);
		for (int i = 0; i < 4; ++i) {
			int j = (i + 1) % 4;
			line(vtx[i].x, vtx[i].y, vtx[j].x, vtx[j].y);
		}
		//------------------------------------

		//present
		endMsaaRender();
	}
}
#endif
//成す角 デカルト座標
#if 0
#include"framework.h"
void gmain() {
	window("math", 1920, 1080, full);
	//オブジェクトデータ
	float2 o(0, 0);
	float2 a(1, 0);
	float2 b(0.866f, 0.5f);
	//マウスで点をつかむためのデータ
	float2 mouse;
	float2* points[] = { &a,&b };
	int numPoints = _countof(points);
	float2* grabPoint = nullptr;
	float grabRadiusSq = powf(0.04f, 2);
	//メインループ
	while (!quit())
	{
		getInputState();
		if (isTrigger(KEY_ESC)) closeWindow();

		//clear
		beginMsaaRender();
		//背景
		strokeWeight(2);
		stroke(0.7f, 0.7f, 0.7f);
		fill(0, 0, 0);
		backgroundRect();
		//デカルト座標
		mathSetAxis(width/2, height/2, 200);
		mathAxis();
		//マウスで点をつかんで移動する
		{
			mouse.set(mathMouseX, mathMouseY);
			if (isPress(MOUSE_LBUTTON)) {
				if (grabPoint == nullptr) {
					//つかむ
					for (int i = 0; i < numPoints; i++) {
						if ((*points[i] - mouse).magSq() <= grabRadiusSq) {
							grabPoint = points[i];
						}
					}
				}
				else {
					if (mouseVx != 0 || mouseVy != 0) {
						//移動
						*grabPoint = mouse;
					}
				}
			}
			else {
				grabPoint = nullptr;
			}
		}
		//内積・外積・なす角
		float dp = a.x * b.x + a.y * b.y;//|a||b|cosθ
		float cp = a.x * b.y - a.y * b.x;//|a||b|sinθ
		float angleAB = atan2(cp, dp);
		//オブジェクト表示
		//底辺・内積
		float2 an = normalize(a);
		float2 d = an * dp;
		mathLine(o.x, o.y, d.x, d.y);
		//高さ・外積
		float2 oa(-an.y, an.x);
		float2 c = oa * cp;
		mathLine(d.x, d.y, d.x + c.x, d.y + c.y);
		//斜辺
		mathLine(o.x, o.y, d.x + c.x, d.y + c.y);

		strokeWeight(6);
		//弧・なす角
		stroke(0.7f, 0.7f, 0.14f);
		mathArc(a.x,a.y,b.x,b.y,0.15f);
		//ベクトル
		stroke(0.93f, 0.34f, 0.42f);
		mathArrow(o.x, o.y, a.x, a.y);
		stroke(0, 0.74f, 0.62f);
		mathArrow(o.x, o.y, b.x, b.y);
		
		fontRectModeCenter();
		fontSize(30);
		fontColor(1, 1, 1);
		float2 ofsa = normalize(a) * 0.08f;
		float2 ofsb = normalize(b) * 0.08f;
		mathText("a", a.x+ofsa.x, a.y+ofsa.y);
		mathText("b", b.x+ofsb.x, b.y+ofsb.y);
		fontRectModeCorner();
		fontColor(0.8f, 0.8f, 0);
		print("マウスでベクトルの先端をつかんで動かせる");
		print("内積:a.x * b.x + a.y * b.y = |a||b|cosθ = %f", dp);
		print("外積:a.x * b.y - a.y * b.x = |a||b|sinθ = %f", cp);
		print("なす角:atan2(内積,外積) = %.1f",angleAB*180/3.1415926f);

		imageColor(1, 1, 1, 0.5f);
		cursor();
		endMsaaRender();
	}
}
#endif
//成す角 スクリーン座標
#if 0
#include"framework.h"
#include"float2.h"
void gmain()
{
	window("Math", 1920, 1080, full);
	//原点・スクリーン中央座標
	float2 o(width / 2, height / 2);
	//ベクトルa,b
	float2 a(200, 0);
	float2 b(173.2f, -100);
	//マウスで点をつかむためのデータ
	float2 mouse;
	float2* points[] = { &a,&b };
	int numPoints = _countof(points);
	float2* grabPoint = nullptr;
	float grabRadiusSq = powf(5, 2);
	//メインループ
	while (!quit())
	{
		getInputState();
		if (isTrigger(KEY_ESC)) closeWindow();

		//clear
		beginMsaaRender();
		//背景
		strokeWeight(2);
		stroke(0.7f, 0.7f, 0.7f);
		fill(0, 0, 0);
		backgroundRect();
		//マウスでベクトルの先端をつかんで移動する
		mouse.set(mouseX, mouseY);
		if (isPress(MOUSE_LBUTTON)) {
			if (grabPoint == nullptr) {
				//つかむ
				for (int i = 0; i < numPoints; i++) {
					if ((*points[i] - (mouse - o)).magSq() <= grabRadiusSq) {
						grabPoint = points[i];
					}
				}
			}
			else {
				if (mouseVx != 0 || mouseVy != 0) {
					//移動
					*grabPoint = mouse - o;
				}
			}
		}
		else {
			grabPoint = nullptr;
		}
		//内積・外積・なす角
		float dp = a.x * b.x + a.y * b.y;//|a||b|cosθ
		float cp = a.x * b.y - a.y * b.x;//|a||b|sinθ
		float angleAB = atan2(cp, dp);
		//描画
		strokeWeight(5);
		stroke(0.93f, 0.34f, 0.42f);
		arrow(o.x, o.y, o.x + a.x, o.y + a.y);
		stroke(0, 0.74f, 0.62f);
		arrow(o.x, o.y, o.x + b.x, o.y + b.y);
		stroke(0.7f, 0.7f, 0.14f);
		arc(o.x, o.y, o.x + a.x, o.y + a.y, o.x + b.x, o.y + b.y, 20);
		//info
		fontRectModeCorner();
		fontSize(30);
		fontColor(0.8f, 0.8f, 0);
		print("マウスでベクトルの先端をつかんで動かせる");
		print("内積:a.x * b.x + a.y * b.y = |a||b|cosθ = %.0f", dp);
		print("外積:a.x * b.y - a.y * b.x = |a||b|sinθ = %.0f", cp);
		print("なす角:atan2(内積,外積) = %.1f (スクリーン座標は符号が逆になる)", angleAB * 180 / 3.1415926f);
		//present
		cursor();
		endMsaaRender();
	}
}
#endif
//Favorite color
#if 0
#include"framework.h"
#include"float2.h"
void gmain()
{
	window("graffiti", 1920, 1080, full);
	//clearColor(0.8f, 0.8f, 0.8f);
	float cx = width / 2;
	float cy = height / 2;
	float color[][3] = {
		239 / 255.f, 87 / 255.f, 108 / 255.f,//red
		0 / 255.f, 191 / 255.f, 160 / 255.f,//green
		16 / 255.f, 120 / 255.f, 151 / 255.f,//blue
		180 / 255.f, 180 / 255.f, 37 / 255.f,//yellow
	};
	//メインループ
	while (!quit())
	{
		//input
		getInputState();
		if (isTrigger(KEY_ESC)) closeWindow();

		//clear
		beginMsaaRender();

		strokeWeight(5);
		for (int i = 0; i < 4; ++i) {
			stroke(color[i][0], color[i][1], color[i][2]);
			line(cx - 200, cy - 300 + 200 * i, cx + 200, cy - 300 + 200 * i);
		}

		//present
		fontSize(25);
		fontColor(0.5f,0.5f,0.5f);
		debugPrint();
		cursor();
		endMsaaRender();
	}
}
#endif
//キャラがWASDキーを押した方向に回転しながら進む
#if 0
#include"framework.h"
#include"float2.h"
void gmain()
{
	window("Math", 1920, 1080, full, 300);
	//いぬのデータ
	int inu = loadImage("assets\\inu.png");//右向き画像であるところが重要！
	float2 pos(width / 2, height / 2);
	float rad = 0;
	float scale = 0.5f;
	float2 a, b;

	initDeltaTime();
	//メインループ
	while (!quit())
	{
		setDeltaTime();
		getInputState();
		if (isTrigger(KEY_ESC)) closeWindow();

		//clear
		beginMsaaRender();
		//背景
		fill(0.1f, 0.3f, 0.1f);
		noStroke();
		rect(width / 2, height / 2, width, height);
		//犬を動かす
		b.x = 0; 
		b.y = 0;
		if (isPress(KEY_A)) { b.x = -1; }
		if (isPress(KEY_D)) { b.x = 1; }
		if (isPress(KEY_W)) { b.y = 1; }
		if (isPress(KEY_S)) { b.y = -1; }
		if (b.x != 0 || b.y != 0) {
			//回転　デカルト座標で計算
			a.x = cos(rad);
			a.y = sin(rad);
			float dp = dot(a, b);
			float cp = crossZ(a, b);
			float rotSpeed = atan2f(cp, dp) * 0.25f * 60;
			rad += rotSpeed * delta;
			if (rad >  3.1415926f * 2)rad -= 3.1415926f * 2;
			if (rad < -3.1415926f * 2)rad += 3.1415926f * 2;
			//移動
			b.normalize();
			b *= 10 * 60 * delta;
			pos.x += b.x;
			pos.y -= b.y;//ここでスクリーン座標に対応
		}
		image(inu, pos.x, pos.y, rad, scale, scale);
		//info
		fontRectModeCorner();
		fontSize(25);
		fontColor(1, 1, 1);
		print("WASDキーを押した方向に回転しながら進む");
		print("deg = %.1f", rad * 180 / 3.1415926f);
		debugPrint();
		//present
		endMsaaRender();
	}
}
#endif
//rayMarching
#if 1
#define SPHERE
//#define FLOOR
//#define BOTH
#include"framework.h"
float2 spherePos(0, 0); float sphereRadius = 1;
float getDist(float2 p) {
	float sphere = (p-spherePos).mag() - sphereRadius;
	float plane = p.y + 1;
#ifdef SPHERE
	return sphere;
#endif
#ifdef FLOOR
	return plane;
#endif
	return min(plane, sphere);
}
void gmain() {
	window("math", 1920, 1080, full, 6000);
	//オブジェクトデータ
	float2 ro(-5, 0);//z,y
	float2 uv(-3, 1);//z,y
	//マウスで点をつかむためのデータ
	float2 mouse;
	float2* points[] = { &uv };
	int numPoints = _countof(points);
	float2* grabPoint = nullptr;
	float grabRadiusSq = powf(0.1f, 2);
	//拡大縮小
	float scale = 100;
	//ステップ実行
	int step = 50;
	//メインループ
	while (!quit())
	{
		getInputState();
		if (isTrigger(KEY_ESC)) closeWindow();

		//clear
		beginMsaaRender();
		//背景
		strokeWeight(2);
		stroke(0.7f, 0.7f, 0.7f);
		fill(0, 0, 0);
		backgroundRect();
		//デカルト座標
		scale += getMouseWheel()*10;
		mathSetAxis(width / 2, height / 2, scale);
		mathAxis();
		//マウスで点をつかんで移動する
		{
			mouse.set(uv.x, mathMouseY);
			if (isPress(MOUSE_LBUTTON)) {
				if (grabPoint == nullptr) {
					//つかむ
					for (int i = 0; i < numPoints; i++) {
						if ((*points[i] - mouse).magSq() <= grabRadiusSq) {
							grabPoint = points[i];
						}
					}
				}
				else {
					if (mouseVx != 0 || mouseVy != 0) {
						//移動
						*grabPoint = mouse;
					}
				}
			}
			else {
				grabPoint = nullptr;
			}
		}
		//uv上の１点
		if (uv.y > 1)uv.y = 1;
		if (uv.y < -1)uv.y = -1;
		strokeWeight(1);
		stroke(1, 1, 0);
		fill(1, 1, 0);
		mathCircle(uv.x, uv.y, 0.1f);
		mathLine(-3, 1, -3, -1);
		//ray origin
		mathCircle(ro.x, ro.y, 0.1f);

		noFill();
		strokeWeight(2);
		stroke(0.5f, 0.5f, 0.5f);
#if defined(SPHERE) || defined(BOTH)
		//球
		mathCircle(spherePos.x, spherePos.y, 2.f);
#endif
#if defined(FLOOR) || defined(BOTH)
		//床
		mathLine(-10, -1, 25, -1);
#endif
		//レイマーチング
		float2 rd = normalize(uv - ro);
		float t = 0;
		if (isTrigger(KEY_D))step++;
		if (isTrigger(KEY_A))step--;
		if (step > 50)step = 0;
		for (int i = 0; i < step; ++i) {
			float2 p = ro + rd * t;
			float r = getDist(p);
			t += r;
			if (r < 0.001f||t>40)break;
			//レイベクトル
			stroke(0.93f, 0.34f, 0.42f);
			strokeWeight(4);
			mathArrow(ro.x, ro.y, p.x, p.y, 0.1f);
			//レイ円
			stroke(16 / 255.f, 120 / 255.f, 151 / 255.f);
			strokeWeight(3);
			mathCircle(p.x, p.y, r*2);
		}
		//text infomation
		fontRectModeCenter();
		fontSize(30);
		fontColor(1, 1, 1);
		fontRectModeCorner();
		fontColor(0.8f, 0.8f, 0);
		print("t=%.2f",t);
		debugPrint();
		//cusor
		imageColor(1, 1, 1, 0.5f);
		cursor();
		endMsaaRender();
	}
}
#endif

