#include"graphic.h"
#include"STATIC_MESH.h"

const float ClearColor[] = { 0.25f, 0.5f, 0.8f, 1.0f };

INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"Lambert Vertex", 1280, 720);

	//����--------------------------------------------------------------
	STATIC_MESH mesh;
	mesh.create();

	while (!quit())
	{
		//�X�V------------------------------------------------------------------
		//��]�p���W�A��
		static float r = 0;
		r += 0.01f;
		//���[���h�}�g���b�N�X
		XMMATRIX world = XMMatrixRotationX(r);
		//�r���[�}�g���b�N�X
		XMFLOAT3 eye = { 0, 0, -2 }, focus = { 0, 0, 0 }, up = { 0, 1, 0 };
		XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&up));
		//�v���W�F�N�V�����}�g���b�N�X
		XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect(), 1.0f, 10.0f);
		//���C�g�ʒu
		XMFLOAT4 lightPos = { 0,1,0,0 };
		mesh.update(world, view, proj, lightPos);

		//�`��------------------------------------------------------------------
		clear(ClearColor);
		mesh.draw();
		present();
	}
	
	waitGPU();
	closeEventHandle();
}
