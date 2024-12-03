#include"graphic.h"
#include"model.h"

//メッシュデータ
//　頂点バッファ
ComPtr<ID3D12Resource>   VertexBuffer = nullptr;
D3D12_VERTEX_BUFFER_VIEW Vbv;
//　頂点インデックスバッファ
ComPtr<ID3D12Resource>  IndexBuffer = nullptr;
D3D12_INDEX_BUFFER_VIEW	Ibv;
//　コンスタントバッファ０
ComPtr<ID3D12Resource> ConstBuffer0 = nullptr;
CONST_BUF0* CB0 = nullptr;
//　コンスタントバッファ１
ComPtr<ID3D12Resource> ConstBuffer1 = nullptr;
CONST_BUF1* CB1 = nullptr;
//　テクスチャバッファ
ComPtr<ID3D12Resource> TextureBuffer = nullptr;
//　ディスクリプタインデックス
UINT CbvTbvIdx = 0;

//Entry point
INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"Lambert Pixel", 1280, 720);

	HRESULT Hr;

	//ディスクリプタ３つ分のヒープをつくる
	Hr = createDescriptorHeap(3);
	assert(SUCCEEDED(Hr));

	//リソース初期化
	{
		//頂点バッファ
		{
			//データサイズを求めておく
			UINT sizeInBytes = sizeof(Vertices);
			UINT strideInBytes = sizeof(float) * NumVertexElements;
			//バッファをつくる
			Hr = createBuffer(sizeInBytes, VertexBuffer);
			assert(SUCCEEDED(Hr));
			//バッファにデータを入れる
			Hr = updateBuffer(Vertices, sizeInBytes, VertexBuffer);
			assert(SUCCEEDED(Hr));
			//バッファビューをつくる
			createVertexBufferView(VertexBuffer, sizeInBytes, strideInBytes, Vbv);
		}
		//頂点インデックスバッファ
		{
			//データサイズを求めておく
			UINT sizeInBytes = sizeof(Indices);
			//バッファをつくる
			Hr = createBuffer(sizeInBytes, IndexBuffer);
			assert(SUCCEEDED(Hr));
			//バッファにデータを入れる
			Hr = updateBuffer(Indices, sizeInBytes, IndexBuffer);
			assert(SUCCEEDED(Hr));
			//インデックスバッファビューをつくる
			createIndexBufferView(IndexBuffer, sizeInBytes, Ibv);
		}
		//コンスタントバッファ０
		{
			//バッファをつくる
			Hr = createBuffer(alignedSize(sizeof(CONST_BUF0)), ConstBuffer0);
			assert(SUCCEEDED(Hr));
			//マップしておく
			Hr = mapBuffer(ConstBuffer0, (void**)&CB0);
			assert(SUCCEEDED(Hr));
			//ビューをつくって、インデックスをもらっておく
			CbvTbvIdx = createConstantBufferView(ConstBuffer0);
		}
		//コンスタントバッファ１
		{
			//バッファをつくる
			Hr = createBuffer(alignedSize(sizeof(CONST_BUF1)), ConstBuffer1);
			assert(SUCCEEDED(Hr));
			//マップしておく
			Hr = mapBuffer(ConstBuffer1, (void**)&CB1);
			assert(SUCCEEDED(Hr));
			//データを入れる
			CB1->ambient = { Ambient[0],Ambient[1],Ambient[2],Ambient[3] };
			CB1->diffuse = { Diffuse[0],Diffuse[1],Diffuse[2],Diffuse[3] };
			//ビューをつくる
			createConstantBufferView(ConstBuffer1);
		}
		//テクスチャバッファ
		{
			Hr = createTextureBuffer(TextureFilename, TextureBuffer);
			assert(SUCCEEDED(Hr));
			//ビューをつくる
			createTextureBufferView(TextureBuffer);
		}
	}
	
	//メインループ
	while (!quit())
	{
		//更新------------------------------------------------------------------
		//ワールドマトリックス
		XMMATRIX world = XMMatrixIdentity();
		//ビューマトリックス
		XMFLOAT3 eye = { 0, 0, -2 }, focus = { 0, 0, 0 }, up = { 0, 1, 0 };
		XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&up));
		//プロジェクションマトリックス
		XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect(), 1.0f, 10.0f);

		//ライト位置移動用ラジアン値
		static float r = 0;
		r += 0.01f;
		CB0->lightPos = { cosf(r), 0, powf(cosf(r/6),2)*-1.5f, 0};
		CB0->worldViewProj = world * view * proj;
		CB0->world = world;

		//描画------------------------------------------------------------------
		beginRender();
		drawMesh(Vbv, Ibv, CbvTbvIdx);
		endRender();
	}
	
	//後始末
	{
		waitGPU();
		closeEventHandle();
		unmapBuffer(ConstBuffer0);
		unmapBuffer(ConstBuffer1);
	}
}
