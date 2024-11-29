#include"graphic.h"
#include"model.h"

//���b�V���f�[�^
//�@���_�o�b�t�@
ComPtr<ID3D12Resource>   VertexBuffer = nullptr;
D3D12_VERTEX_BUFFER_VIEW Vbv;
//�@���_�C���f�b�N�X�o�b�t�@
ComPtr<ID3D12Resource>  IndexBuffer = nullptr;
D3D12_INDEX_BUFFER_VIEW	Ibv;
//�@�R���X�^���g�o�b�t�@�O
ComPtr<ID3D12Resource> ConstBuffer0 = nullptr;
CONST_BUF0* CB0 = nullptr;
//�@�R���X�^���g�o�b�t�@�P
ComPtr<ID3D12Resource> ConstBuffer1 = nullptr;
CONST_BUF1* CB1 = nullptr;
//�@�e�N�X�`���o�b�t�@
ComPtr<ID3D12Resource> TextureBuffer = nullptr;
//�@�f�B�X�N���v�^�C���f�b�N�X
UINT CbvTbvIdx = 0;

//Entry point
INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"Phong Pixel", 1280, 720);

	HRESULT Hr;

	//���b�V���ɕK�v�ȃR���X�^���g�o�b�t�@�r���[�A�e�N�X�`���o�b�t�@�r���[�̓��ꕨ��p�ӂ��Ă����B
	Hr = createDescriptorHeap(3);
	assert(SUCCEEDED(Hr));

	//���\�[�X������
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
			//�o�b�t�@������
			Hr = createBuffer(256, ConstBuffer0);
			assert(SUCCEEDED(Hr));
			//�}�b�v���Ă���
			Hr = mapBuffer(ConstBuffer0, (void**)&CB0);
			assert(SUCCEEDED(Hr));
			//�r���[������
			CbvTbvIdx = createConstantBufferView(ConstBuffer0);
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
			CB1->ambient = { ::Ambient[0],::Ambient[1],::Ambient[2],::Ambient[3] };
			CB1->diffuse = { ::Diffuse[0],::Diffuse[1],::Diffuse[2],::Diffuse[3] };
			CB1->specular = { ::Specular[0],::Specular[1],::Specular[2],::Specular[3] };
			//�ύX���Ȃ��̂ŃA���}�b�v����
			unmapBuffer(ConstBuffer1);
			//�r���[������
			createConstantBufferView(ConstBuffer1);
		}
		//�e�N�X�`���o�b�t�@
		{
			//�摜�t�@�C����ǂݍ���ŁA�o�b�t�@������A�摜�f�[�^�������
			Hr = createTextureBuffer(TextureFilename, TextureBuffer);
			assert(SUCCEEDED(Hr));
			//�r���[������
			createTextureBufferView(TextureBuffer);
		}
	}

	//���C�����[�v
	while (!quit())
	{
		//�X�V------------------------------------------------------------------
		//��]�p���W�A��
		static float r = 0;
		r += 0.01f;
		//���[���h�}�g���b�N�X
		XMMATRIX world = XMMatrixIdentity();
		//�r���[�}�g���b�N�X
		XMFLOAT3 eye = { 0, 0, -2.0f }, focus = { 0, 0, 0 }, up = { 0, 1, 0 };
		XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&up));
		//�v���W�F�N�V�����}�g���b�N�X
		XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect(), 1.0f, 10.0f);

		CB0->lightPos = { cosf(r), 0, powf(cosf(r / 6),2)*-1.5f-0.05f, 0 };
		CB0->eyePos = { eye.x,eye.y,eye.z,0 };
		CB0->worldViewProj = world * view * proj;
		CB0->world = world;

		//�`��------------------------------------------------------------------
		beginRender();
		drawMesh(Vbv, Ibv, CbvTbvIdx);
		endRender();
	}
	
	//��n��
	{
		waitGPU();
		closeEventHandle();
		unmapBuffer(ConstBuffer0);
	}
}
