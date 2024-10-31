#include"graphic.h"
#include"model.h"

//Grobal variables
//���_�o�b�t�@
UINT NumVertices = 0;
ComPtr<ID3D12Resource>   VertexBuffer = nullptr;
D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
//���_�C���f�b�N�X�o�b�t�@
UINT NumIndices = 0;
ComPtr<ID3D12Resource>  IndexBuffer = nullptr;
D3D12_INDEX_BUFFER_VIEW	IndexBufferView;
//�R���X�^���g�o�b�t�@
struct CONST_BUF0 {
	XMMATRIX worldViewProj;
	XMMATRIX world;
	XMFLOAT4 lightPos;
};
struct CONST_BUF1 {
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
};
CONST_BUF0* CB0 = nullptr;
CONST_BUF1* CB1 = nullptr;
ComPtr<ID3D12Resource> ConstBuffer0 = nullptr;
ComPtr<ID3D12Resource> ConstBuffer1 = nullptr;
//�e�N�X�`���o�b�t�@
ComPtr<ID3D12Resource> TextureBuffer = nullptr;
//�f�B�X�N���v�^�q�[�v
ComPtr<ID3D12DescriptorHeap> CbvTbvHeap = nullptr;
UINT CbvTbvIncSize = 0;

//Entry point
INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"Lambert Pixel", 1280, 720);

	HRESULT Hr;

	//���\�[�X������
	{
		//���_�o�b�t�@
		{
			//�f�[�^�T�C�Y�����߂Ă���
			UINT sizeInBytes = sizeof(Vertices);
			UINT strideInBytes = sizeof(float) * NumVertexElements;
			NumVertices = sizeInBytes / strideInBytes;
			//�o�b�t�@������
			Hr = createBuffer(sizeInBytes, VertexBuffer);
			assert(SUCCEEDED(Hr));
			//�o�b�t�@�Ƀf�[�^������
			Hr = updateBuffer(Vertices, sizeInBytes, VertexBuffer);
			assert(SUCCEEDED(Hr));
			//�o�b�t�@�r���[������
			createVertexBufferView(VertexBuffer, sizeInBytes, strideInBytes, VertexBufferView);
		}
		//���_�C���f�b�N�X�o�b�t�@
		{
			//�f�[�^�T�C�Y�����߂Ă���
			UINT sizeInBytes = sizeof(Indices);
			NumIndices =  sizeInBytes / sizeof(UINT16);
			//�o�b�t�@������
			Hr = createBuffer(sizeInBytes, IndexBuffer);
			assert(SUCCEEDED(Hr));
			//�o�b�t�@�Ƀf�[�^������
			Hr = updateBuffer(Indices, sizeInBytes, IndexBuffer);
			assert(SUCCEEDED(Hr));
			//�C���f�b�N�X�o�b�t�@�r���[������
			createIndexBufferView(IndexBuffer, sizeInBytes, IndexBufferView);
		}
		//�R���X�^���g�o�b�t�@�O
		{
			//�o�b�t�@������
			Hr = createBuffer(256, ConstBuffer0);
			assert(SUCCEEDED(Hr));
			//�}�b�v���Ă���
			Hr = mapBuffer(ConstBuffer0, (void**)&CB0);
			assert(SUCCEEDED(Hr));
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
			CB1->ambient = { Ambient[0],Ambient[1],Ambient[2],Ambient[3] };
			CB1->diffuse = { Diffuse[0],Diffuse[1],Diffuse[2],Diffuse[3] };
		}
		//�e�N�X�`���o�b�t�@
		{
			Hr = createTextureBuffer(TextureFilename, TextureBuffer);
			assert(SUCCEEDED(Hr));
		}
		//�f�B�X�N���v�^�q�[�v
		{
			//�f�B�X�N���v�^(�r���[)�R���̃q�[�v������
			Hr = createDescriptorHeap(3, CbvTbvHeap);
			assert(SUCCEEDED(Hr));
			CbvTbvIncSize = cbvTbvIncSize();
			//�P�߂̃f�B�X�N���v�^(�r���[)���q�[�v�ɂ���
			auto hCbvTbvHeap = CbvTbvHeap->GetCPUDescriptorHandleForHeapStart();
			createConstantBufferView(ConstBuffer0, hCbvTbvHeap);
			//�Q�߂̃f�B�X�N���v�^(�r���[)���q�[�v�ɂ���
			hCbvTbvHeap.ptr += CbvTbvIncSize;
			createConstantBufferView(ConstBuffer1, hCbvTbvHeap);
			//�R�߂̃f�B�X�N���v�^(�r���[)���q�[�v�ɂ���
			hCbvTbvHeap.ptr += CbvTbvIncSize;
			createTextureBufferView(TextureBuffer, hCbvTbvHeap);
		}
	}
	
	//�`��p�ɃN���[�����Ă���
	ComPtr<ID3D12GraphicsCommandList>& CommandList = commandList();

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
		XMFLOAT3 eye = { 0, 0, -2 }, focus = { 0, 0, 0 }, up = { 0, 1, 0 };
		XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&up));
		//�v���W�F�N�V�����}�g���b�N�X
		XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect(), 1.0f, 10.0f);

		CB0->worldViewProj = world * view * proj;
		CB0->world = world;
		CB0->lightPos = { cosf(r),0,-0.3f,0 };

		//�`��------------------------------------------------------------------
		//�o�b�N�o�b�t�@�N���A
		clear(ClearColor);
		//���_���Z�b�g
		CommandList->IASetVertexBuffers(0, 1, &VertexBufferView);
		CommandList->IASetIndexBuffer(&IndexBufferView);
		//�f�B�X�N���v�^�q�[�v���f�o�t�ɃZ�b�g
		CommandList->SetDescriptorHeaps(1, CbvTbvHeap.GetAddressOf());
		//�f�B�X�N���v�^�q�[�v���f�B�X�N���v�^�e�[�u���ɃZ�b�g
		auto hCbvTbvHeap = CbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
		CommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);
		//�`��
		CommandList->DrawIndexedInstanced(NumIndices, 1, 0, 0, 0);
		//�o�b�N�o�b�t�@�\��
		present();
	}
	
	//��n��
	{
		waitGPU();
		closeEventHandle();
		ConstBuffer1->Unmap(0, nullptr);
		ConstBuffer0->Unmap(0, nullptr);
	}
}
