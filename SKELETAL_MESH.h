#pragma once
#include<vector>
#include"graphic.h"

//���b�V���p�[�c�\����
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
	UINT cbvTbvIdx=0;
};

//�{�[���\����
struct BONE
{
	//�v�Z��̍ŏI�I�ȍs��B������V�F�[�_�ɑ���
	XMMATRIX world;
	//�e����̑��Ύp���s��
	XMMATRIX bindWorld;
	//�A�j���[�V�����f�[�^�B�L�[�t���[���s��
	std::vector<XMMATRIX> keyframeWorlds;
	XMMATRIX currentFrameWorld;
	//���̒l���g���āA�q���C���f�b�N�X�z�������
	int parentIdx;
	//�q���C���f�b�N�X�z��
	std::vector<int> childIdxs;
};

//Skeletal Mesh
class SKELETAL_MESH
{
private:
	std::vector<MESH> Meshes;
	std::vector<BONE> Bones;
	UINT FrameCount = 0;
	UINT Interval;//�L�[�t���[���̊Ԋu
public:
	SKELETAL_MESH();
	~SKELETAL_MESH();
	void create();
	void update(XMMATRIX& world, XMMATRIX& viewProj, XMFLOAT4& light);
	void draw();
private:
	//update()�̒�����Ăяo�����Q�̊֐�
	void UpdateWorlds(BONE& bone, const XMMATRIX& parentWorld);
	XMMATRIX LerpMatrix(XMMATRIX& a, XMMATRIX& b, float t);
};

