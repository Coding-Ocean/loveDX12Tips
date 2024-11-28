#include"graphic.h"
#include"HIERARCHY_MESH.h"

const float ClearColor[] = { 0.8f, 0.8f, 0.8f, 1.0f };

INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"Hierarchy Mesh", 1280, 720);

	//つくる--------------------------------------------------------------
	//全てのコンスタントバッファビュー、テクスチャバッファビューの入れ物を用意する
	//１つのメッシュにビュー３つ × メッシュ３つ ＝ ９
	createDescriptorHeap(3*3);
	
	HIERARCHY_MESH hierarchyMesh;
	hierarchyMesh.create();

	while (!quit())
	{
		//更新------------------------------------------------------------------
		//ワールドマトリックス
		XMMATRIX world = XMMatrixIdentity();
		//ビューマトリックス
		float r = 0.3f;
		float radius = 3.6f;
		XMFLOAT3 eye = {-sin(r)*radius, 0.7f, -cos(r)*radius}, focus = {0, 0, 0}, up = {0, 1, 0};
		XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&up));
		//プロジェクションマトリックス
		XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect(), 1.0f, 10.0f);
		//ライトの位置
		XMFLOAT4 lightPos(0, 1, 0, 0);
		//アニメーションメッシュの行列とコンスタントバッファ更新
		hierarchyMesh.update(world, view, proj, lightPos);

		//描画------------------------------------------------------------------
		beginRender(ClearColor);
		hierarchyMesh.draw();
		endRender();
	}
	
	waitGPU();
	closeEventHandle();
}
