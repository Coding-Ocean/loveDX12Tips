#include"graphic.h"
#include"model.h"

//頂点バッファ
ComPtr<ID3D12Resource>   VertexBuffer = nullptr;
D3D12_VERTEX_BUFFER_VIEW Vbv;
//頂点インデックスバッファ
ComPtr<ID3D12Resource>  IndexBuffer = nullptr;
D3D12_INDEX_BUFFER_VIEW	Ibv;
//コンスタントバッファ
ComPtr<ID3D12Resource> ConstBuffer0 = nullptr;
ComPtr<ID3D12Resource> ConstBuffer1 = nullptr;
CONST_BUF0* CB0 = nullptr;
CONST_BUF1* CB1 = nullptr;
//テクスチャバッファ
ComPtr<ID3D12Resource> TextureBuffer = nullptr;
//graphic.cppにあるCbvTbvHeap上のインデックス
UINT CbvTbvIdx;

//Entry point
INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"Graphic Functions2", 1280, 720);
	setClearColor(0.25f, 0.5f, 0.9f);

	HRESULT Hr;

	//リソース初期化
	{
		//最初に必要な数のコンスタントバッファ系ディスクリプタヒープをつくっておく
		{
			Hr = createDescriptorHeap(3);
			assert(SUCCEEDED(Hr));
		}

		//１つのメッシュをつくっていく
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
			//ビューをつくる
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
			//ビューをつくる
			createIndexBufferView(IndexBuffer, sizeInBytes, Ibv);
		}
		//　コンスタントバッファ０
		{
			//バッファをつくる
			Hr = createBuffer(alignedSize(sizeof(CB0)), ConstBuffer0);
			assert(SUCCEEDED(Hr));
			//マップしておく
			Hr = mapBuffer(ConstBuffer0, (void**)&CB0);
			assert(SUCCEEDED(Hr));
			//ビューを作り、そのインデックスを取得しておく
			CbvTbvIdx = createConstantBufferView(ConstBuffer0);
		}
		//　コンスタントバッファ１
		{
			//バッファをつくる
			Hr = createBuffer(alignedSize(sizeof(CB1)), ConstBuffer1);
			assert(SUCCEEDED(Hr));
			//マップしておく
			Hr = mapBuffer(ConstBuffer1, (void**)&CB1);
			assert(SUCCEEDED(Hr));
			//データを入れる
			CB1->diffuse = { ::Diffuse[0],::Diffuse[1],::Diffuse[2],::Diffuse[3] };
			//ビューをつくる
			createConstantBufferView(ConstBuffer1);
		}
		//　テクスチャバッファ
		{
			//バッファをつくる
			Hr = createTextureBuffer(::TextureFilename, TextureBuffer);
			assert(SUCCEEDED(Hr));
			//ビューをつくる
			createTextureBufferView(TextureBuffer);
		}
	}
	
	//メインループ
	while (!quit())
	{
		//更新------------------------------------------------------------------
		//回転用ラジアン
		static float r = 0;
		r += 0.01f;
		//ワールドマトリックス
		XMMATRIX world = XMMatrixRotationY(r);
		//ビューマトリックス
		XMFLOAT3 eye = { 0, 0, -2 }, focus = { 0, 0, 0 }, up = { 0, 1, 0 };
		XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&up));
		//プロジェクションマトリックス
		XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect(), 1.0f, 10.0f);
		CB0->worldViewProj = world * view * proj;

		//描画------------------------------------------------------------------
		beginDraw();
		drawMesh(Vbv, Ibv, CbvTbvIdx);
		endDraw();
	}
	
	//後始末
	{
		waitGPU();
		closeEventHandle();
		unmapBuffer(ConstBuffer1);
		unmapBuffer(ConstBuffer0);
	}
}
