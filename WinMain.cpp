#include"graphic.h"
#include"MESH.h"

const float ClearColor[] = { 0.25f, 0.5f, 0.8f, 1.0f };

INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"Hierarchy Mesh", 1280, 720);

	//����--------------------------------------------------------------
	MESH mesh;
	mesh.create();

	while (!quit())
	{
		//�X�V------------------------------------------------------------------
		//���[���h�}�g���b�N�X
		XMMATRIX world = XMMatrixIdentity();
		//�r���[�}�g���b�N�X
		float r = 0.5f;
		float radius = 4.0f;
		XMFLOAT3 eye = {-sin(r)*radius, 0.8f, -cos(r)*radius}, focus = {0, 0, 0}, up = {0, 1, 0};
		XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&up));
		//�v���W�F�N�V�����}�g���b�N�X
		XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect(), 1.0f, 10.0f);
		//���C�g�̈ʒu
		XMFLOAT4 lightPos(0, 1, 0, 0);

		//�A�j���[�V�������b�V���̍s��ƃR���X�^���g�o�b�t�@�X�V
		static int frameCount = 0;
		int interval = 260;//�L�[�t���[���̊Ԋu
		mesh.update(frameCount, interval, world, view, proj, lightPos);
		frameCount++;
		if (frameCount > interval * 2)//�Ԋu�͂Q��
		{
			frameCount = 0;
		}

		//�`��------------------------------------------------------------------
		clear(ClearColor);
		mesh.draw();
		present();
	}
	
	waitGPU();
	closeEventHandle();
}
