#include "HIERARCHY_MESH.h"
#include "model.h"

HIERARCHY_MESH::HIERARCHY_MESH()
	:Interval(::Interval)
{
}

HIERARCHY_MESH::~HIERARCHY_MESH()
{
	for (auto& mesh : Meshes) {
		unmapBuffer(mesh.constBuffer0);
		unmapBuffer(mesh.constBuffer1);
	}
}

void HIERARCHY_MESH::create()
{
	HRESULT Hr = E_FAIL;

	//�p�[�c�z�������
	for (int i = 0; i < NumMeshes; ++i) {

		MESH mesh;
		
		//���_�o�b�t�@
		{
			//�f�[�^�T�C�Y�����߂Ă���
			UINT sizeInBytes = sizeof(Vertices[i]);
			UINT strideInBytes = sizeof(float) * NumVertexElements;
			//�o�b�t�@������
			Hr = createBuffer(sizeInBytes, mesh.vertexBuffer);
			assert(SUCCEEDED(Hr));
			//�o�b�t�@�Ƀf�[�^������
			Hr = updateBuffer(Vertices[i], sizeInBytes, mesh.vertexBuffer);
			assert(SUCCEEDED(Hr));
			//�o�b�t�@�r���[������
			createVertexBufferView(mesh.vertexBuffer, sizeInBytes, strideInBytes, mesh.vbv);
		}
		//���_�C���f�b�N�X�o�b�t�@
		{
			//�f�[�^�T�C�Y�����߂Ă���
			UINT sizeInBytes = sizeof(Indices[i]);
			//�o�b�t�@������
			Hr = createBuffer(sizeInBytes, mesh.indexBuffer);
			assert(SUCCEEDED(Hr));
			//�o�b�t�@�Ƀf�[�^������
			Hr = updateBuffer(Indices[i], sizeInBytes, mesh.indexBuffer);
			assert(SUCCEEDED(Hr));
			//�C���f�b�N�X�o�b�t�@�r���[������
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
			//�r���[������
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
	
		//�K�w�}�g���b�N�X
		{
			//�e�̃C���f�b�N�X
			mesh.parentIdx = ::ParentIdx[i];

			//�e���猩�����Ύp���s��
			mesh.bindWorld = ::BindWorld[i];
			//mesh.bindWorld = XMMatrixIdentity();

			//�A�j���[�V�����f�[�^�B�L�[�t���[���s��
			for (int j = 0; j < ::NumKeyframes; j++){
				mesh.keyframeWorlds.push_back(KeyframeWorlds[j][i]);
				//mesh.keyframeWorlds.push_back(XMMatrixIdentity());
			}
		}

		Meshes.push_back(mesh);
	}

	//�����̎q���̃C���f�b�N�X��childIdxs�z��ɃZ�b�g
	for (int i = 0; i < ::NumMeshes; i++){
		for (int j = 0; j < ::NumMeshes; j++){
			if (i == j)	{
				continue;
			}
			if (Meshes[i].parentIdx == Meshes[j].parentIdx - 1){
				Meshes[i].childIdxs.push_back(j);
			}
		}
	}
}

void HIERARCHY_MESH::update(XMMATRIX& world, XMMATRIX& viewProj, XMFLOAT4& lightPos)
{
	//�ǂ̃L�[�t���[���̊Ԃɂ���̂�
	int keyFrameIdx = FrameCount / Interval;
	//�Ō�܂ōs������ŏ�����
	if (keyFrameIdx + 1 >= Meshes[0].keyframeWorlds.size()){
		keyFrameIdx = 0;
		FrameCount = 0;
	}
	//�L�[�t���[���s��̐��`���
	float t = static_cast<float>(FrameCount % Interval);
	t /= Interval;
	for (int i = 0; i < ::NumMeshes; i++) {
		XMMATRIX a = Meshes[i].keyframeWorlds[keyFrameIdx];
		XMMATRIX b = Meshes[i].keyframeWorlds[keyFrameIdx + 1];
		Meshes[i].currentFrameWorld = LerpMatrix(a, b, t);
	}
	//�S���b�V����world���X�V�i�ċN�֐��j
	UpdateWorlds(Meshes[0], world);
	//���̃t���[����
	FrameCount++;

	//�R���X�^���g�o�b�t�@�X�V
	for (auto& mesh : Meshes) {
		mesh.cb0->lightPos = lightPos;
		mesh.cb0->viewProj = viewProj;
		mesh.cb0->world = mesh.world;
	}
}
void HIERARCHY_MESH::UpdateWorlds(MESH& mesh, XMMATRIX& parentWorld)
{
	mesh.world = mesh.currentFrameWorld * mesh.bindWorld * parentWorld;

	for(auto& childIdx : mesh.childIdxs) {
		UpdateWorlds(Meshes[childIdx], mesh.world);
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
	for (auto& mesh : Meshes) {
		drawMesh(mesh.vbv, mesh.ibv, mesh.cbvTbvIdx);
	}
}
