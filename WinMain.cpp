//text, print
#if 1
#include"graphic.h"
INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"rect", 500, 500);
	setClearColor(1, 0.8f, 0);

	//最初に、必要な数のコンスタント・テクスチャ用ディスクリプタヒープをつくる
	int numDescriptors = 2;
	int numShapes = 60;
	createDescriptorHeap(numDescriptors * numShapes);//この数を超えるとぶっ止まります

	fontSize(30); fontColor(1, 1, 1);
	
	createWhiteTexture();
	float rad = 0;
	while (!quit())
	{
		beginRender();//rootSignatureを変更してます！

		rad += 0.01f;
		for (int y = 0; y < 5; ++y) {
			for (int x = 0; x < 5; ++x) {
				if ((x + y) % 2)
					fill(0, 0.4f, 0);
				else
					fill(0.f, 0.f, 0.f);
				rect(100.0f * x + 50, 100.0f * y + 50, 100, 100, rad);
			}
		}

		print("rad:%.2f", rad);

		endRender();
	}

	waitGPU();
	closeEventHandle();
}
#endif
//フォントフェイスを変える１
#if 0
#include"graphic.h"
INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"Love", 1280, 720);

	//最初に必要な数のコンスタント・テクスチャ用ディスクリプタヒープをつくる
	int numDescriptors = 2;
	int numFonts = 120;
	createDescriptorHeap(numDescriptors * numFonts);

	setPrintInitX(330);
	fontSize(100);
	while (!quit()) {
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
	}
	waitGPU();
	closeEventHandle();
}
#endif
//フォントフェイスを変える２
#if 0
#include"graphic.h"
INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"Love", 1280, 720);
	
	//最初に必要な数のコンスタント・テクスチャ用ディスクリプタヒープをつくる
	int numDescriptors = 2;
	int numFonts = 30;
	createDescriptorHeap(numDescriptors * numFonts);

	while (!quit()) {
		beginRender();

		fontFace("HGP明朝E", JP);
		fontSize(150);
		fontColor(1, 1, 1);
		print("エヴァンゲリオン");
		print("時に、西暦%d年", 2057);
		print("使徒、襲来");
		fontFace("HG丸ｺﾞｼｯｸM-Pro", JP);
		text("綾波レイ", 0, 150*3+20);
		
		endRender();
	}
	waitGPU();
	closeEventHandle();
}
#endif
//インストールされていないフォントを使う
#if 0
#include<memory>
#include"graphic.h"
INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"Love", 1280, 720);

	//最初に必要な数のコンスタント・テクスチャ用ディスクリプタヒープをつくる
	int numDescriptors = 2;
	int numFonts = 30;
	createDescriptorHeap(numDescriptors * numFonts);

	//インストールされていないプロジェクトフォルダ内に用意したフォントが使える（ループ中にいれてはいけない）
	USER_FONT uf("PixelMplus12-Bold.ttf");

	fontFace("PixelMplus12", JP);
	fontSize(70);

	//文字列（全角文字前提）
	const char* src{ "白魔導士はエリクサーをなくして死んだ" };
	size_t len = strlen(src);
	auto disp = std::make_unique<char[]>(len + 1);//表示する文字だけここにコピーする
	len /= 2;//全角文字数にする
	int dispCnt = 0;//表示全角文字数
	int frameCnt = 0;//フレームカウント
	int interval = 10;//フレーム間隔
	int rest = 80;//停止フレーム数
	while (!quit()) {
		dispCnt = ++frameCnt / interval;
		if (dispCnt <= len) {
			//コピー
			int i;
			for (i = 0; i < dispCnt * 2; i++) {
				disp[i] = src[i];
			}
			disp[i] = '\0';
		}
		else if (frameCnt > len * interval + rest) {
			//リセット
			frameCnt = 0;
			dispCnt = 0;
		}

		beginRender();
		text(disp.get(), 10, 10);
		endRender();
	}
	waitGPU();
	closeEventHandle();
}
#endif
