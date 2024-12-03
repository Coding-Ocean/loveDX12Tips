#include "SKELETAL_MESH.h"
#include "model.h"

SKELETAL_MESH::SKELETAL_MESH()
	:Interval(::Interval)
{
}

SKELETAL_MESH::~SKELETAL_MESH()
{
	for (auto& mesh : Meshes) {
		unmapBuffer(mesh.constBuffer0);
		unmapBuffer(mesh.constBuffer1);
	}
}

void SKELETAL_MESH::create()
{
	HRESULT Hr;

	//���b�V���z�������(�z��Ƃ����Ă�����̓��b�V���P��)
	for (int i = 0; i < ::NumMeshes; ++i) {

		MESH mesh;
		
		//���_�o�b�t�@
		{
			//�f�[�^�T�C�Y�����߂Ă���
			UINT sizeInBytes = sizeof(::Vertices[i]);
			UINT strideInBytes = sizeof(float) * ::NumVertexElements;
			//�o�b�t�@������
			Hr = createBuffer(sizeInBytes, mesh.vertexBuffer);
			assert(SUCCEEDED(Hr));
			//�o�b�t�@�Ƀf�[�^������
			Hr = updateBuffer(::Vertices[i], sizeInBytes, mesh.vertexBuffer);
			assert(SUCCEEDED(Hr));
			//�r���[������
			createVertexBufferView(mesh.vertexBuffer, sizeInBytes, strideInBytes, mesh.vbv);
		}
		//���_�C���f�b�N�X�o�b�t�@
		{
			//�f�[�^�T�C�Y�����߂Ă���
			UINT sizeInBytes = sizeof(::Indices[i]);
			//�o�b�t�@������
			Hr = createBuffer(sizeInBytes, mesh.indexBuffer);
			assert(SUCCEEDED(Hr));
			//�o�b�t�@�Ƀf�[�^������
			Hr = updateBuffer(::Indices[i], sizeInBytes, mesh.indexBuffer);
			assert(SUCCEEDED(Hr));
			//�r���[������
			createIndexBufferView(mesh.indexBuffer, sizeInBytes, mesh.ibv);
		}
		//�R���X�^���g�o�b�t�@�O
		{
			//�o�b�t�@������
			Hr = createBuffer(alignedSize(sizeof(CONST_BUF0)), mesh.constBuffer0);
			assert(SUCCEEDED(Hr));
			//�}�b�v���Ă���
			Hr = mapBuffer(mesh.constBuffer0, (void**)&mesh.cb0);
			assert(SUCCEEDED(Hr));
			//�r���[������A�C���f�b�N�X���󂯎���Ă���
			mesh.cbvTbvIdx = createConstantBufferView(mesh.constBuffer0);
		}
		//�R���X�^���g�o�b�t�@�P
		{
			//�o�b�t�@������
			Hr = createBuffer(alignedSize(sizeof(CONST_BUF1)), mesh.constBuffer1);
			assert(SUCCEEDED(Hr));
			//�}�b�v���Ă���
			Hr = mapBuffer(mesh.constBuffer1, (void**)&mesh.cb1);
			assert(SUCCEEDED(Hr));
			//�f�[�^������
			mesh.cb1->ambient = { Ambient[i][0],Ambient[i][1],Ambient[i][2],Ambient[i][3] };
			mesh.cb1->diffuse = { Diffuse[i][0],Diffuse[i][1],Diffuse[i][2],Diffuse[i][3] };
			//�r���[������
			createConstantBufferView(mesh.constBuffer1);
		}
		//�e�N�X�`���o�b�t�@
		{
			Hr = createTextureBuffer(TextureFilename, mesh.textureBuffer);
			assert(SUCCEEDED(Hr));
			//�r���[������
			createTextureBufferView(mesh.textureBuffer);
		}

		Meshes.push_back(mesh);
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

void SKELETAL_MESH::update(XMMATRIX& world, XMMATRIX& viewProj, XMFLOAT4& lightPos)
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

	//�R���X�^���g�o�b�t�@0�X�V(���[�v���Ă邯�Ǎ��񃁃b�V���͂ЂƂ�)
	for (auto& mesh : Meshes) {
		mesh.cb0->lightPos = lightPos;
		mesh.cb0->viewProj = viewProj;
		for (int i = 0; i < ::NumBones; ++i) {
			mesh.cb0->boneWorlds[i] = Bones[i].world;
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
	for (auto& mesh : Meshes) {//���[�v���Ă邯�Ǎ��񃁃b�V���͂ЂƂ�
		drawMesh(mesh.vbv, mesh.ibv, mesh.cbvTbvIdx);
	}
}
