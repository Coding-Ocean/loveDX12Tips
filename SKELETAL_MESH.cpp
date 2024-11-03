#include "SKELETAL_MESH.h"
#include "model.h"

SKELETAL_MESH::SKELETAL_MESH()
	:Interval(::Interval)
{
}

SKELETAL_MESH::~SKELETAL_MESH()
{
	ConstBuffer0->Unmap(0, nullptr);
	for (auto& parts : Parts) {
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

	//�R���X�^���g�o�b�t�@�O
	{
		//�o�b�t�@������
		Hr = createBuffer(512, ConstBuffer0);
		assert(SUCCEEDED(Hr));
		//�}�b�v���Ă���
		Hr = mapBuffer(ConstBuffer0, (void**)&Cb0);
		assert(SUCCEEDED(Hr));
	}

	//�f�B�X�N���v�^�q�[�v
	{
		//�f�B�X�N���v�^(�r���[)�R���̃q�[�v������
		Hr = createDescriptorHeap(1 + 2 * NumParts, CbvTbvHeap);
		assert(SUCCEEDED(Hr));
		//�P�߂̃f�B�X�N���v�^(�r���[)���q�[�v�ɂ���
		auto hCbvTbvHeap = CbvTbvHeap->GetCPUDescriptorHandleForHeapStart();
		createConstantBufferView(ConstBuffer0, hCbvTbvHeap);
		for (auto& parts : Parts) {
			//�Q�߂̃f�B�X�N���v�^(�r���[)���q�[�v�ɂ���
			hCbvTbvHeap.ptr += CbvTbvIncSize;
			createConstantBufferView(parts.constBuffer1, hCbvTbvHeap);
			//�R�߂̃f�B�X�N���v�^(�r���[)���q�[�v�ɂ���
			hCbvTbvHeap.ptr += CbvTbvIncSize;
			createTextureBufferView(parts.textureBuffer, hCbvTbvHeap);
		}
	}
}

//2�̍s��̊Ԃ̐��`��Ԃ��v�Z���A���ʂ̍s���Ԃ� 
XMMATRIX SKELETAL_MESH::LerpMatrix(XMMATRIX& a, XMMATRIX& b, float t)
{
	//���ꂼ��̍s�񂩂畽�s�ړ��������A���ꂼ��x�N�g���Ɏ��o��
	XMFLOAT3 vA, vB;
	vA.x = a._41;
	vA.y = a._42;
	vA.z = a._43;
	vB.x = b._41;
	vB.y = b._42;
	vB.z = b._43;

	//���ꂼ��̍s�񂩂�X�P�[�����O�������A���ꂼ��x�N�g���Ɏ��o��
	XMFLOAT3 vA1(a._11, a._12, a._13);
	XMVECTOR a1 = XMLoadFloat3(&vA1);
	XMVECTOR length = XMVector3Length(a1);
	float Sax = 0;//�X�P�[�����OX����
	XMStoreFloat(&Sax, length);
	XMFLOAT3 vA2(a._21, a._22, a._23);
	XMVECTOR a2 = XMLoadFloat3(&vA2);
	length = XMVector3Length(a2);
	float Say = 0;//�X�P�[�����OY����
	XMStoreFloat(&Say, length);
	XMFLOAT3 vA3(a._31, a._32, a._33);
	XMVECTOR a3 = XMLoadFloat3(&vA3);
	length = XMVector3Length(a3);
	float Saz = 0;//�X�P�[�����OZ����
	XMStoreFloat(&Saz, length);

	XMFLOAT3 vB1(b._11, b._12, b._13);
	XMVECTOR b1 = XMLoadFloat3(&vB1);
	length = XMVector3Length(b1);
	float Sbx = 0;//�X�P�[�����OX����
	XMStoreFloat(&Sbx, length);
	XMFLOAT3 vB2(b._21, b._22, b._23);
	XMVECTOR b2 = XMLoadFloat3(&vB2);
	length = XMVector3Length(b2);
	float Sby = 0;//�X�P�[�����OY����
	XMStoreFloat(&Sby, length);
	XMFLOAT3 vB3(b._31, b._32, b._33);
	XMVECTOR b3 = XMLoadFloat3(&vB3);
	length = XMVector3Length(b3);
	float Sbz = 0;//�X�P�[�����OZ����
	XMStoreFloat(&Sbz, length);

	//���ꂼ��̍s�񂩂��]�������A���ꂼ��N�H�[�^�j�I���Ɏ��o��
	//��]�ƃX�P�[�����O�͍���������Ă��邽�߁A��]�����o���O�ɁA�s�񂩂�X�P�[�����O�������������Ȃ��Ă͂Ȃ�Ȃ��B�i���ꂼ��X�P�[�����O�W���ōs������j
	a._11 /= Sax; a._12 /= Sax; a._13 /= Sax;
	a._21 /= Say; a._22 /= Say;	a._23 /= Say;
	a._31 /= Saz; a._32 /= Saz;	a._33 /= Saz;

	b._11 /= Sbx; b._12 /= Sbx;	b._13 /= Sbx;
	b._21 /= Sby; b._22 /= Sby;	b._23 /= Sby;
	b._31 /= Sbz; b._32 /= Sbz;	b._33 /= Sbz;
	
	//��]�s�� �� �N�H�[�^�j�I��
	XMVECTOR qA = XMQuaternionRotationMatrix(a);
	XMVECTOR qB = XMQuaternionRotationMatrix(b);
	//�N�H�[�^�j�I�����`�⊮
	XMVECTOR qR = XMQuaternionSlerp(qA, qB, t);
	//�N�H�[�^�j�I�� �� ��]�s��
	XMMATRIX ret = XMMatrixRotationQuaternion(qR);

	//�ړ���������`���
	XMVECTOR vvA = XMLoadFloat3(&vA);
	XMVECTOR vvB = XMLoadFloat3(&vB);
	XMVECTOR vR = (1.0f - t) * vvA + t * vvB;
	//�ړ��������s��ɖ߂�
	ret._41 = XMVectorGetX(vR);
	ret._42 = XMVectorGetY(vR);
	ret._43 = XMVectorGetZ(vR);
	ret._44 = 1.0;

	return ret;
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
	//�S��worl�{�[����world���X�V�i�ċN�֐��j
	UpdateWorld(Bones[0], XMMatrixIdentity());
	//���̃t���[����
	FrameCount++;

	//�R���X�^���g�o�b�t�@0�X�V
	Cb0->worldViewProj = world * view * proj;
	Cb0->world = world;
	Cb0->lightPos = lightPos;
	for (int i = 0; i < NumBones; ++i) {
		Cb0->boneWorlds[i] = Bones[i].world;
	}
}
void SKELETAL_MESH::UpdateWorld(BONE& bone, const XMMATRIX& parentWorld)
{
	XMMATRIX invBindWorld = XMMatrixInverse(nullptr, bone.bindWorld);
	bone.world = invBindWorld * bone.currentFrameWorld * bone.bindWorld * parentWorld;

	for(auto& childIdx : bone.childIdxs) {
		UpdateWorld(Bones[childIdx], bone.world);
	}
}

void SKELETAL_MESH::draw()
{
	//�f�B�X�N���v�^�q�[�v���f�o�t�ɃZ�b�g
	CommandList->SetDescriptorHeaps(1, CbvTbvHeap.GetAddressOf());
	//Cb0v���f�B�X�N���v�^�e�[�u��0�ɃZ�b�g
	auto hCbvTbvHeap = CbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
	CommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);

	for (auto& parts : Parts) {
		//���_���Z�b�g
		CommandList->IASetVertexBuffers(0, 1, &parts.vertexBufferView);
		CommandList->IASetIndexBuffer(&parts.indexBufferView);
		//cb1v��tbv���f�B�X�N���v�^�e�[�u��1�ɃZ�b�g
		hCbvTbvHeap.ptr += CbvTbvIncSize;
		CommandList->SetGraphicsRootDescriptorTable(1, hCbvTbvHeap);
		//�`��
		CommandList->DrawIndexedInstanced(parts.numIndices, 1, 0, 0, 0);
	}
}
