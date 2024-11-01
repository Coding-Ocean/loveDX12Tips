#include "MESH.h"
#include "model.h"

MESH::MESH()
{
}

MESH::~MESH()
{
	for (auto& parts : Parts) {
		parts.ConstBuffer0->Unmap(0, nullptr);
		parts.ConstBuffer1->Unmap(0, nullptr);
	}
}

void MESH::create()
{
	//�p�[�c�z�������
	for (int i = 0; i < NumParts; ++i) {

		PARTS parts;
		
		//���_�o�b�t�@
		{
			//�f�[�^�T�C�Y�����߂Ă���
			UINT sizeInBytes = sizeof(Vertices[i]);
			UINT strideInBytes = sizeof(float) * NumVertexElements;
			parts.NumVertices = sizeInBytes / strideInBytes;
			//�o�b�t�@������
			Hr = createBuffer(sizeInBytes, parts.VertexBuffer);
			assert(SUCCEEDED(Hr));
			//�o�b�t�@�Ƀf�[�^������
			Hr = updateBuffer(Vertices[i], sizeInBytes, parts.VertexBuffer);
			assert(SUCCEEDED(Hr));
			//�o�b�t�@�r���[������
			createVertexBufferView(parts.VertexBuffer, sizeInBytes, strideInBytes, parts.VertexBufferView);
		}
		//���_�C���f�b�N�X�o�b�t�@
		{
			//�f�[�^�T�C�Y�����߂Ă���
			UINT sizeInBytes = sizeof(Indices[i]);
			parts.NumIndices = sizeInBytes / sizeof(UINT16);
			//�o�b�t�@������
			Hr = createBuffer(sizeInBytes, parts.IndexBuffer);
			assert(SUCCEEDED(Hr));
			//�o�b�t�@�Ƀf�[�^������
			Hr = updateBuffer(Indices[i], sizeInBytes, parts.IndexBuffer);
			assert(SUCCEEDED(Hr));
			//�C���f�b�N�X�o�b�t�@�r���[������
			createIndexBufferView(parts.IndexBuffer, sizeInBytes, parts.IndexBufferView);
		}
		//�R���X�^���g�o�b�t�@�O
		{
			//�o�b�t�@������
			Hr = createBuffer(256, parts.ConstBuffer0);
			assert(SUCCEEDED(Hr));
			//�}�b�v���Ă���
			Hr = mapBuffer(parts.ConstBuffer0, (void**)&parts.CB0);
			assert(SUCCEEDED(Hr));
		}
		//�R���X�^���g�o�b�t�@�P
		{
			//�o�b�t�@������
			Hr = createBuffer(256, parts.ConstBuffer1);
			assert(SUCCEEDED(Hr));
			//�}�b�v���Ă���
			Hr = mapBuffer(parts.ConstBuffer1, (void**)&parts.CB1);
			assert(SUCCEEDED(Hr));
			//�f�[�^������
			parts.CB1->ambient = { Ambient[0],Ambient[1],Ambient[2],Ambient[3] };
			parts.CB1->diffuse = { Diffuse[0],Diffuse[1],Diffuse[2],Diffuse[3] };
		}
		//�e�N�X�`���o�b�t�@
		{
			Hr = createTextureBuffer(TextureFilename, parts.TextureBuffer);
			assert(SUCCEEDED(Hr));
		}
		//�f�B�X�N���v�^�q�[�v
		{
			//�f�B�X�N���v�^(�r���[)�R���̃q�[�v������
			Hr = createDescriptorHeap(3, parts.CbvTbvHeap);
			assert(SUCCEEDED(Hr));
			CbvTbvIncSize = cbvTbvIncSize();
			//�P�߂̃f�B�X�N���v�^(�r���[)���q�[�v�ɂ���
			auto hCbvTbvHeap = parts.CbvTbvHeap->GetCPUDescriptorHandleForHeapStart();
			createConstantBufferView(parts.ConstBuffer0, hCbvTbvHeap);
			//�Q�߂̃f�B�X�N���v�^(�r���[)���q�[�v�ɂ���
			hCbvTbvHeap.ptr += CbvTbvIncSize;
			createConstantBufferView(parts.ConstBuffer1, hCbvTbvHeap);
			//�R�߂̃f�B�X�N���v�^(�r���[)���q�[�v�ɂ���
			hCbvTbvHeap.ptr += CbvTbvIncSize;
			createTextureBufferView(parts.TextureBuffer, hCbvTbvHeap);
		}
	
		//�K�w�}�g���b�N�X
		{
			//�e�̃C���f�b�N�X
			parts.parentIdx = ParentIdx[i];

			//�e���猩�����Ύp���s��
			parts.bindWorld = BindWorld[i];
			//parts.bindWorld = XMMatrixIdentity();

			//�A�j���[�V�����f�[�^�B�L�[�t���[���s��
			for (int j = 0; j < NumKeyframes; j++)
			{
				parts.keyframeWorlds.push_back(KeyframeWorlds[j][i]);
				//parts.keyframeWorlds.push_back(XMMatrixIdentity());
			}
		}

		Parts.push_back(parts);
	}
	//�����̎q���̃C���f�b�N�X��childIdxs�z��ɃZ�b�g
	for (int i = 0; i < NumParts; i++)
	{
		for (int j = 0; j < NumParts; j++)
		{
			if (i == j)
			{
				continue;
			}
			if (Parts[i].parentIdx == Parts[j].parentIdx - 1)
			{
				Parts[i].childIdxs.push_back(j);
			}
		}
	}
}

