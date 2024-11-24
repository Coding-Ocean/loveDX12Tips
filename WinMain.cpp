#include"graphic.h"
#include"model.h"

//Grobal variables
//頂点バッファ
UINT NumVertices = 0;
ComPtr<ID3D12Resource>   VertexBuffer = nullptr;
D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
//頂点インデックスバッファ
UINT NumIndices = 0;
ComPtr<ID3D12Resource>  IndexBuffer = nullptr;
D3D12_INDEX_BUFFER_VIEW	IndexBufferView;
//コンスタントバッファ
struct CONST_BUF0 {
	XMMATRIX worldViewProj;
	XMMATRIX world;
	XMFLOAT4 lightPos;
	XMFLOAT4 eyePos;
};
struct CONST_BUF1 {
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
};
struct CONST_BUF2 {
	XMFLOAT4 offset;
};
CONST_BUF0* CB0 = nullptr;
CONST_BUF1* CB1 = nullptr;
ComPtr<ID3D12Resource> ConstBuffer0 = nullptr;
ComPtr<ID3D12Resource> ConstBuffer1 = nullptr;
//テクスチャバッファ
ComPtr<ID3D12Resource> TextureBuffer = nullptr;
//ディスクリプタヒープ
ComPtr<ID3D12DescriptorHeap> CbvTbvHeap = nullptr;
UINT CbvTbvIncSize = 0;

//===
CONST_BUF2* CB2_1 = nullptr;
CONST_BUF2* CB2_2 = nullptr;
ComPtr<ID3D12Resource> ConstBuffer2_1 = nullptr;
ComPtr<ID3D12Resource> ConstBuffer2_2 = nullptr;
ComPtr<ID3D12Resource>   VertexBuffer1 = nullptr;
D3D12_VERTEX_BUFFER_VIEW VertexBufferView1;

