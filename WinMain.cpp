#include"graphic.h"
#include"model.h"

//���b�V��
//�@���_�o�b�t�@
ComPtr<ID3D12Resource>   VertexBuffer = nullptr;
D3D12_VERTEX_BUFFER_VIEW Vbv;
//�@���_�C���f�b�N�X�o�b�t�@
ComPtr<ID3D12Resource>  IndexBuffer = nullptr;
D3D12_INDEX_BUFFER_VIEW	Ibv;
//�@�R���X�^���g�o�b�t�@
ComPtr<ID3D12Resource> ConstBuffer0 = nullptr;
ComPtr<ID3D12Resource> ConstBuffer1 = nullptr;
CONST_BUF0* CB0 = nullptr;
CONST_BUF1* CB1 = nullptr;
//�@�e�N�X�`���o�b�t�@
ComPtr<ID3D12Resource> TextureBuffer = nullptr;
UINT CbvTbvIdx = 0;

//Entry point
INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"Deferred Rendering", 1280, 720);

	HRESULT Hr;

	//���\�[�X������
	{
		//�R���X�^���g�E�e�N�X�`���n�f�B�X�N���v�^�q�[�v
		{
			//���b�V���p3*1 �{ �f�B�t�@�[�h�p2*2
			Hr = createDescriptorHeap(3 + 4);
			assert(SUCCEEDED(Hr));
		}

		//���b�V��������
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
			//�o�b�t�@�r���[������
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
			//�C���f�b�N�X�o�b�t�@�r���[������
			createIndexBufferView(IndexBuffer, sizeInBytes, Ibv);
		}
		//�@�R���X�^���g�o�b�t�@�O
		{
			//�o�b�t�@������
			Hr = createBuffer(256, ConstBuffer0);
			assert(SUCCEEDED(Hr));
			//�}�b�v���Ă���
			Hr = mapBuffer(ConstBuffer0, (void**)&CB0);
			assert(SUCCEEDED(Hr));
			//�r���[
			CbvTbvIdx = createConstantBufferView(ConstBuffer0);
		}
		//�@�R���X�^���g�o�b�t�@�P
		{
			//�o�b�t�@������
			Hr = createBuffer(256, ConstBuffer1);
			assert(SUCCEEDED(Hr));
			//�}�b�v���Ă���
			Hr = mapBuffer(ConstBuffer1, (void**)&CB1);
			assert(SUCCEEDED(Hr));
			//�f�[�^������
			CB1->ambient  = { ::Ambient[0],::Ambient[1],::Ambient[2],::Ambient[3] };
			CB1->diffuse  = { ::Diffuse[0],::Diffuse[1],::Diffuse[2],::Diffuse[3] };
			CB1->specular = { ::Specular[0],::Specular[1],::Specular[2],::Specular[3] };
			//�r���[
			createConstantBufferView(ConstBuffer1);
		}
		//�@�e�N�X�`���o�b�t�@
		{
			Hr = createTextureBuffer(::TextureFilename, TextureBuffer);
			assert(SUCCEEDED(Hr));
			createTextureBufferView(TextureBuffer);
		}

		//�f�B�t�@�[�h�����_�����O�̏���
		createPipelinesAndResourcesForDeferred();
	}

	//���C�����[�v
	while (!quit())
	{
		//�X�V------------------------------------------------------------------
		//��]�p���W�A��
		static float r = 0;
		r += 0.01f;
		//���[���h�}�g���b�N�X
		XMMATRIX world = XMMatrixRotationY(sinf(r));
		//�r���[�}�g���b�N�X
		XMFLOAT3 eye = { 0, 0, -2 }, focus = { 0, 0, 0 }, up = { 0, 1, 0 };
		XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&up));
		//�v���W�F�N�V�����}�g���b�N�X
		XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect(), 1.0f, 10.0f);
		//�R���X�^���g�o�b�t�@�O�X�V
		CB0->lightPos = { 0, 0, -1, 0 };
		CB0->eyePos = { eye.x, eye.y, eye.z, 0 };
		CB0->worldViewProj = world * view * proj;
		CB0->world = world;

		//�`��------------------------------------------------------------------
#if 0
		setClearColor(0.2f, 0.2f, 0.2f);
		beginRender();
		drawMesh(Vbv, Ibv, CbvTbvIdx);
		endRender();
#else
		beginDeferredRender();
		drawMesh(Vbv, Ibv, CbvTbvIdx);
		endDeferredRender();
#endif
	}
	
	//��n��
	{
		waitGPU();
		closeEventHandle();
		unmapDeferredRenderConstant();
		unmapBuffer(ConstBuffer0);
		unmapBuffer(ConstBuffer1);
	}
}
