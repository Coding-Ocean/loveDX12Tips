#include"graphic.h"
#include"model.h"

//システム----------------------------------------------------------------------
HRESULT Hr;
ComPtr<ID3D12Device> Device;
ComPtr<ID3D12GraphicsCommandList> CommandList;
//リソース----------------------------------------------------------------------
//頂点バッファ
UINT NumVertices = 0;
ComPtr<ID3D12Resource> VertexBuf = nullptr;
D3D12_VERTEX_BUFFER_VIEW VertexBufView;
//頂点インデックスバッファ
UINT NumIndices = 0;
ComPtr<ID3D12Resource> IndexBuf = nullptr;
D3D12_INDEX_BUFFER_VIEW	IndexBufView;
//コンスタントバッファ
struct CONST_BUF0 {
	XMMATRIX worldViewProj;
};
struct CONST_BUF1 {
	XMFLOAT4 diffuse;
};
CONST_BUF0* CB0 = nullptr;
CONST_BUF1* CB1 = nullptr;
ComPtr<ID3D12Resource> ConstBuf0 = nullptr;
ComPtr<ID3D12Resource> ConstBuf1 = nullptr;
//テクスチャバッファ
ComPtr<ID3D12Resource> TextureBuf = nullptr;
//ディスクリプタヒープ
ComPtr<ID3D12DescriptorHeap> CbvTbvHeap = nullptr;// ConstBufView と TextureBufView の Heap
UINT CbvTbvIncSize = 0;

//エントリーポイント
INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	Device = device();
	CommandList = commandList();

	//リソース
	{
		//頂点バッファ
		{
			//サイズを求める
			UINT sizeInBytes = sizeof(Vertices);
			UINT strideInBytes = sizeof(float) * NumVertexElements;
			NumVertices = sizeInBytes / strideInBytes;

			//バッファをつくる
			Hr = createBuffer(sizeInBytes, VertexBuf);
			assert(SUCCEEDED(Hr));

			//バッファにデータをコピー
			UINT8* mappedBuf;
			Hr = VertexBuf->Map(0, nullptr, reinterpret_cast<void**>(&mappedBuf));
			assert(SUCCEEDED(Hr));
			memcpy(mappedBuf, Vertices, sizeInBytes);
			VertexBuf->Unmap(0, nullptr);

			//バッファのビューを初期化しておく。（ディスクリプタヒープに作らなくてよい）
			VertexBufView.BufferLocation = VertexBuf->GetGPUVirtualAddress();
			VertexBufView.SizeInBytes = sizeInBytes;
			VertexBufView.StrideInBytes = strideInBytes;
		}
		//頂点インデックスバッファ
		{
			//サイズを求める
			UINT sizeInBytes = sizeof(Indices);
			NumIndices =  sizeInBytes / sizeof(unsigned short);

			//インデックスバッファをつくる
			Hr = createBuffer(sizeInBytes, IndexBuf);
			assert(SUCCEEDED(Hr));

			//作ったバッファにデータをコピー
			UINT8* mappedBuf = nullptr;
			Hr = IndexBuf->Map(0, nullptr, (void**)&mappedBuf);
			assert(SUCCEEDED(Hr));
			memcpy(mappedBuf, Indices, sizeInBytes);
			IndexBuf->Unmap(0, nullptr);

			//インデックスバッファビューをつくる
			IndexBufView.BufferLocation = IndexBuf->GetGPUVirtualAddress();
			IndexBufView.SizeInBytes = sizeInBytes;
			IndexBufView.Format = DXGI_FORMAT_R16_UINT;
		}
		//コンスタントバッファ０
		{
			Hr = createBuffer(256, ConstBuf0);
			assert(SUCCEEDED(Hr));

			//マップする。コンスタントバッファはUnmapしない
			Hr = ConstBuf0->Map(0, nullptr, (void**)&CB0);
			assert(SUCCEEDED(Hr));
		}
		//コンスタントバッファ１
		{
			Hr = createBuffer(256, ConstBuf0);
			assert(SUCCEEDED(Hr));

			//マップする。コンスタントバッファはUnmapしない。
			Hr = ConstBuf1->Map(0, nullptr, (void**)&CB1);
			assert(SUCCEEDED(Hr));
			//コンスタントバッファ１更新
			CB1->diffuse = {Diffuse[0],Diffuse[1],Diffuse[2],Diffuse[3]};
		}
		//テクスチャバッファ
		{
			createTextureBuffer(TextureFilename, TextureBuf);
		}
		//ディスクリプタヒープ
		{
			//「ビュー」の入れ物である「ディスクリプタヒープ」をつくる
			{
				D3D12_DESCRIPTOR_HEAP_DESC desc = {};
				desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
				desc.NumDescriptors = 3;//コンスタントバッファ２つとテクスチャバッファ１つ
				desc.NodeMask = 0;
				desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
				Hr = Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(CbvTbvHeap.ReleaseAndGetAddressOf()));
				assert(SUCCEEDED(Hr));
			}

			//ハンドル(ポインタ)増分サイズを取得しておく
			CbvTbvIncSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			//ヒープの先頭アドレスをハンドルに取得
			auto hCbvTbvHeap = CbvTbvHeap->GetCPUDescriptorHandleForHeapStart();

			//コンスタントバッファ０の「ビュー」を「ディスクリプタヒープ」につくる
			{
				D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
				desc.BufferLocation = ConstBuf0->GetGPUVirtualAddress();
				desc.SizeInBytes = static_cast<UINT>(ConstBuf0->GetDesc().Width);
				Device->CreateConstantBufferView(&desc, hCbvTbvHeap);
			}

			hCbvTbvHeap.ptr += CbvTbvIncSize;

			//コンスタントバッファ１の「ビュー」を「ディスクリプタヒープ」につくる
			{
				D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
				desc.BufferLocation = ConstBuf1->GetGPUVirtualAddress();
				desc.SizeInBytes = static_cast<UINT>(ConstBuf1->GetDesc().Width);
				Device->CreateConstantBufferView(&desc, hCbvTbvHeap);
			}

			hCbvTbvHeap.ptr += CbvTbvIncSize;

			//テクスチャバッファの「ビュー」を「ディスクリプタヒープ」につくる
			{
				D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
				desc.Format = TextureBuf->GetDesc().Format;
				desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
				desc.Texture2D.MipLevels = 1;//ミップマップは使用しないので1
				Device->CreateShaderResourceView(TextureBuf.Get(), &desc, hCbvTbvHeap);
			}
		}
	}{}

	//メインループ
	while (true)
	{
		//ウィンドウメッセージの取得、送出
		{
			MSG msg = { 0 };
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				if (msg.message == WM_QUIT)
					break;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				continue;
			}
		}

		//コンスタントバッファ更新
		{
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
		}

		//バックバッファに描画
		{
			//頂点をセット
			CommandList->IASetVertexBuffers(0, 1, &VertexBufView);
			CommandList->IASetIndexBuffer(&IndexBufView);

			//ディスクリプタヒープをＧＰＵにセット
			CommandList->SetDescriptorHeaps(1, CbvTbvHeap.GetAddressOf());
			//ディスクリプタヒープをディスクリプタテーブルにセット
			auto hCbvTbvHeap = CbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
			CommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);
			//描画
			CommandList->DrawIndexedInstanced(NumIndices, 1, 0, 0, 0);
		}
	}{}
	//解放
	{
		waitDrawDone();
		ConstBuf1->Unmap(0, nullptr);
		ConstBuf0->Unmap(0, nullptr);
		CloseHandle(FenceEvent);
	}
}
