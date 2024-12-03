#include"graphic.h"
#include"SKELETAL_MESH.h"

INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"Skeletal Mesh", 1280, 720);

	//つくる--------------------------------------------------------------
	
	createDescriptorHeap(3);

	SKELETAL_MESH skeletalMesh;
	skeletalMesh.create();

	while (!quit())
	{
		//更新------------------------------------------------------------------
		//ワールドマトリックス
		XMMATRIX world = XMMatrixTranslation(0, 1, 0);
		//ビューマトリックス
		XMFLOAT3 eye = {0, 0, -4}, focus = {0, 0, 0}, up = {0, 1, 0};
		XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&up));
		//プロジェクションマトリックス
		XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect(), 1.0f, 10.0f);
		//ライトの位置
		XMFLOAT4 lightPos(0, 0, -1, 0);
		//アニメーションメッシュの行列とコンスタントバッファ更新
		skeletalMesh.update(world, view, proj, lightPos);

		//描画------------------------------------------------------------------
		beginRender();
		skeletalMesh.draw();
		endRender();
	}
	
	waitGPU();
	closeEventHandle();
}
