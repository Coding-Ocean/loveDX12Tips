#pragma once
#include"graphic.h"

//�R���X�^���g�o�b�t�@�\����
struct CONST_BUF0 
{
	XMFLOAT4 lightPos;
	XMMATRIX viewProj;
	XMMATRIX world;
};
struct CONST_BUF1 
{
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
};

//���b�V��
struct MESH 
{
	//���_�o�b�t�@
	UINT numVertices = 0;
	ComPtr<ID3D12Resource>   vertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
	//���_�C���f�b�N�X�o�b�t�@
	UINT numIndices = 0;
	ComPtr<ID3D12Resource>  indexBuffer = nullptr;
	D3D12_INDEX_BUFFER_VIEW	indexBufferView = {};
	//�R���X�^���g�o�b�t�@0
	ComPtr<ID3D12Resource> constBuffer0 = nullptr;
	CONST_BUF0* cb0 = nullptr;
	//�R���X�^���g�o�b�t�@1
	ComPtr<ID3D12Resource> constBuffer1 = nullptr;
	CONST_BUF1* cb1 = nullptr;
	//�e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> textureBuffer = nullptr;
	//�f�B�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> cbvTbvHeap = nullptr;
};

//�`���ς��Ȃ����b�V��
class STATIC_MESH
{
private:
	MESH Mesh;//�����Mesh�ЂƂ�

	//�V�X�e���n
	HRESULT Hr = E_FAIL;
	UINT CbvTbvIncSize = cbvTbvIncSize();
	ComPtr<ID3D12GraphicsCommandList>& CommandList = commandList();
public:
	STATIC_MESH();
	~STATIC_MESH();
	void create();
	void update(XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj, XMFLOAT4& lightPos);
	void draw();
};

