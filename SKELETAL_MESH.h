#pragma once
#include<vector>
#include"graphic.h"

//�R���X�^���g�o�b�t�@�\����
struct CONST_BUF0 
{
	XMMATRIX viewProj;
	XMFLOAT4 lightPos;
	XMMATRIX boneWorlds[2];
};
struct CONST_BUF1 
{
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
};

//�p�[�c���b�V���\����
struct PARTS 
{
	//���_�o�b�t�@
	UINT numVertices = 0;
	ComPtr<ID3D12Resource>   vertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
	//���_�C���f�b�N�X�o�b�t�@
	UINT numIndices = 0;
	ComPtr<ID3D12Resource>  indexBuffer = nullptr;
	D3D12_INDEX_BUFFER_VIEW	indexBufferView = {};
	//�R���X�^���g�o�b�t�@
	CONST_BUF1* cb1 = nullptr;
	ComPtr<ID3D12Resource> constBuffer1 = nullptr;
	//�e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> textureBuffer = nullptr;
};

//�{�[���\����
struct BONE
{
	//�v�Z��̍ŏI�I�ȍs��
	XMMATRIX world;
	//�e����̑��Ύp���s��
	XMMATRIX bindWorld;
	//�A�j���[�V�����f�[�^�B�L�[�t���[���s��
	std::vector<XMMATRIX> keyframeWorlds;
	XMMATRIX currentFrameWorld;
	//�@���̒l���g���āA�q���C���f�b�N�X�z�������
	int parentIdx;
	//�@�q���C���f�b�N�X�z��
	std::vector<int> childIdxs;
};

//Skeletal Mesh
class SKELETAL_MESH
{
private:
	std::vector<PARTS> Parts;
	std::vector<BONE> Bones;

	UINT FrameCount = 0;
	UINT Interval;//�L�[�t���[���̊Ԋu

	CONST_BUF0* Cb0 = nullptr;
	ComPtr<ID3D12Resource> ConstBuffer0 = nullptr;
	ComPtr<ID3D12DescriptorHeap> CbvTbvHeap = nullptr;

	//�V�X�e���n
	HRESULT Hr = E_FAIL;
	UINT CbvTbvIncSize = cbvTbvIncSize();
	ComPtr<ID3D12GraphicsCommandList>& CommandList = commandList();
public:
	SKELETAL_MESH();
	~SKELETAL_MESH();
	void create();
	void update(XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj, XMFLOAT4& light);
	void draw();
private:
	//update()�̒�����Ăяo�����Q�̊֐�
	XMMATRIX LerpMatrix(XMMATRIX& a, XMMATRIX& b, float t);
	void UpdateWorld(BONE& b, const XMMATRIX& m);
};

