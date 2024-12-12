//text, print
#if 1
#include"graphic.h"
#include<sstream>
INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"Font", 1280, 720);
	setClearColor(0.f, 0.f, 0.f);

	int numFonts = 60;

	//最初に必要な数のコンスタント・テクスチャ用ディスクリプタヒープをつくる
	HRESULT hr = createDescriptorHeap(2 * numFonts);
	assert(SUCCEEDED(hr));
	//===
	//フォント用のコンスタントバッファを用意しておく必要があります。
	createFontConstants(numFonts);

	//メインループ
	while (!quit())
	{
		beginRender();//rootSignatureを変更してます！

		static int num = 0;
		static int cnt = 0;
		if (++cnt % 20 == 0)num++;
		std::ostringstream ostr;
		ostr << "釈迦" << num;

		float r, g, b, a;
		r = 1, g = 51.0f / 255, b = 153.0f / 255, a = 1;

		//text 座標指定
		text(ostr.str().c_str(), 300, 200, r, g, b, a);

		//print 自動改行
		printColor(1, 1, 0);
		print("num=%d", num);

		printColor(1, 0, 0);
		print("乃木坂%d", 46);

		printColor(0, 1, 0);
		print("ＡＫＢ%d", 48);

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
	setClearColor(0.f, 0.f, 0.f);

	int numFonts = 15*7;

	//最初に必要な数のコンスタント・テクスチャ用ディスクリプタヒープをつくる
	HRESULT hr = createDescriptorHeap(2 * numFonts);
	assert(SUCCEEDED(hr));
	//===
	//フォント用のコンスタントバッファを用意しておく必要があります。
	createFontConstants(numFonts);

	setPrintInitX(330);
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
			fontSize(100);
			printColor(1, 1.0f / 6 * i, 1);
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
	setClearColor(0.f, 0.f, 0.f);
	
	int numFonts = 30;

	//最初に必要な数のコンスタント・テクスチャ用ディスクリプタヒープをつくる
	HRESULT hr = createDescriptorHeap(2 * numFonts);
	assert(SUCCEEDED(hr));
	//===
	//フォント用のコンスタントバッファを用意しておく必要があります。
	createFontConstants(numFonts);

	while (!quit()) {
		beginRender();
		fontFace("HGP明朝E", JP);
		fontSize(150);
		printColor(1, 1, 1);
		print("エヴァンゲリオン");
		print("時に、西暦%d年", 2057);
		print("使徒、襲来");

		//fontSize(150);
		fontFace("HG丸ｺﾞｼｯｸM-Pro", JP);
		text("綾波レイ", 0, 150*3+20,1,1,1,1);
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
	setClearColor(0.f, 0.f, 0.f);

	int numFonts = 30;

	//最初に必要な数のコンスタント・テクスチャ用ディスクリプタヒープをつくる
	HRESULT hr = createDescriptorHeap(2 * numFonts);
	assert(SUCCEEDED(hr));
	//===
	//フォント用のコンスタントバッファを用意しておく必要があります。
	createFontConstants(numFonts);

	//これでインストールされていないフォントが使える
	//（ループ中にいれてはいけない）
	USER_FONT uf("PixelMplus12-Bold.ttf");

	fontFace("PixelMplus12", JP);
	fontSize(70);

	//文字列（全角文字前提）
	const char* src{ "白魔導士はエリクサーを落とした" };
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
		text(disp.get(), 0, 0, 1,1,0,1);
		endRender();
	}
	waitGPU();
	closeEventHandle();
}
#endif
