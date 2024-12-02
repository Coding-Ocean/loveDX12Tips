#include<sstream>
#include"graphic.h"
#include"model.h"

//���b�V���f�[�^
//�@���_�o�b�t�@
ComPtr<ID3D12Resource>   VertexBuffer = nullptr;
D3D12_VERTEX_BUFFER_VIEW Vbv;
//�@���_�C���f�b�N�X�o�b�t�@
ComPtr<ID3D12Resource>  IndexBuffer = nullptr;
D3D12_INDEX_BUFFER_VIEW	Ibv;

//�@�R���X�^���g�o�b�t�@�O�i�s��j
constexpr UINT NumCharactor = 2;
ComPtr<ID3D12Resource> ConstBuffer0[NumCharactor];
CONST_BUF0* CB0[NumCharactor] = {};

//�@�R���X�^���g�o�b�t�@�P�i�F�j
ComPtr<ID3D12Resource> ConstBuffer1 = nullptr;
CONST_BUF1* CB1 = nullptr;

//�@�R���X�^���g�o�b�t�@�̐����p�ӂ��Ă���
constexpr UINT NumConstBuffers = NumCharactor + 1;

//�@�e�N�X�`���o�b�t�@
constexpr UINT NumTextureBuffers = 8;//�����̃o�b�t�@��p�ӂ���
ComPtr<ID3D12Resource> TextureBuffers[NumTextureBuffers];//�z��ɂ��܂�
//�@����Ńf�B�X�N���v�^�̏ꏊ���w��
UINT Cb0vIdx[2] = {};
UINT Cb1vIdx = 0;
UINT TbvIdxs[NumTextureBuffers] = {};//�C���f�b�N�X�z��
UINT TbvIdx = 0;//�C���f�b�N�X�z����w���C���f�b�N�X

//Entry point
INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"Texture Animation", 1280, 720);
	setClearColor(0.25f, 0.5f, 0.9f);

	HRESULT Hr;

	//�ŏ��ɕK�v�ȃR���X�^���g�o�b�t�@�r���[�A�e�N�X�`���o�b�t�@�r���[�̃q�[�v��p�ӂ��Ă���
	Hr = createDescriptorHeap(NumConstBuffers + NumTextureBuffers);
	assert(SUCCEEDED(Hr));

	//���\�[�X������
	{
		//���_�o�b�t�@
		{
			//�f�[�^�T�C�Y�����߂Ă���
			UINT sizeInBytes = sizeof(::Vertices);
			UINT strideInBytes = sizeof(float) * NumVertexElements;
			//�o�b�t�@������
			Hr = createBuffer(sizeInBytes, VertexBuffer);
			assert(SUCCEEDED(Hr));
			//�o�b�t�@�Ƀf�[�^������
			Hr = updateBuffer(::Vertices, sizeInBytes, VertexBuffer);
			assert(SUCCEEDED(Hr));
			//�r���[������
			createVertexBufferView(VertexBuffer, sizeInBytes, strideInBytes, Vbv);
		}
		//���_�C���f�b�N�X�o�b�t�@
		{
			//�f�[�^�T�C�Y�����߂Ă���
			UINT sizeInBytes = sizeof(::Indices);
			//�o�b�t�@������
			Hr = createBuffer(sizeInBytes, IndexBuffer);
			assert(SUCCEEDED(Hr));
			//�o�b�t�@�Ƀf�[�^������
			Hr = updateBuffer(::Indices, sizeInBytes, IndexBuffer);
			assert(SUCCEEDED(Hr));
			//�r���[������
			createIndexBufferView(IndexBuffer, sizeInBytes, Ibv);
		}
		//�R���X�^���g�o�b�t�@�O
		{
			for (UINT i = 0; i < NumCharactor; ++i) {
				//�o�b�t�@������
				Hr = createBuffer(256, ConstBuffer0[i]);
				assert(SUCCEEDED(Hr));
				//�}�b�v���Ă���
				Hr = mapBuffer(ConstBuffer0[i], (void**)&CB0[i]);
				assert(SUCCEEDED(Hr));
				//�r���[�������āA�C���f�b�N�X��������Ă���
				Cb0vIdx[0] = createConstantBufferView(ConstBuffer0[i]);
			}
		}
		//�R���X�^���g�o�b�t�@�P
		{
			//�o�b�t�@������
			Hr = createBuffer(256, ConstBuffer1);
			assert(SUCCEEDED(Hr));
			//�}�b�v���Ă���
			Hr = mapBuffer(ConstBuffer1, (void**)&CB1);
			assert(SUCCEEDED(Hr));
			//�f�[�^������
			CB1->diffuse = {::Diffuse[0],::Diffuse[1],::Diffuse[2],::Diffuse[3]};
			//�ύX���Ȃ��̂ŃA���}�b�v����
			unmapBuffer(ConstBuffer1);
			//�r���[������
			Cb1vIdx = createConstantBufferView(ConstBuffer1);
		}
		//�e�N�X�`���o�b�t�@
		{
			//�t�@�C����ǂݍ��݁A�e�N�X�`���o�b�t�@������
			for (int i = 0; i < NumTextureBuffers; ++i) {
				//�o�b�t�@������
				std::ostringstream filename;
				filename << "assets\\lady\\a" << i << ".png";
				createTextureBuffer(filename.str().c_str(), TextureBuffers[i]);
				//�r���[�������āA�C���f�b�N�X��������Ă���
				TbvIdxs[i] = createTextureBufferView(TextureBuffers[i]);
			}
		}
	}
	
	//���C�����[�v
	while (!quit())
	{
		//�X�V------------------------------------------------------------------
		//�r���[�}�g���b�N�X
		XMFLOAT3 eye = { 0, 0, -2.1f }, focus = { 0, 0, 0 }, up = { 0, 1, 0 };
		XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&up));
		//�v���W�F�N�V�����}�g���b�N�X
		XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect(), 1.0f, 10.0f);

		//���[���h�}�g���b�N�X
		//��]�p���W�A��
		static float r = 0;
		r += 0.01f;
		XMMATRIX world;

		world = XMMatrixRotationY(r) * XMMatrixTranslation(-sin(r), 0, -cos(r));
		CB0[0]->worldViewProj = world * view * proj;

		world = XMMatrixRotationY(-r-3.14f) * XMMatrixTranslation(-sin(-r), 0, -cos(-r));
		CB0[1]->worldViewProj = world * view * proj;

		//���Ԋu�Ńe�N�X�`���C���f�b�N�X���J�E���g�A�b�v����
		{
			static UINT count = 0;
			int interval = 7;
			//count��interval�Ŋ���؂ꂽ����TbvIdx���J�E���g�A�b�v����
			if (++count % interval == 0) {
				count = 0;
				++TbvIdx %= NumTextureBuffers;
			}
		}

		//�`��------------------------------------------------------------------
		beginRender();
		for (UINT i = 0; i < NumCharactor; ++i) {
			//�I�I�I���̊֐����g�p����ɂ̓��[�g�V�O�l�`���̕ύX���K�v�I�I�I
			drawMesh(Vbv, Ibv, Cb0vIdx[i], Cb1vIdx, TbvIdxs[TbvIdx]);
		}
		endRender();
	}
	
	//��n��
	{
		waitGPU();
		closeEventHandle();
		unmapBuffer(ConstBuffer0[0]);
		unmapBuffer(ConstBuffer0[1]);
	}
}