//2�̍s��̊Ԃ̐��`��Ԃ��v�Z���A���ʂ̍s���Ԃ� 
// �N�H�[�^�j�I������]�s��XMMatrixRotationQuaternion    
// ��]�s�񁨃N�H�[�^�j�I����XMQuaternionRotationMatrix
XMMATRIX MESH::LerpMatrix(XMMATRIX& a, XMMATRIX& b, float t)
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

	XMVECTOR qA = XMQuaternionRotationMatrix(a);
	XMVECTOR qB = XMQuaternionRotationMatrix(b);

	//���ꂼ����A���̔䗦�Ő��`��Ԃ���
	XMVECTOR vvA = XMLoadFloat3(&vA);
	XMVECTOR vvB = XMLoadFloat3(&vB);
	XMVECTOR vR = (1.0 - t) * vvA + t * vvB;

	XMVECTOR qR = XMQuaternionSlerp(qA, qB, t);

	//�x�N�g���ƃN�H�[�^�j�I�����s��ɖ߂�
	XMMATRIX ret = XMMatrixRotationQuaternion(qR);

	ret._41 = XMVectorGetX(vR);
	ret._42 = XMVectorGetY(vR);
	ret._43 = XMVectorGetZ(vR);
	ret._44 = 1.0;

	return ret;
}

void MESH::update(int frameCount, int interval, XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj, XMFLOAT4& lightPos)
{
	int keyFrameIdx = frameCount / interval;
	if (keyFrameIdx + 1 >= Parts[0].keyframeWorlds.size())
	{
		return;
	}

	//�L�[�t���[���s��̐��`���
	float t = frameCount % interval;
	t /= interval;
	for (int i = 0; i < NumParts; i++)
	{
		XMMATRIX a = Parts[i].keyframeWorlds[keyFrameIdx];
		XMMATRIX b = Parts[i].keyframeWorlds[keyFrameIdx + 1];
		Parts[i].currentFrameWorld = LerpMatrix(a, b, t);
	}

	//�S�p�[�c��finalWorld���X�V�i�ċN�֐��j
	UpdateFinalWorld(Parts[0], world);

	//�R���X�^���g�o�b�t�@�X�V
	for (auto& parts : Parts) {
		parts.CB0->worldViewProj = parts.finalWorld * view * proj;
		parts.CB0->world = parts.finalWorld;
		parts.CB0->lightPos = lightPos;
	}
}
void MESH::UpdateFinalWorld(PARTS& parts, XMMATRIX& parentWorld)
{
	parts.finalWorld = parts.currentFrameWorld * parts.bindWorld * parentWorld;

	for (int i = 0; i < parts.childIdxs.size(); i++)
	{
		UpdateFinalWorld(Parts[parts.childIdxs[i]], parts.finalWorld);
	}
}

void MESH::draw()
{
	for (auto& parts : Parts) {
		//���_���Z�b�g
		CommandList->IASetVertexBuffers(0, 1, &parts.VertexBufferView);
		CommandList->IASetIndexBuffer(&parts.IndexBufferView);
		//�f�B�X�N���v�^�q�[�v���f�o�t�ɃZ�b�g
		CommandList->SetDescriptorHeaps(1, parts.CbvTbvHeap.GetAddressOf());
		//�f�B�X�N���v�^�q�[�v���f�B�X�N���v�^�e�[�u���ɃZ�b�g
		auto hCbvTbvHeap = parts.CbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
		CommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);
		//�`��
		CommandList->DrawIndexedInstanced(parts.NumIndices, 1, 0, 0, 0);
	}
}
