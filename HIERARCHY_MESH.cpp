#include "HIERARCHY_MESH.h"
#include "model.h"

HIERARCHY_MESH::HIERARCHY_MESH()
	:Interval(::Interval)
{
}

HIERARCHY_MESH::~HIERARCHY_MESH()
{
	for (auto& parts : Parts) {
		parts.constBuffer0->Unmap(0, nullptr);
		parts.constBuffer1->Unmap(0, nullptr);
	}
}

void HIERARCHY_MESH::create()
{
	//�p�[�c�z�������
	for (int i = 0; i < NumParts; ++i) {

		PARTS parts;
		
		//���_�o�b�t�@
		{
			//�f�[�^�T�C�Y�����߂Ă���
			UINT sizeInBytes = sizeof(Vertices[i]);
			UINT strideInBytes = sizeof(float) * NumVertexElements;
			parts.numVertices = sizeInBytes / strideInBytes;
			//�o�b�t�@������
			Hr = createBuffer(sizeInBytes, parts.vertexBuffer);
			assert(SUCCEEDED(Hr));
			//�o�b�t�@�Ƀf�[�^������
			Hr = updateBuffer(Vertices[i], sizeInBytes, parts.vertexBuffer);
			assert(SUCCEEDED(Hr));
			//�o�b�t�@�r���[������
			createVertexBufferView(parts.vertexBuffer, sizeInBytes, strideInBytes, parts.vertexBufferView);
		}
		//���_�C���f�b�N�X�o�b�t�@
		{
			//�f�[�^�T�C�Y�����߂Ă���
			UINT sizeInBytes = sizeof(Indices[i]);
			parts.numIndices = sizeInBytes / sizeof(UINT16);
			//�o�b�t�@������
			Hr = createBuffer(sizeInBytes, parts.indexBuffer);
			assert(SUCCEEDED(Hr));
			//�o�b�t�@�Ƀf�[�^������
			Hr = updateBuffer(Indices[i], sizeInBytes, parts.indexBuffer);
			assert(SUCCEEDED(Hr));
			//�C���f�b�N�X�o�b�t�@�r���[������
			createIndexBufferView(parts.indexBuffer, sizeInBytes, parts.indexBufferView);
		}
		//�R���X�^���g�o�b�t�@�O
		{
			//�o�b�t�@������
			Hr = createBuffer(256, parts.constBuffer0);
			assert(SUCCEEDED(Hr));
			//�}�b�v���Ă���
			Hr = mapBuffer(parts.constBuffer0, (void**)&parts.cb0);
			assert(SUCCEEDED(Hr));
		}
		//�R���X�^���g�o�b�t�@�P
		{
			//�o�b�t�@������
			Hr = createBuffer(256, parts.constBuffer1);
			assert(SUCCEEDED(Hr));
			//�}�b�v���Ă���
			Hr = mapBuffer(parts.constBuffer1, (void**)&parts.cb1);
			assert(SUCCEEDED(Hr));
			//�f�[�^������
			parts.cb1->ambient = { Ambient[i][0],Ambient[i][1],Ambient[i][2],Ambient[i][3] };
			parts.cb1->diffuse = { Diffuse[i][0],Diffuse[i][1],Diffuse[i][2],Diffuse[i][3] };
		}
		//�e�N�X�`���o�b�t�@
		{
			Hr = createTextureBuffer(TextureFilename, parts.textureBuffer);
			assert(SUCCEEDED(Hr));
		}
		//�f�B�X�N���v�^�q�[�v
		{
			//�f�B�X�N���v�^(�r���[)�R���̃q�[�v������
			Hr = createDescriptorHeap(3, parts.cbvTbvHeap);
			assert(SUCCEEDED(Hr));
			//�P�߂̃f�B�X�N���v�^(�r���[)���q�[�v�ɂ���
			auto hCbvTbvHeap = parts.cbvTbvHeap->GetCPUDescriptorHandleForHeapStart();
			createConstantBufferView(parts.constBuffer0, hCbvTbvHeap);
			//�Q�߂̃f�B�X�N���v�^(�r���[)���q�[�v�ɂ���
			hCbvTbvHeap.ptr += CbvTbvIncSize;
			createConstantBufferView(parts.constBuffer1, hCbvTbvHeap);
			//�R�߂̃f�B�X�N���v�^(�r���[)���q�[�v�ɂ���
			hCbvTbvHeap.ptr += CbvTbvIncSize;
			createTextureBufferView(parts.textureBuffer, hCbvTbvHeap);
		}
	
		//�K�w�}�g���b�N�X
		{
			//�e�̃C���f�b�N�X
			parts.parentIdx = ::ParentIdx[i];

			//�e���猩�����Ύp���s��
			parts.bindWorld = ::BindWorld[i];
			//parts.bindWorld = XMMatrixIdentity();

			//�A�j���[�V�����f�[�^�B�L�[�t���[���s��
			for (int j = 0; j < ::NumKeyframes; j++){
				parts.keyframeWorlds.push_back(KeyframeWorlds[j][i]);
				//parts.keyframeWorlds.push_back(XMMatrixIdentity());
			}
		}

		Parts.push_back(parts);
	}

	//�����̎q���̃C���f�b�N�X��childIdxs�z��ɃZ�b�g
	for (int i = 0; i < ::NumParts; i++){
		for (int j = 0; j < ::NumParts; j++){
			if (i == j)	{
				continue;
			}
			if (Parts[i].parentIdx == Parts[j].parentIdx - 1){
				Parts[i].childIdxs.push_back(j);
			}
		}
	}
}

