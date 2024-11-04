#include "SKELETAL_MESH.h"
#include "model.h"

SKELETAL_MESH::SKELETAL_MESH()
	:Interval(::Interval)
{
}

SKELETAL_MESH::~SKELETAL_MESH()
{
	for (auto& parts : Parts) {
		parts.constBuffer0->Unmap(0, nullptr);
		parts.constBuffer1->Unmap(0, nullptr);
	}
}

void SKELETAL_MESH::create()
{
	//�p�[�c�z�������(����͂P����)
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
			Hr = createBuffer((sizeof(CONST_BUF0) + 0xff) & ~0xff, parts.constBuffer0);
			assert(SUCCEEDED(Hr));
			//�}�b�v���Ă���
			Hr = mapBuffer(parts.constBuffer0, (void**)&parts.cb0);
			assert(SUCCEEDED(Hr));
		}
		//�R���X�^���g�o�b�t�@�P
		{
			//�o�b�t�@������
			Hr = createBuffer((sizeof(CONST_BUF1) + 0xff) & ~0xff, parts.constBuffer1);
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

		Parts.push_back(parts);
	}

	//�{�[���K�w�}�g���b�N�X
	{
		for (int i = 0; i < ::NumBones; ++i) {
			BONE bone;
			
			//�e�̃C���f�b�N�X
			bone.parentIdx = ::ParentIdx[i];

			//�e���猩�����Ύp���s��
			bone.bindWorld = ::BindWorld[i];

			//�A�j���[�V�����f�[�^�B�L�[�t���[���s��
			for (int j = 0; j < ::NumKeyframes; j++) {
				bone.keyframeWorlds.push_back(KeyframeWorlds[j][i]);
			}

			Bones.push_back(bone);
		}
	}
	//�����̎q���̃C���f�b�N�X��childIdxs�z��ɃZ�b�g
	for (int i = 0; i < ::NumBones; i++){
		for (int j = 0; j < ::NumBones; j++){
			if (i == j)	{
				continue;
			}
			if (Bones[i].parentIdx == Bones[j].parentIdx - 1){
				Bones[i].childIdxs.push_back(j);
			}
		}
	}
}

void SKELETAL_MESH::update(XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj, XMFLOAT4& lightPos)
{
	//�ǂ̃L�[�t���[���̊Ԃɂ���̂�
	int keyFrameIdx = FrameCount / Interval;
	if (keyFrameIdx + 1 >= Bones[0].keyframeWorlds.size()){
		keyFrameIdx = 0;
		FrameCount = 0;
	}
	//�L�[�t���[���s��̐��`���
	float t = FrameCount % Interval;
	t /= Interval;
	for (int i = 0; i < ::NumBones; i++) {
		XMMATRIX a = Bones[i].keyframeWorlds[keyFrameIdx];
		XMMATRIX b = Bones[i].keyframeWorlds[keyFrameIdx + 1];
		Bones[i].currentFrameWorld = LerpMatrix(a, b, t);
	}
	//�S�Ẵ{�[��world���X�V�i�ċN�֐��j
	UpdateWorlds(Bones[0], world);
	//���̃t���[����
	FrameCount++;

	//�R���X�^���g�o�b�t�@0�X�V(���[�v���Ă邯�Ǎ���Parts�͂ЂƂ�)
	for (auto& parts : Parts) {
		parts.cb0->viewProj = view * proj;
		parts.cb0->lightPos = lightPos;
		for (int i = 0; i < NumBones; ++i) {
			parts.cb0->boneWorlds[i] = Bones[i].world;
		}
	}
}
void SKELETAL_MESH::UpdateWorlds(BONE& bone, const XMMATRIX& parentWorld)
{
	XMMATRIX invBindWorld = XMMatrixInverse(nullptr, bone.bindWorld);
	bone.world = invBindWorld * bone.currentFrameWorld * bone.bindWorld * parentWorld;

	for(auto& childIdx : bone.childIdxs) {
		UpdateWorlds(Bones[childIdx], bone.world);
	}
}
//2�̍s��̕�Ԍv�Z
XMMATRIX SKELETAL_MESH::LerpMatrix(XMMATRIX& m1, XMMATRIX& m2, float t)
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

void SKELETAL_MESH::draw()
{
	for (auto& parts : Parts) {//���[�v���Ă邯�Ǎ���Parts�͂ЂƂ�
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
