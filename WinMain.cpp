#include"graphic.h"

class IMAGE
{
	//�R���X�^���g�o�b�t�@0
	ComPtr<ID3D12Resource> ConstBuffer0 = nullptr;
	CONST_BUF0* CB0 = nullptr;
	//�e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> TextureBuffer = nullptr;
	int TextureWidth=0, TextureHeight=0;
	//�f�B�X�N���v�^�C���f�b�N�X�igraphic.cpp�ɂ���CbvTbvHeap��̃C���f�b�N�X�j
	UINT CbvIdx = 0;
	UINT TbvIdx = 0;
	//�f�[�^
	float Px = 0, Py = 0, Rad = 0, Sx = 1, Sy = 1, R = 1, G = 1, B = 1, A = 1;
	int CornerFlag=0;
public:
	IMAGE()
	{
	}
	~IMAGE()
	{
		unmapBuffer(ConstBuffer0);
	}
	void create(const char* textureFilename)
	{
		//�R���X�^���g�o�b�t�@�O
		{
			createBuffer(alignedSize(sizeof(CONST_BUF0)), ConstBuffer0);
			mapBuffer(ConstBuffer0, (void**)&CB0);
			CbvIdx = createConstantBufferView(ConstBuffer0);
		}
		//�e�N�X�`���o�b�t�@
		{
			TbvIdx = createTextureBufferAndView(textureFilename, TextureBuffer, &TextureWidth, &TextureHeight);
		}
	}
	void draw()
	{
		//2D�p�}�g���b�N�X
		float chw = clientWidth() / 2;//client half width
		float chh = clientHeight() / 2;//client half height
		XMMATRIX world;
		if (CornerFlag) {
			world = XMMatrixTranslation(0.5f, -0.5f, 0);//Px,Py�̈ʒu�ɉ摜�̍��オ����
		}
		else {
			world = XMMatrixIdentity();
		}
		world *=
			XMMatrixScaling(Sx * TextureWidth, Sy * TextureHeight, 1)
			* XMMatrixRotationZ(Rad)
			* XMMatrixScaling(1.0f / chw, 1.0f / chh, 1)
			* XMMatrixTranslation(Px / chw - 1, Py / -chh + 1, 0);
		CB0->worldViewProj = world;
		CB0->diffuse = { R,G,B,A };
		//�`��
		drawImage(CbvIdx,TbvIdx);
	}
	void rectModeCorner()
	{
		CornerFlag = 1;
	}
	void rectModeCenter()
	{
		CornerFlag = 0;
	}
	//Setter
	void px(float px) { Px = px; }
	void py(float py) { Py = py; }
	void rad(float rad) { Rad = rad; }
	void r(float r) { R = r; }
	void g(float g) { G = g; }
	void b(float b) { B = b; }
	void a(float a) { A = a; }
	//Getter
	float px() { return Px; }
	float py() { return Py; }
	float width() { return TextureWidth; }
	float height() { return TextureHeight; }
};

//Entry point
INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"2DGraphics", 1280, 720);
	setClearColor(0.25f, 0.5f, 0.9f);

	const int numImages = 4;

	//�ŏ��ɕK�v�Ȑ��̃R���X�^���g�E�e�N�X�`���p�f�B�X�N���v�^�q�[�v������
	{
		HRESULT hr = createDescriptorHeap(3 * numImages+100);
		assert(SUCCEEDED(hr));
	}

	IMAGE images[numImages];
	images[0].create("assets\\image.png");
	images[1].create("assets\\image.png");//�����摜���w�肵�Ă����ʂɃo�b�t�@�����܂��ʁB
	images[2].create("assets\\image.png");
	images[3].create("assets\\image.png");

	float clientHalfHeight = clientHeight() / 2;
	images[0].py(clientHalfHeight);
	images[1].py(clientHalfHeight);
	images[2].py(clientHalfHeight);

	images[3].rectModeCorner();

	//���C�����[�v
	while (!quit())
	{
		//�X�V------------------------------------------------------------------
		static float px = images[0].width()/2, py = 0, rad = 0;
		static float vx = 5;
		
		rad += 0.01f;
		px += vx;

		//diffuse color
		float col = sinf(rad) < 0 ? 0 : 1;
		float r = 1 - col;
		float g = col;

		images[0].px(px);

		images[1].px(clientWidth() - px);
		
		images[2].px(clientWidth() / 2);
		images[2].rad(sinf(rad));
		images[2].r(r);
		images[2].g(g);

		//�L�������m����������
		float a = images[0].px() - images[1].px();
		float c = images[0].width();
		if (a*a < c*c) vx *= -1;
		//�ǂƓ�������
		if (px<c/2 || px>clientWidth()-c/2 ) vx *= -1;

		//�`��------------------------------------------------------------------
		beginRender();//rootSignature��ύX���Ă܂��I
		for (auto& image : images) {
			//image.draw();
		}

		text("����������", 0, 0, 1, 0, 1, 1);
		
		endRender();
	}
	
	//��n��
	{
		waitGPU();
		closeEventHandle();
	}
}
