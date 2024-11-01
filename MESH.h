#pragma once
#include"graphic.h"

//�R���X�^���g�o�b�t�@�\����
struct CONST_BUF0 
{
	XMMATRIX worldViewProj;
	XMMATRIX world;
	XMFLOAT4 lightPos;
};
struct CONST_BUF1 
{
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
};

//�p�[�c���b�V��
struct PARTS 
{
	//���_�o�b�t�@
	UINT NumVertices = 0;
	ComPtr<ID3D12Resource>   VertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView = {};
	//���_�C���f�b�N�X�o�b�t�@
	UINT NumIndices = 0;
	ComPtr<ID3D12Resource>  IndexBuffer = nullptr;
	D3D12_INDEX_BUFFER_VIEW	IndexBufferView = {};
	//�R���X�^���g�o�b�t�@
	CONST_BUF0* CB0 = nullptr;
	CONST_BUF1* CB1 = nullptr;
	ComPtr<ID3D12Resource> ConstBuffer0 = nullptr;
	ComPtr<ID3D12Resource> ConstBuffer1 = nullptr;
	//�e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> TextureBuffer = nullptr;
	//�f�B�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> CbvTbvHeap = nullptr;
};

class MESH
{
private:
	PARTS Parts;//�����Parts�ЂƂ�
	UINT CbvTbvIncSize = 0;
	HRESULT Hr;
	ComPtr<ID3D12GraphicsCommandList>& CommandList=commandList();
public:
	MESH();
	~MESH();
	void create();
	void update(XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj);
	void draw();
};
