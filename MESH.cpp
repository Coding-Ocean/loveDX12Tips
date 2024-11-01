#include "MESH.h"
#include "model.h"

MESH::MESH()
{
}

MESH::~MESH()
{
	Parts.ConstBuffer1->Unmap(0, nullptr);
	Parts.ConstBuffer0->Unmap(0, nullptr);
}

void MESH::create()
{
	//���_�o�b�t�@
	{
		//�f�[�^�T�C�Y�����߂Ă���
		UINT sizeInBytes = sizeof(Vertices);
		UINT strideInBytes = sizeof(float) * NumVertexElements;
		Parts.NumVertices = sizeInBytes / strideInBytes;
		//�o�b�t�@������
		Hr = createBuffer(sizeInBytes, Parts.VertexBuffer);
		assert(SUCCEEDED(Hr));
		//�o�b�t�@�Ƀf�[�^������
		Hr = updateBuffer(Vertices, sizeInBytes, Parts.VertexBuffer);
		assert(SUCCEEDED(Hr));
		//�o�b�t�@�r���[������
		createVertexBufferView(Parts.VertexBuffer, sizeInBytes, strideInBytes, Parts.VertexBufferView);
	}
	//���_�C���f�b�N�X�o�b�t�@
	{
		//�f�[�^�T�C�Y�����߂Ă���
		UINT sizeInBytes = sizeof(Indices);
		Parts.NumIndices = sizeInBytes / sizeof(UINT16);
		//�o�b�t�@������
		Hr = createBuffer(sizeInBytes, Parts.IndexBuffer);
		assert(SUCCEEDED(Hr));
		//�o�b�t�@�Ƀf�[�^������
		Hr = updateBuffer(Indices, sizeInBytes, Parts.IndexBuffer);
		assert(SUCCEEDED(Hr));
		//�C���f�b�N�X�o�b�t�@�r���[������
		createIndexBufferView(Parts.IndexBuffer, sizeInBytes, Parts.IndexBufferView);
	}
	//�R���X�^���g�o�b�t�@�O
	{
		//�o�b�t�@������
		Hr = createBuffer(256, Parts.ConstBuffer0);
		assert(SUCCEEDED(Hr));
		//�}�b�v���Ă���
		Hr = mapBuffer(Parts.ConstBuffer0, (void**)&Parts.CB0);
		assert(SUCCEEDED(Hr));
	}
	//�R���X�^���g�o�b�t�@�P
	{
		//�o�b�t�@������
		Hr = createBuffer(256, Parts.ConstBuffer1);
		assert(SUCCEEDED(Hr));
		//�}�b�v���Ă���
		Hr = mapBuffer(Parts.ConstBuffer1, (void**)&Parts.CB1);
		assert(SUCCEEDED(Hr));
		//�f�[�^������
		Parts.CB1->ambient = { Ambient[0],Ambient[1],Ambient[2],Ambient[3] };
		Parts.CB1->diffuse = { Diffuse[0],Diffuse[1],Diffuse[2],Diffuse[3] };
	}
	//�e�N�X�`���o�b�t�@
	{
		Hr = createTextureBuffer(TextureFilename, Parts.TextureBuffer);
		assert(SUCCEEDED(Hr));
	}
	//�f�B�X�N���v�^�q�[�v
	{
		//�f�B�X�N���v�^(�r���[)�R���̃q�[�v������
		Hr = createDescriptorHeap(3, Parts.CbvTbvHeap);
		assert(SUCCEEDED(Hr));
		CbvTbvIncSize = cbvTbvIncSize();
		//�P�߂̃f�B�X�N���v�^(�r���[)���q�[�v�ɂ���
		auto hCbvTbvHeap = Parts.CbvTbvHeap->GetCPUDescriptorHandleForHeapStart();
		createConstantBufferView(Parts.ConstBuffer0, hCbvTbvHeap);
		//�Q�߂̃f�B�X�N���v�^(�r���[)���q�[�v�ɂ���
		hCbvTbvHeap.ptr += CbvTbvIncSize;
		createConstantBufferView(Parts.ConstBuffer1, hCbvTbvHeap);
		//�R�߂̃f�B�X�N���v�^(�r���[)���q�[�v�ɂ���
		hCbvTbvHeap.ptr += CbvTbvIncSize;
		createTextureBufferView(Parts.TextureBuffer, hCbvTbvHeap);
	}
}

void MESH::update(XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj)
{
	Parts.CB0->worldViewProj = world * view * proj;
	Parts.CB0->world = world;
	Parts.CB0->lightPos = { 0,1,0,0 };
}

void MESH::draw()
{
	//���_���Z�b�g
	CommandList->IASetVertexBuffers(0, 1, &Parts.VertexBufferView);
	CommandList->IASetIndexBuffer(&Parts.IndexBufferView);
	//�f�B�X�N���v�^�q�[�v���f�o�t�ɃZ�b�g
	CommandList->SetDescriptorHeaps(1, Parts.CbvTbvHeap.GetAddressOf());
	//�f�B�X�N���v�^�q�[�v���f�B�X�N���v�^�e�[�u���ɃZ�b�g
	auto hCbvTbvHeap = Parts.CbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
	CommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);
	//�`��
	CommandList->DrawIndexedInstanced(Parts.NumIndices, 1, 0, 0, 0);
}
