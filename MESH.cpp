#include "MESH.h"
#include "model.h"

MESH::MESH()
{
}

MESH::~MESH()
{
	Parts.constBuffer1->Unmap(0, nullptr);
	Parts.constBuffer0->Unmap(0, nullptr);
}

void MESH::create()
{
	//���_�o�b�t�@
	{
		//�f�[�^�T�C�Y�����߂Ă���
		UINT sizeInBytes = sizeof(Vertices);
		UINT strideInBytes = sizeof(float) * NumVertexElements;
		Parts.numVertices = sizeInBytes / strideInBytes;
		//�o�b�t�@������
		Hr = createBuffer(sizeInBytes, Parts.vertexBuffer);
		assert(SUCCEEDED(Hr));
		//�o�b�t�@�Ƀf�[�^������
		Hr = updateBuffer(Vertices, sizeInBytes, Parts.vertexBuffer);
		assert(SUCCEEDED(Hr));
		//�o�b�t�@�r���[������
		createVertexBufferView(Parts.vertexBuffer, sizeInBytes, strideInBytes, Parts.vertexBufferView);
	}
	//���_�C���f�b�N�X�o�b�t�@
	{
		//�f�[�^�T�C�Y�����߂Ă���
		UINT sizeInBytes = sizeof(Indices);
		Parts.numIndices = sizeInBytes / sizeof(UINT16);
		//�o�b�t�@������
		Hr = createBuffer(sizeInBytes, Parts.indexBuffer);
		assert(SUCCEEDED(Hr));
		//�o�b�t�@�Ƀf�[�^������
		Hr = updateBuffer(Indices, sizeInBytes, Parts.indexBuffer);
		assert(SUCCEEDED(Hr));
		//�C���f�b�N�X�o�b�t�@�r���[������
		createIndexBufferView(Parts.indexBuffer, sizeInBytes, Parts.indexBufferView);
	}
	//�R���X�^���g�o�b�t�@�O
	{
		//�o�b�t�@������
		Hr = createBuffer(256, Parts.constBuffer0);
		assert(SUCCEEDED(Hr));
		//�}�b�v���Ă���
		Hr = mapBuffer(Parts.constBuffer0, (void**)&Parts.cb0);
		assert(SUCCEEDED(Hr));
	}
	//�R���X�^���g�o�b�t�@�P
	{
		//�o�b�t�@������
		Hr = createBuffer(256, Parts.constBuffer1);
		assert(SUCCEEDED(Hr));
		//�}�b�v���Ă���
		Hr = mapBuffer(Parts.constBuffer1, (void**)&Parts.cb1);
		assert(SUCCEEDED(Hr));
		//�f�[�^������
		Parts.cb1->ambient = { Ambient[0],Ambient[1],Ambient[2],Ambient[3] };
		Parts.cb1->diffuse = { Diffuse[0],Diffuse[1],Diffuse[2],Diffuse[3] };
	}
	//�e�N�X�`���o�b�t�@
	{
		Hr = createTextureBuffer(TextureFilename, Parts.textureBuffer);
		assert(SUCCEEDED(Hr));
	}
	//�f�B�X�N���v�^�q�[�v
	{
		//�f�B�X�N���v�^(�r���[)�R���̃q�[�v������
		Hr = createDescriptorHeap(3, Parts.cbvTbvHeap);
		assert(SUCCEEDED(Hr));
		CbvTbvIncSize = cbvTbvIncSize();
		//�P�߂̃f�B�X�N���v�^(�r���[)���q�[�v�ɂ���
		auto hCbvTbvHeap = Parts.cbvTbvHeap->GetCPUDescriptorHandleForHeapStart();
		createConstantBufferView(Parts.constBuffer0, hCbvTbvHeap);
		//�Q�߂̃f�B�X�N���v�^(�r���[)���q�[�v�ɂ���
		hCbvTbvHeap.ptr += CbvTbvIncSize;
		createConstantBufferView(Parts.constBuffer1, hCbvTbvHeap);
		//�R�߂̃f�B�X�N���v�^(�r���[)���q�[�v�ɂ���
		hCbvTbvHeap.ptr += CbvTbvIncSize;
		createTextureBufferView(Parts.textureBuffer, hCbvTbvHeap);
	}
}

void MESH::update(XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj, XMFLOAT4& lightPos)
{
	Parts.cb0->worldViewProj = world * view * proj;
	Parts.cb0->world = world;
	Parts.cb0->lightPos = lightPos;
}

void MESH::draw()
{
	//���_���Z�b�g
	CommandList->IASetVertexBuffers(0, 1, &Parts.vertexBufferView);
	CommandList->IASetIndexBuffer(&Parts.indexBufferView);
	//�f�B�X�N���v�^�q�[�v���f�o�t�ɃZ�b�g
	CommandList->SetDescriptorHeaps(1, Parts.cbvTbvHeap.GetAddressOf());
	//�f�B�X�N���v�^�q�[�v���f�B�X�N���v�^�e�[�u���ɃZ�b�g
	auto hCbvTbvHeap = Parts.cbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
	CommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);
	//�`��
	CommandList->DrawIndexedInstanced(Parts.numIndices, 1, 0, 0, 0);
}
