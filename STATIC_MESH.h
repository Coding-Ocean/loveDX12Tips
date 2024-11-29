#pragma once
#include"graphic.h"

//���b�V��
struct MESH 
{
	//���_�o�b�t�@
	ComPtr<ID3D12Resource>   vertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbv = {};
	//���_�C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource>  indexBuffer = nullptr;
	D3D12_INDEX_BUFFER_VIEW	ibv = {};
	//�R���X�^���g�o�b�t�@0
	ComPtr<ID3D12Resource> constBuffer0 = nullptr;
	CONST_BUF0* cb0 = nullptr;
	//�R���X�^���g�o�b�t�@1
	ComPtr<ID3D12Resource> constBuffer1 = nullptr;
	CONST_BUF1* cb1 = nullptr;
	//�e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> textureBuffer = nullptr;
	//�f�B�X�N���v�^�C���f�b�N�X
	UINT cbvTbvIdx = 0;
};

//�`���ς��Ȃ����b�V��
class STATIC_MESH
{
private:
	MESH Mesh;//�����Mesh�ЂƂ�
public:
	STATIC_MESH();
	~STATIC_MESH();
	void create();
	void update(XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj, XMFLOAT4& lightPos);
	void draw();
};

