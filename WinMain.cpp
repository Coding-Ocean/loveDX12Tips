#include<sstream>
#include"graphic.h"
#include"model.h"

//Grobal variables
//背景カラー
const float ClearColor[] = { 0.25f, 0.5f, 0.9f, 1.0f };
//頂点バッファ
UINT NumVertices = 0;
ComPtr<ID3D12Resource>   VertexBuffer = nullptr;
D3D12_VERTEX_BUFFER_VIEW Vbv;
//頂点インデックスバッファ
UINT NumIndices = 0;
ComPtr<ID3D12Resource>  IndexBuffer = nullptr;
D3D12_INDEX_BUFFER_VIEW	Ibv;
//コンスタントバッファ
struct CONST_BUF0 {
	XMMATRIX worldViewProj;
};
struct CONST_BUF1 {
	XMFLOAT4 diffuse;
};
CONST_BUF0* CB0 = nullptr;
CONST_BUF1* CB1 = nullptr;
ComPtr<ID3D12Resource> ConstBuffer0 = nullptr;
ComPtr<ID3D12Resource> ConstBuffer1 = nullptr;
constexpr UINT NumConstBuffers = 2;//これも用意しておこう
//テクスチャバッファ
constexpr UINT NumTextureBuffers = 8;//複数のバッファを用意する
ComPtr<ID3D12Resource> TextureBuffers[NumTextureBuffers];//配列にします
//これでディスクリプタヒープの場所を指す
int CbvIdx = 0;
int TbvIdx = 0;

//Entry point
INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"Texture Animation", 1280, 720);
	setClearColor(0.25f, 0.5f, 0.9f);

	HRESULT Hr;

	Hr = createDescriptorHeap(NumConstBuffers + NumTextureBuffers);
	assert(SUCCEEDED(Hr));

	//リソース初期化
	{
		//頂点バッファ
		{
			//データサイズを求めておく
			UINT sizeInBytes = sizeof(Vertices);
			UINT strideInBytes = sizeof(float) * NumVertexElements;
			NumVertices = sizeInBytes / strideInBytes;
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
			NumIndices =  sizeInBytes / sizeof(UINT16);
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
			Hr = createBuffer(256, ConstBuffer0);
			assert(SUCCEEDED(Hr));
			//マップしておく
			Hr = mapBuffer(ConstBuffer0, (void**)&CB0);
			assert(SUCCEEDED(Hr));

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
			CB1->diffuse = {Diffuse[0],Diffuse[1],Diffuse[2],Diffuse[3]};
		
			createConstantBufferView(ConstBuffer1);
		}
		//テクスチャバッファ
		{
			//ファイルを読み込み、テクスチャバッファをつくる
			for (int i = 0; i < NumTextureBuffers; ++i) {
				std::ostringstream filename;
				filename << "assets\\lady\\a" << i << ".png";
				createTextureBuffer(filename.str().c_str(), TextureBuffers[i]);
			
				TbvIdx = createTextureBufferView(TextureBuffers[i]);
			}
		}
	}
	
	//描画用にクローンしておく
	ComPtr<ID3D12GraphicsCommandList>& CommandList = commandList();

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
				TbvIdx++;
				if (TbvIdx >= CbvIdx + NumConstBuffers + NumTextureBuffers) {
					TbvIdx = CbvIdx + NumConstBuffers;
				}
			}
		}

		//描画------------------------------------------------------------------
		beginRender();
		//！！！この関数を使用するにはルートシグネチャの変更が必要！！！
		drawMesh(Vbv, Ibv, CbvIdx, TbvIdx);
		endRender();
	}
	
	//後始末
	{
		waitGPU();
		closeEventHandle();
		ConstBuffer1->Unmap(0, nullptr);
		ConstBuffer0->Unmap(0, nullptr);
	}
}
