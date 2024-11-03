#include"graphic.h"
#include"SKELETAL_MESH.h"

const float ClearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"Hierarchy Mesh", 1280, 720);

	//����--------------------------------------------------------------
	SKELETAL_MESH skinedMesh;
	skinedMesh.create();

	while (!quit())
	{
		//�X�V------------------------------------------------------------------
		//���[���h�}�g���b�N�X
		XMMATRIX world = XMMatrixTranslation(0,0.5f,0);
		//�r���[�}�g���b�N�X
		XMFLOAT3 eye = {0, 0, -2}, focus = {0, 0, 0}, up = {0, 1, 0};
		XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&up));
		//�v���W�F�N�V�����}�g���b�N�X
		XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect(), 1.0f, 10.0f);
		//���C�g�̈ʒu
		XMFLOAT4 lightPos(0, 0, -1, 0);
		//�A�j���[�V�������b�V���̍s��ƃR���X�^���g�o�b�t�@�X�V
		skinedMesh.update(world, view, proj, lightPos);

		//�`��------------------------------------------------------------------
		clear(ClearColor);
		skinedMesh.draw();
		present();
	}
	
	waitGPU();
	closeEventHandle();
}
