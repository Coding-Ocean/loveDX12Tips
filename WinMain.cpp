//text, print
#if 1
#include"graphic.h"
#include<sstream>
INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"Font", 1280, 720);
	setClearColor(0.f, 0.f, 0.f);

	int numFonts = 60;

	//�ŏ��ɕK�v�Ȑ��̃R���X�^���g�E�e�N�X�`���p�f�B�X�N���v�^�q�[�v������
	HRESULT hr = createDescriptorHeap(2 * numFonts);
	assert(SUCCEEDED(hr));
	//===
	//�t�H���g�p�̃R���X�^���g�o�b�t�@��p�ӂ��Ă����K�v������܂��B
	createFontConstants(numFonts);

	//���C�����[�v
	while (!quit())
	{
		beginRender();//rootSignature��ύX���Ă܂��I

		static int num = 0;
		static int cnt = 0;
		if (++cnt % 20 == 0)num++;
		std::ostringstream ostr;
		ostr << "�߉�" << num;

		float r, g, b, a;
		r = 1, g = 51.0f / 255, b = 153.0f / 255, a = 1;

		//text ���W�w��
		text(ostr.str().c_str(), 300, 200, r, g, b, a);

		//print �������s
		printColor(1, 1, 0);
		print("num=%d", num);

		printColor(1, 0, 0);
		print("�T�؍�%d", 46);

		printColor(0, 1, 0);
		print("�`�j�a%d", 48);

		endRender();
	}

	waitGPU();
	closeEventHandle();
}
#endif
//�t�H���g�t�F�C�X��ς���P
#if 0
#include"graphic.h"
INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"Love", 1280, 720);
	setClearColor(0.f, 0.f, 0.f);

	int numFonts = 15*7;

	//�ŏ��ɕK�v�Ȑ��̃R���X�^���g�E�e�N�X�`���p�f�B�X�N���v�^�q�[�v������
	HRESULT hr = createDescriptorHeap(2 * numFonts);
	assert(SUCCEEDED(hr));
	//===
	//�t�H���g�p�̃R���X�^���g�o�b�t�@��p�ӂ��Ă����K�v������܂��B
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
//�t�H���g�t�F�C�X��ς���Q
#if 0
#include"graphic.h"
INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"Love", 1280, 720);
	setClearColor(0.f, 0.f, 0.f);
	
	int numFonts = 30;

	//�ŏ��ɕK�v�Ȑ��̃R���X�^���g�E�e�N�X�`���p�f�B�X�N���v�^�q�[�v������
	HRESULT hr = createDescriptorHeap(2 * numFonts);
	assert(SUCCEEDED(hr));
	//===
	//�t�H���g�p�̃R���X�^���g�o�b�t�@��p�ӂ��Ă����K�v������܂��B
	createFontConstants(numFonts);

	while (!quit()) {
		beginRender();
		fontFace("HGP����E", JP);
		fontSize(150);
		printColor(1, 1, 1);
		print("�G���@���Q���I��");
		print("���ɁA����%d�N", 2057);
		print("�g�k�A�P��");

		//fontSize(150);
		fontFace("HG�ۺ޼��M-Pro", JP);
		text("���g���C", 0, 150*3+20,1,1,1,1);
		endRender();
	}
	waitGPU();
	closeEventHandle();
}
#endif
//�C���X�g�[������Ă��Ȃ��t�H���g���g��
#if 0
#include<memory>
#include"graphic.h"
INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"Love", 1280, 720);
	setClearColor(0.f, 0.f, 0.f);

	int numFonts = 30;

	//�ŏ��ɕK�v�Ȑ��̃R���X�^���g�E�e�N�X�`���p�f�B�X�N���v�^�q�[�v������
	HRESULT hr = createDescriptorHeap(2 * numFonts);
	assert(SUCCEEDED(hr));
	//===
	//�t�H���g�p�̃R���X�^���g�o�b�t�@��p�ӂ��Ă����K�v������܂��B
	createFontConstants(numFonts);

	//����ŃC���X�g�[������Ă��Ȃ��t�H���g���g����
	//�i���[�v���ɂ���Ă͂����Ȃ��j
	USER_FONT uf("PixelMplus12-Bold.ttf");

	fontFace("PixelMplus12", JP);
	fontSize(70);

	//������i�S�p�����O��j
	const char* src{ "�������m�̓G���N�T�[�𗎂Ƃ���" };
	size_t len = strlen(src);
	auto disp = std::make_unique<char[]>(len + 1);//�\�����镶�����������ɃR�s�[����
	len /= 2;//�S�p�������ɂ���
	int dispCnt = 0;//�\���S�p������
	int frameCnt = 0;//�t���[���J�E���g
	int interval = 10;//�t���[���Ԋu
	int rest = 80;//��~�t���[����
	while (!quit()) {
		dispCnt = ++frameCnt / interval;
		if (dispCnt <= len) {
			//�R�s�[
			int i;
			for (i = 0; i < dispCnt * 2; i++) {
				disp[i] = src[i];
			}
			disp[i] = '\0';
		}
		else if (frameCnt > len * interval + rest) {
			//���Z�b�g
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