void HIERARCHY_MESH::update(XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj, XMFLOAT4& lightPos)
{
	//�ǂ̃L�[�t���[���̊Ԃɂ���̂�
	int keyFrameIdx = FrameCount / Interval;
	//�Ō�܂ōs������ŏ�����
	if (keyFrameIdx + 1 >= Parts[0].keyframeWorlds.size()){
		keyFrameIdx = 0;
		FrameCount = 0;
	}
	//�L�[�t���[���s��̐��`���
	float t = FrameCount % Interval;
	t /= Interval;
	for (int i = 0; i < ::NumParts; i++) {
		XMMATRIX a = Parts[i].keyframeWorlds[keyFrameIdx];
		XMMATRIX b = Parts[i].keyframeWorlds[keyFrameIdx + 1];
		Parts[i].currentFrameWorld = LerpMatrix(a, b, t);
	}
	//�S�p�[�c��world���X�V�i�ċN�֐��j
	UpdateWorlds(Parts[0], world);
	//���̃t���[����
	FrameCount++;

	//�R���X�^���g�o�b�t�@�X�V
	for (auto& parts : Parts) {
		parts.cb0->worldViewProj = parts.world * view * proj;
		parts.cb0->world = parts.world;
		parts.cb0->lightPos = lightPos;
	}
}
void HIERARCHY_MESH::UpdateWorlds(PARTS& parts, XMMATRIX& parentWorld)
{
	parts.world = parts.currentFrameWorld * parts.bindWorld * parentWorld;

	for(auto& childIdx : parts.childIdxs) {
		UpdateWorlds(Parts[childIdx], parts.world);
	}
}
//2�̍s��̕�Ԍv�Z
XMMATRIX HIERARCHY_MESH::LerpMatrix(XMMATRIX& m1, XMMATRIX& m2, float t)
{
	//t��0����1�͈̔͂ɐ���
	t = (t < 0) ? 0 : (t > 1) ? 1 : t;

	//�s�񂩂�N�H�[�^�j�I���ɕϊ�
	XMVECTOR q1 = XMQuaternionRotationMatrix(m1);
	XMVECTOR q2 = XMQuaternionRotationMatrix(m2);
	//�N�H�[�^�j�I���Ԃ�SLERP
	XMVECTOR q = XMQuaternionSlerp(q1, q2, t);
	//��Ԃ��ꂽ�N�H�[�^�j�I�����s��ɕϊ�
	XMMATRIX m = XMMatrixRotationQuaternion(q);

	//�s�񂩂畽�s�ړ������𒊏o
	XMVECTOR t1 = m1.r[3];
	XMVECTOR t2 = m2.r[3];
	//���s�ړ���������`���
	XMVECTOR t_ = XMVectorLerp(t1, t2, t);
	//m�̉�]������ێ����A��Ԃ��ꂽ���s�ړ���K�p
	m.r[3] = t_;

	return m;
}

void HIERARCHY_MESH::draw()
{
	for (auto& parts : Parts) {
		//���_���Z�b�g
		CommandList->IASetVertexBuffers(0, 1, &parts.vertexBufferView);
		CommandList->IASetIndexBuffer(&parts.indexBufferView);
		//�f�B�X�N���v�^�q�[�v���f�o�t�ɃZ�b�g
		CommandList->SetDescriptorHeaps(1, parts.cbvTbvHeap.GetAddressOf());
		//�f�B�X�N���v�^�q�[�v���f�B�X�N���v�^�e�[�u���ɃZ�b�g
		auto hCbvTbvHeap = parts.cbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
		CommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);
		//�`��
		CommandList->DrawIndexedInstanced(parts.numIndices, 1, 0, 0, 0);
	}
}
