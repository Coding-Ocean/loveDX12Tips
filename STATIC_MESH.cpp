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
	HRESULT Hr = E_FAIL;

	//���_�o�b�t�@
	{
		//�f�[�^�T�C�Y�����߂Ă���
		UINT sizeInBytes = sizeof(Vertices);
		UINT strideInBytes = sizeof(float) * NumVertexElements;
		//�o�b�t�@������
		Hr = createBuffer(sizeInBytes, Mesh.vertexBuffer);
		assert(SUCCEEDED(Hr));
		//�o�b�t�@�Ƀf�[�^������
		Hr = updateBuffer(Vertices, sizeInBytes, Mesh.vertexBuffer);
		assert(SUCCEEDED(Hr));
		//�o�b�t�@�r���[������
		createVertexBufferView(Mesh.vertexBuffer, sizeInBytes, strideInBytes, Mesh.vbv);
	}
	//���_�C���f�b�N�X�o�b�t�@
	{
		//�f�[�^�T�C�Y�����߂Ă���
		UINT sizeInBytes = sizeof(Indices);
		//�o�b�t�@������
		Hr = createBuffer(sizeInBytes, Mesh.indexBuffer);
		assert(SUCCEEDED(Hr));
		//�o�b�t�@�Ƀf�[�^������
		Hr = updateBuffer(Indices, sizeInBytes, Mesh.indexBuffer);
		assert(SUCCEEDED(Hr));
		//�C���f�b�N�X�o�b�t�@�r���[������
		createIndexBufferView(Mesh.indexBuffer, sizeInBytes, Mesh.ibv);
	}
	//�R���X�^���g�o�b�t�@�O
	{
		//�o�b�t�@������
		Hr = createBuffer(256, Mesh.constBuffer0);
		assert(SUCCEEDED(Hr));
		//�}�b�v���Ă���
		Hr = mapBuffer(Mesh.constBuffer0, (void**)&Mesh.cb0);
		assert(SUCCEEDED(Hr));
		//�r���[������
		Mesh.cbvTbvIdx = createConstantBufferView(Mesh.constBuffer0);
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
		//�r���[������
		createConstantBufferView(Mesh.constBuffer1);
	}
	//�e�N�X�`���o�b�t�@
	{
		//�t�@�C����ǂݍ���ŁA�o�b�t�@������
		Hr = createTextureBuffer(TextureFilename, Mesh.textureBuffer);
		assert(SUCCEEDED(Hr));
		//�r���[������
		createTextureBufferView(Mesh.textureBuffer);
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
	drawMesh(Mesh.vbv, Mesh.ibv, Mesh.cbvTbvIdx);
}
