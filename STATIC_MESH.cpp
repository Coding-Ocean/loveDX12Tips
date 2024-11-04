#include "STATIC_MESH.h"
#include "model.h"

STATIC_MESH::STATIC_MESH()
{
}

STATIC_MESH::~STATIC_MESH()
{
	Mesh.constBuffer1->Unmap(0, nullptr);
	Mesh.constBuffer0->Unmap(0, nullptr);
}

void STATIC_MESH::create()
{
	//���_�o�b�t�@
	{
		//�f�[�^�T�C�Y�����߂Ă���
		UINT sizeInBytes = sizeof(Vertices);
		UINT strideInBytes = sizeof(float) * NumVertexElements;
		Mesh.numVertices = sizeInBytes / strideInBytes;
		//�o�b�t�@������
		Hr = createBuffer(sizeInBytes, Mesh.vertexBuffer);
		assert(SUCCEEDED(Hr));
		//�o�b�t�@�Ƀf�[�^������
		Hr = updateBuffer(Vertices, sizeInBytes, Mesh.vertexBuffer);
		assert(SUCCEEDED(Hr));
		//�o�b�t�@�r���[������
		createVertexBufferView(Mesh.vertexBuffer, sizeInBytes, strideInBytes, Mesh.vertexBufferView);
	}
	//���_�C���f�b�N�X�o�b�t�@
	{
		//�f�[�^�T�C�Y�����߂Ă���
		UINT sizeInBytes = sizeof(Indices);
		Mesh.numIndices = sizeInBytes / sizeof(UINT16);
		//�o�b�t�@������
		Hr = createBuffer(sizeInBytes, Mesh.indexBuffer);
		assert(SUCCEEDED(Hr));
		//�o�b�t�@�Ƀf�[�^������
		Hr = updateBuffer(Indices, sizeInBytes, Mesh.indexBuffer);
		assert(SUCCEEDED(Hr));
		//�C���f�b�N�X�o�b�t�@�r���[������
		createIndexBufferView(Mesh.indexBuffer, sizeInBytes, Mesh.indexBufferView);
	}
	//�R���X�^���g�o�b�t�@�O
	{
		//�o�b�t�@������
		Hr = createBuffer(256, Mesh.constBuffer0);
		assert(SUCCEEDED(Hr));
		//�}�b�v���Ă���
		Hr = mapBuffer(Mesh.constBuffer0, (void**)&Mesh.cb0);
		assert(SUCCEEDED(Hr));
	}
	//�R���X�^���g�o�b�t�@�P
	{
		//�o�b�t�@������
		Hr = createBuffer(256, Mesh.constBuffer1);
		assert(SUCCEEDED(Hr));
		//�}�b�v���Ă���
		Hr = mapBuffer(Mesh.constBuffer1, (void**)&Mesh.cb1);
		assert(SUCCEEDED(Hr));
		//�f�[�^������
		Mesh.cb1->ambient = { Ambient[0],Ambient[1],Ambient[2],Ambient[3] };
		Mesh.cb1->diffuse = { Diffuse[0],Diffuse[1],Diffuse[2],Diffuse[3] };
	}
	//�e�N�X�`���o�b�t�@
	{
		Hr = createTextureBuffer(TextureFilename, Mesh.textureBuffer);
		assert(SUCCEEDED(Hr));
	}
	//�f�B�X�N���v�^�q�[�v
	{
		//�f�B�X�N���v�^(�r���[)�R���̃q�[�v������
		Hr = createDescriptorHeap(3, Mesh.cbvTbvHeap);
		assert(SUCCEEDED(Hr));
		//�P�߂̃f�B�X�N���v�^(�r���[)���q�[�v�ɂ���
		auto hCbvTbvHeap = Mesh.cbvTbvHeap->GetCPUDescriptorHandleForHeapStart();
		createConstantBufferView(Mesh.constBuffer0, hCbvTbvHeap);
		//�Q�߂̃f�B�X�N���v�^(�r���[)���q�[�v�ɂ���
		hCbvTbvHeap.ptr += CbvTbvIncSize;
		createConstantBufferView(Mesh.constBuffer1, hCbvTbvHeap);
		//�R�߂̃f�B�X�N���v�^(�r���[)���q�[�v�ɂ���
		hCbvTbvHeap.ptr += CbvTbvIncSize;
		createTextureBufferView(Mesh.textureBuffer, hCbvTbvHeap);
	}
}

void STATIC_MESH::update(XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj, XMFLOAT4& lightPos)
{
	Mesh.cb0->lightPos = lightPos;
	Mesh.cb0->viewProj = view * proj;
	Mesh.cb0->world = world;
}

void STATIC_MESH::draw()
{
	//���_���Z�b�g
	CommandList->IASetVertexBuffers(0, 1, &Mesh.vertexBufferView);
	CommandList->IASetIndexBuffer(&Mesh.indexBufferView);
	//�f�B�X�N���v�^�q�[�v���f�o�t�ɃZ�b�g
	CommandList->SetDescriptorHeaps(1, Mesh.cbvTbvHeap.GetAddressOf());
	//�f�B�X�N���v�^�q�[�v���f�B�X�N���v�^�e�[�u���ɃZ�b�g
	auto hCbvTbvHeap = Mesh.cbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
	CommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);
	//�`��
	CommandList->DrawIndexedInstanced(Mesh.numIndices, 1, 0, 0, 0);
}
