#include"graphic.h"
#include"MESH.h"

const float ClearColor[] = { 0.25f, 0.5f, 0.8f, 1.0f };

INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"Hierarchy Mesh", 1280, 720);

	//つくる--------------------------------------------------------------
	MESH mesh;
	mesh.create();

	while (!quit())
	{
		//更新------------------------------------------------------------------
		//ワールドマトリックス
		XMMATRIX world = XMMatrixIdentity();
		//ビューマトリックス
		float r = 0.5f;
		float radius = 4.0f;
		XMFLOAT3 eye = {-sin(r)*radius, 0.8f, -cos(r)*radius}, focus = {0, 0, 0}, up = {0, 1, 0};
		XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&up));
		//プロジェクションマトリックス
		XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect(), 1.0f, 10.0f);
		//ライトの位置
		XMFLOAT4 lightPos(0, 1, 0, 0);

		//アニメーションメッシュの行列とコンスタントバッファ更新
		static int frameCount = 0;
		int interval = 260;//キーフレームの間隔
		mesh.update(frameCount, interval, world, view, proj, lightPos);
		frameCount++;
		if (frameCount > interval * 2)//間隔は２つ
		{
			frameCount = 0;
		}

		//描画------------------------------------------------------------------
		clear(ClearColor);
		mesh.draw();
		present();
	}
	
	waitGPU();
	closeEventHandle();
}
