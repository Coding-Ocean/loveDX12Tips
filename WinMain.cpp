#include"graphic.h"
#include"STATIC_MESH.h"

const float ClearColor[] = { 0.25f, 0.5f, 0.8f, 1.0f };

INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"Lambert Vertex", 1280, 720);

	//つくる--------------------------------------------------------------
	STATIC_MESH mesh;
	mesh.create();

	while (!quit())
	{
		//更新------------------------------------------------------------------
		//回転用ラジアン
		static float r = 0;
		r += 0.01f;
		//ワールドマトリックス
		XMMATRIX world = XMMatrixRotationX(r);
		//ビューマトリックス
		XMFLOAT3 eye = { 0, 0, -2 }, focus = { 0, 0, 0 }, up = { 0, 1, 0 };
		XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&up));
		//プロジェクションマトリックス
		XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect(), 1.0f, 10.0f);
		//ライト位置
		XMFLOAT4 lightPos = { 0,1,0,0 };
		mesh.update(world, view, proj, lightPos);

		//描画------------------------------------------------------------------
		clear(ClearColor);
		mesh.draw();
		present();
	}
	
	waitGPU();
	closeEventHandle();
}