//Entry point
INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	int w = 720, h = 720;
	window(L"Deferred Rendering", w, h);

	HRESULT Hr;

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
			createVertexBufferView(VertexBuffer, sizeInBytes, strideInBytes, VertexBufferView);
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
			createIndexBufferView(IndexBuffer, sizeInBytes, IndexBufferView);
		}
		//コンスタントバッファ０
		{
			//バッファをつくる
			Hr = createBuffer(256, ConstBuffer0);
			assert(SUCCEEDED(Hr));
			//マップしておく
			Hr = mapBuffer(ConstBuffer0, (void**)&CB0);
			assert(SUCCEEDED(Hr));
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
			CB1->ambient = { Ambient[0],Ambient[1],Ambient[2],Ambient[3] };
			CB1->diffuse = { Diffuse[0],Diffuse[1],Diffuse[2],Diffuse[3] };
			CB1->specular = { Specular[0],Specular[1],Specular[2],Specular[3] };
		}
		//テクスチャバッファ
		{
			Hr = createTextureBuffer(TextureFilename, TextureBuffer);
			assert(SUCCEEDED(Hr));
		}

		//=== コンスタントバッファ２ === ここでつくるのは分かりずらい！！なんとかせねば。
		{
			//バッファをつくる
			Hr = createBuffer(256, ConstBuffer2_1);
			assert(SUCCEEDED(Hr));
			//マップしておく
			Hr = mapBuffer(ConstBuffer2_1, (void**)&CB2_1);
			assert(SUCCEEDED(Hr));
			//データを入れる
			CB2_1->offset = { 1.0f/w, 0.0f, 0.0f, 0.0f };
		}
		{
			//バッファをつくる
			Hr = createBuffer(256, ConstBuffer2_2);
			assert(SUCCEEDED(Hr));
			//マップしておく
			Hr = mapBuffer(ConstBuffer2_2, (void**)&CB2_2);
			assert(SUCCEEDED(Hr));
			//データを入れる
			CB2_2->offset = { 0.0f, 1.0f/h, 0.0f, 0.0f };
		}

		//ディスクリプタヒープ
		{
			//ディスクリプタ(ビュー)７つ分のヒープをつくる
			Hr = createDescriptorHeap(3+4, CbvTbvHeap);
			assert(SUCCEEDED(Hr));
			CbvTbvIncSize = cbvTbvIncSize();
			//１つめのディスクリプタ(ビュー)をヒープにつくる
			auto hCbvTbvHeap = CbvTbvHeap->GetCPUDescriptorHandleForHeapStart();
			createConstantBufferView(ConstBuffer0, hCbvTbvHeap);//0
			//２つめのディスクリプタ(ビュー)をヒープにつくる
			hCbvTbvHeap.ptr += CbvTbvIncSize;
			createConstantBufferView(ConstBuffer1, hCbvTbvHeap);//1
			//３つめのディスクリプタ(ビュー)をヒープにつくる
			hCbvTbvHeap.ptr += CbvTbvIncSize;
			createTextureBufferView(TextureBuffer, hCbvTbvHeap);//2

			//===
			hCbvTbvHeap.ptr += CbvTbvIncSize;
			createConstantBufferView(ConstBuffer2_1, hCbvTbvHeap);//3
			hCbvTbvHeap.ptr += CbvTbvIncSize;
			createTextureBufferView(0,hCbvTbvHeap);//4
			hCbvTbvHeap.ptr += CbvTbvIncSize;
			createConstantBufferView(ConstBuffer2_2, hCbvTbvHeap);//5
			hCbvTbvHeap.ptr += CbvTbvIncSize;
			createTextureBufferView(1,hCbvTbvHeap);//6
		}
	}
	//頂点バッファ1
	{
		float vertices[] = {
			-1, 1,0,  0,0,
			-1,-1,0,  0,1,
			 1, 1,0,  1,0,
			 1,-1,0,  1,1,
		};
		//データサイズを求めておく
		UINT sizeInBytes = sizeof(vertices);
		UINT strideInBytes = sizeof(float) * 5;
		//バッファをつくる
		Hr = createBuffer(sizeInBytes, VertexBuffer1);
		assert(SUCCEEDED(Hr));
		//バッファにデータを入れる
		Hr = updateBuffer(vertices, sizeInBytes, VertexBuffer1);
		assert(SUCCEEDED(Hr));
		//バッファビューをつくる
		createVertexBufferView(VertexBuffer1, sizeInBytes, strideInBytes, VertexBufferView1);
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
		XMMATRIX world = XMMatrixRotationY(sin(r));
		//ビューマトリックス
		XMFLOAT3 eye = { 0, 0, -2.0f }, focus = { 0, 0, 0 }, up = { 0, 1, 0 };
		XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&up));
		//プロジェクションマトリックス
		XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect(), 1.0f, 10.0f);

		CB0->worldViewProj = world * view * proj;
		CB0->world = world;
		CB0->lightPos = { 0,0,-0.01f,0 };
		CB0->eyePos = { eye.x,eye.y,eye.z,0 };

		//描画------------------------------------------------------------------
		//バックバッファクリア
		clear(ClearColor);

		//ディスクリプタヒープをＧＰＵにセット
		CommandList->SetDescriptorHeaps(1, CbvTbvHeap.GetAddressOf());

		//頂点をセット
		CommandList->IASetVertexBuffers(0, 1, &VertexBufferView);
		CommandList->IASetIndexBuffer(&IndexBufferView);
		//ディスクリプタヒープをディスクリプタテーブルにセット
		auto hCbvTbvHeap = CbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
		CommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);
		//描画
		CommandList->DrawIndexedInstanced(NumIndices, 1, 0, 0, 0);
		

		//===
		CommandList->IASetVertexBuffers(0, 1, &VertexBufferView1);
		
		postTransition1();
		hCbvTbvHeap.ptr += CbvTbvIncSize * 3;
		CommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);
		CommandList->DrawInstanced(4, 1, 0, 0);

		postTransition2();
		hCbvTbvHeap.ptr += CbvTbvIncSize * 2;
		CommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);
		CommandList->DrawInstanced(4, 1, 0, 0);
		
		postTransition3();
		hCbvTbvHeap.ptr -= CbvTbvIncSize * 2;
		CommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);
		CommandList->DrawInstanced(4, 1, 0, 0);

		//バックバッファ表示
		present();
	}
	
	//後始末
	{
		waitGPU();
		closeEventHandle();
		ConstBuffer1->Unmap(0, nullptr);
		ConstBuffer0->Unmap(0, nullptr);
	}
}
