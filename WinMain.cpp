#include"graphic.h"
#include"model.h"

//メッシュ
//　頂点バッファ
ComPtr<ID3D12Resource>   VertexBuffer = nullptr;
D3D12_VERTEX_BUFFER_VIEW Vbv;
//　頂点インデックスバッファ
ComPtr<ID3D12Resource>  IndexBuffer = nullptr;
D3D12_INDEX_BUFFER_VIEW	Ibv;
//　コンスタントバッファ
ComPtr<ID3D12Resource> ConstBuffer0 = nullptr;
ComPtr<ID3D12Resource> ConstBuffer1 = nullptr;
CONST_BUF0* CB0 = nullptr;
CONST_BUF1* CB1 = nullptr;
//　テクスチャバッファ
ComPtr<ID3D12Resource> TextureBuffer = nullptr;
UINT CbvTbvIdx = 0;

//Entry point
INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"Deferred Rendering", 1280, 720);

	HRESULT Hr;

	//リソース初期化
	{
		//コンスタント・テクスチャ系ディスクリプタヒープ
		{
			//メッシュ用3*1 ＋ ディファード用2*2
			Hr = createDescriptorHeap(3 + 4);
			assert(SUCCEEDED(Hr));
		}

		//メッシュをつくる
		//　頂点バッファ
		{
			//データサイズを求めておく
			UINT sizeInBytes = sizeof(::Vertices);
			UINT strideInBytes = sizeof(float) * ::NumVertexElements;
			//バッファをつくる
			Hr = createBuffer(sizeInBytes, VertexBuffer);
			assert(SUCCEEDED(Hr));
			//バッファにデータを入れる
			Hr = updateBuffer(::Vertices, sizeInBytes, VertexBuffer);
			assert(SUCCEEDED(Hr));
			//バッファビューをつくる
			createVertexBufferView(VertexBuffer, sizeInBytes, strideInBytes, Vbv);
		}
		//　頂点インデックスバッファ
		{
			//データサイズを求めておく
			UINT sizeInBytes = sizeof(::Indices);
			//バッファをつくる
			Hr = createBuffer(sizeInBytes, IndexBuffer);
			assert(SUCCEEDED(Hr));
			//バッファにデータを入れる
			Hr = updateBuffer(::Indices, sizeInBytes, IndexBuffer);
			assert(SUCCEEDED(Hr));
			//インデックスバッファビューをつくる
			createIndexBufferView(IndexBuffer, sizeInBytes, Ibv);
		}
		//　コンスタントバッファ０
		{
			//バッファをつくる
			Hr = createBuffer(256, ConstBuffer0);
			assert(SUCCEEDED(Hr));
			//マップしておく
			Hr = mapBuffer(ConstBuffer0, (void**)&CB0);
			assert(SUCCEEDED(Hr));
			//ビュー
			CbvTbvIdx = createConstantBufferView(ConstBuffer0);
		}
		//　コンスタントバッファ１
		{
			//バッファをつくる
			Hr = createBuffer(256, ConstBuffer1);
			assert(SUCCEEDED(Hr));
			//マップしておく
			Hr = mapBuffer(ConstBuffer1, (void**)&CB1);
			assert(SUCCEEDED(Hr));
			//データを入れる
			CB1->ambient  = { ::Ambient[0],::Ambient[1],::Ambient[2],::Ambient[3] };
			CB1->diffuse  = { ::Diffuse[0],::Diffuse[1],::Diffuse[2],::Diffuse[3] };
			CB1->specular = { ::Specular[0],::Specular[1],::Specular[2],::Specular[3] };
			//ビュー
			createConstantBufferView(ConstBuffer1);
		}
		//　テクスチャバッファ
		{
			Hr = createTextureBuffer(::TextureFilename, TextureBuffer);
			assert(SUCCEEDED(Hr));
			createTextureBufferView(TextureBuffer);
		}

		//ディファードレンダリングの準備
		createPipelinesAndResourcesForDeferred();
	}

	//メインループ
	while (!quit())
	{
		//更新------------------------------------------------------------------
		//回転用ラジアン
		static float r = 0;
		r += 0.01f;
		//ワールドマトリックス
		XMMATRIX world = XMMatrixRotationY(sinf(r));
		//ビューマトリックス
		XMFLOAT3 eye = { 0, 0, -2 }, focus = { 0, 0, 0 }, up = { 0, 1, 0 };
		XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&up));
		//プロジェクションマトリックス
		XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect(), 1.0f, 10.0f);
		//コンスタントバッファ０更新
		CB0->lightPos = { 0, 0, -1, 0 };
		CB0->eyePos = { eye.x, eye.y, eye.z, 0 };
		CB0->worldViewProj = world * view * proj;
		CB0->world = world;

		//描画------------------------------------------------------------------
#if 0
		setClearColor(0.2f, 0.2f, 0.2f);
		beginRender();
		drawMesh(Vbv, Ibv, CbvTbvIdx);
		endRender();
#else
		beginDeferredRender();
		drawMesh(Vbv, Ibv, CbvTbvIdx);
		endDeferredRender();
#endif
	}
	
	//後始末
	{
		waitGPU();
		closeEventHandle();
		unmapDeferredRenderConstant();
		unmapBuffer(ConstBuffer0);
		unmapBuffer(ConstBuffer1);
	}
}
