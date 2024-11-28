#include"graphic.h"
#include"HIERARCHY_MESH.h"

const float ClearColor[] = { 0.8f, 0.8f, 0.8f, 1.0f };

INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"Hierarchy Mesh", 1280, 720);

	//����--------------------------------------------------------------
	//�S�ẴR���X�^���g�o�b�t�@�r���[�A�e�N�X�`���o�b�t�@�r���[�̓��ꕨ��p�ӂ���
	//�P�̃��b�V���Ƀr���[�R�� �~ ���b�V���R�� �� �X
	createDescriptorHeap(3*3);
	
	HIERARCHY_MESH hierarchyMesh;
	hierarchyMesh.create();

	while (!quit())
	{
		//�X�V------------------------------------------------------------------
		//���[���h�}�g���b�N�X
		XMMATRIX world = XMMatrixIdentity();
		//�r���[�}�g���b�N�X
		float r = 0.3f;
		float radius = 3.6f;
		XMFLOAT3 eye = {-sin(r)*radius, 0.7f, -cos(r)*radius}, focus = {0, 0, 0}, up = {0, 1, 0};
		XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&up));
		//�v���W�F�N�V�����}�g���b�N�X
		XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect(), 1.0f, 10.0f);
		//���C�g�̈ʒu
		XMFLOAT4 lightPos(0, 1, 0, 0);
		//�A�j���[�V�������b�V���̍s��ƃR���X�^���g�o�b�t�@�X�V
		hierarchyMesh.update(world, view, proj, lightPos);

		//�`��------------------------------------------------------------------
		beginRender(ClearColor);
		hierarchyMesh.draw();
		endRender();
	}
	
	waitGPU();
	closeEventHandle();
}
