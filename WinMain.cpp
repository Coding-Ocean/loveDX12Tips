#include"graphic.h"
#include"model.h"

//���_�o�b�t�@
ComPtr<ID3D12Resource>   VertexBuffer = nullptr;
D3D12_VERTEX_BUFFER_VIEW Vbv;
//���_�C���f�b�N�X�o�b�t�@
ComPtr<ID3D12Resource>  IndexBuffer = nullptr;
D3D12_INDEX_BUFFER_VIEW	Ibv;
//�R���X�^���g�o�b�t�@
ComPtr<ID3D12Resource> ConstBuffer0 = nullptr;
ComPtr<ID3D12Resource> ConstBuffer1 = nullptr;
CONST_BUF0* CB0 = nullptr;
CONST_BUF1* CB1 = nullptr;
//�e�N�X�`���o�b�t�@
ComPtr<ID3D12Resource> TextureBuffer = nullptr;
//graphic.cpp�ɂ���CbvTbvHeap��̃C���f�b�N�X
UINT CbvTbvIdx;

//Entry point
INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"Graphic Functions2", 1280, 720);
	setClearColor(0.25f, 0.5f, 0.9f);

	HRESULT Hr;

	//���\�[�X������
	{
		//�ŏ��ɕK�v�Ȑ��̃R���X�^���g�o�b�t�@�n�f�B�X�N���v�^�q�[�v�������Ă���
		{
			Hr = createDescriptorHeap(3);
			assert(SUCCEEDED(Hr));
		}

		//�P�̃��b�V���������Ă���
		//�@���_�o�b�t�@
		{
			//�f�[�^�T�C�Y�����߂Ă���
			UINT sizeInBytes = sizeof(::Vertices);
			UINT strideInBytes = sizeof(float) * ::NumVertexElements;
			//�o�b�t�@������
			Hr = createBuffer(sizeInBytes, VertexBuffer);
			assert(SUCCEEDED(Hr));
			//�o�b�t�@�Ƀf�[�^������
			Hr = updateBuffer(::Vertices, sizeInBytes, VertexBuffer);
			assert(SUCCEEDED(Hr));
			//�r���[������
			createVertexBufferView(VertexBuffer, sizeInBytes, strideInBytes, Vbv);
		}
		//�@���_�C���f�b�N�X�o�b�t�@
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
		//�@�R���X�^���g�o�b�t�@�O
		{
			//�o�b�t�@������
			Hr = createBuffer(alignedSize(sizeof(CB0)), ConstBuffer0);
			assert(SUCCEEDED(Hr));
			//�}�b�v���Ă���
			Hr = mapBuffer(ConstBuffer0, (void**)&CB0);
			assert(SUCCEEDED(Hr));
			//�r���[�����A���̃C���f�b�N�X���擾���Ă���
			CbvTbvIdx = createConstantBufferView(ConstBuffer0);
		}
		//�@�R���X�^���g�o�b�t�@�P
		{
			//�o�b�t�@������
			Hr = createBuffer(alignedSize(sizeof(CB1)), ConstBuffer1);
			assert(SUCCEEDED(Hr));
			//�}�b�v���Ă���
			Hr = mapBuffer(ConstBuffer1, (void**)&CB1);
			assert(SUCCEEDED(Hr));
			//�f�[�^������
			CB1->diffuse = { ::Diffuse[0],::Diffuse[1],::Diffuse[2],::Diffuse[3] };
			//�r���[������
			createConstantBufferView(ConstBuffer1);
		}
		//�@�e�N�X�`���o�b�t�@
		{
			//�o�b�t�@������
			Hr = createTextureBuffer(::TextureFilename, TextureBuffer);
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
		XMMATRIX world = XMMatrixRotationY(r);
		//�r���[�}�g���b�N�X
		XMFLOAT3 eye = { 0, 0, -2 }, focus = { 0, 0, 0 }, up = { 0, 1, 0 };
		XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&up));
		//�v���W�F�N�V�����}�g���b�N�X
		XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect(), 1.0f, 10.0f);
		CB0->worldViewProj = world * view * proj;

		//�`��------------------------------------------------------------------
		beginDraw();
		drawMesh(Vbv, Ibv, CbvTbvIdx);
		endDraw();
	}
	
	//��n��
	{
		waitGPU();
		closeEventHandle();
		unmapBuffer(ConstBuffer1);
		unmapBuffer(ConstBuffer0);
	}
}
