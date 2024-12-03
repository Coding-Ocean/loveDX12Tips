#include"graphic.h"
#include"SKELETAL_MESH.h"

INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"Skeletal Mesh", 1280, 720);

	//����--------------------------------------------------------------
	
	createDescriptorHeap(3);

	SKELETAL_MESH skeletalMesh;
	skeletalMesh.create();

	while (!quit())
	{
		//�X�V------------------------------------------------------------------
		//���[���h�}�g���b�N�X
		XMMATRIX world = XMMatrixTranslation(0, 1, 0);
		//�r���[�}�g���b�N�X
		XMFLOAT3 eye = {0, 0, -4}, focus = {0, 0, 0}, up = {0, 1, 0};
		XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&up));
		//�v���W�F�N�V�����}�g���b�N�X
		XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect(), 1.0f, 10.0f);
		//���C�g�̈ʒu
		XMFLOAT4 lightPos(0, 0, -1, 0);
		//�A�j���[�V�������b�V���̍s��ƃR���X�^���g�o�b�t�@�X�V
		skeletalMesh.update(world, view, proj, lightPos);

		//�`��------------------------------------------------------------------
		beginRender();
		skeletalMesh.draw();
		endRender();
	}
	
	waitGPU();
	closeEventHandle();
}
