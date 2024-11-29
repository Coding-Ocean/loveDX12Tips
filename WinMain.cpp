#include<sstream>
#include"graphic.h"
#include"model.h"

//メッシュデータ
//　頂点バッファ
ComPtr<ID3D12Resource>   VertexBuffer = nullptr;
D3D12_VERTEX_BUFFER_VIEW Vbv;
//　頂点インデックスバッファ
ComPtr<ID3D12Resource>  IndexBuffer = nullptr;
D3D12_INDEX_BUFFER_VIEW	Ibv;
//　コンスタントバッファの数も用意しておく
constexpr UINT NumConstBuffers = 2;
//　コンスタントバッファ０
ComPtr<ID3D12Resource> ConstBuffer0 = nullptr;
CONST_BUF0* CB0 = nullptr;
//　コンスタントバッファ１
ComPtr<ID3D12Resource> ConstBuffer1 = nullptr;
CONST_BUF1* CB1 = nullptr;
//　テクスチャバッファ
constexpr UINT NumTextureBuffers = 8;//複数のバッファを用意する
ComPtr<ID3D12Resource> TextureBuffers[NumTextureBuffers];//配列にします
//　これでディスクリプタの場所を指す
UINT CbvIdx = 0;
UINT TbvIdxs[NumTextureBuffers] = {};//インデックス配列
UINT TbvIdx = 0;//インデックス配列を指すインデックス

//Entry point
INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"Texture Animation", 1280, 720);
	setClearColor(0.25f, 0.5f, 0.9f);

	HRESULT Hr;

	//最初に必要なコンスタントバッファビュー、テクスチャバッファビューのヒープを用意しておく
	Hr = createDescriptorHeap(NumConstBuffers + NumTextureBuffers);
	assert(SUCCEEDED(Hr));

	//リソースをつくる
	{
		//頂点バッファ
		{
			//データサイズを求めておく
			UINT sizeInBytes = sizeof(::Vertices);
			UINT strideInBytes = sizeof(float) * NumVertexElements;
			//バッファをつくる
			Hr = createBuffer(sizeInBytes, VertexBuffer);
			assert(SUCCEEDED(Hr));
			//バッファにデータを入れる
			Hr = updateBuffer(::Vertices, sizeInBytes, VertexBuffer);
			assert(SUCCEEDED(Hr));
			//ビューをつくる
			createVertexBufferView(VertexBuffer, sizeInBytes, strideInBytes, Vbv);
		}
		//頂点インデックスバッファ
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
		//コンスタントバッファ０
		{
			//バッファをつくる
			Hr = createBuffer(256, ConstBuffer0);
			assert(SUCCEEDED(Hr));
			//マップしておく
			Hr = mapBuffer(ConstBuffer0, (void**)&CB0);
			assert(SUCCEEDED(Hr));
			//ビューをつくって、インデックスをもらっておく
			CbvIdx = createConstantBufferView(ConstBuffer0);
		}
		//コンスタントバッファ１
		{
			//バッファをつくる
			Hr = createBuffer(256, ConstBuffer1);
			assert(SUCCEEDED(Hr));
			//マップしておく
			Hr = mapBuffer(ConstBuffer1, (void**)&CB1);
			assert(SUCCEEDED(Hr));
			//データを入れる
			CB1->diffuse = {::Diffuse[0],::Diffuse[1],::Diffuse[2],::Diffuse[3]};
			//変更しないのでアンマップする
			unmapBuffer(ConstBuffer1);
			//ビューをつくる
			createConstantBufferView(ConstBuffer1);
		}
		//テクスチャバッファ
		{
			//ファイルを読み込み、テクスチャバッファをつくる
			for (int i = 0; i < NumTextureBuffers; ++i) {
				//バッファをつくる
				std::ostringstream filename;
				filename << "assets\\lady\\a" << i << ".png";
				createTextureBuffer(filename.str().c_str(), TextureBuffers[i]);
				//ビューをつくって、インデックスをもらっておく
				TbvIdxs[i] = createTextureBufferView(TextureBuffers[i]);
			}
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
		XMMATRIX world = XMMatrixRotationY(r)*XMMatrixTranslation(-sin(r),0,-cos(r));
		//ビューマトリックス
		XMFLOAT3 eye = { 0, 0, -2.1f }, focus = { 0, 0, 0 }, up = { 0, 1, 0 };
		XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&up));
		//プロジェクションマトリックス
		XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect(), 1.0f, 10.0f);
		CB0->worldViewProj = world * view * proj;

		//一定間隔でテクスチャインデックスをカウントアップする
		{
			static UINT count = 0;
			int interval = 7;
			//countがintervalで割り切れた時にTbvIdxをカウントアップする
			if (++count % interval == 0) {
				count = 0;
				++TbvIdx %= NumTextureBuffers;
			}
		}

		//描画------------------------------------------------------------------
		beginRender();
		//！！！この関数を使用するにはルートシグネチャの変更が必要！！！
		drawMesh(Vbv, Ibv, CbvIdx, TbvIdxs[TbvIdx]);
		endRender();
	}
	
	//後始末
	{
		waitGPU();
		closeEventHandle();
		unmapBuffer(ConstBuffer0);
	}
}
