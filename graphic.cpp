#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"winmm.lib")

#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
#include"d3dx12.h"
#include<dxgi1_6.h>
#include<cassert>
#include<map>
#include<unordered_map>
#include<memory>
#include"BIN_FILE12.h"
#include"graphic.h"
#include"window.h"

//グローバル変数-----------------------------------------------------------------
// デバイス
ComPtr<ID3D12Device> Device;
// コマンド
ComPtr<ID3D12CommandAllocator> CommandAllocator;
ComPtr<ID3D12GraphicsCommandList> CommandList;
ComPtr<ID3D12CommandQueue> CommandQueue;
//===
// コマンド２。テクスチャをつくる時に使用
ComPtr<ID3D12CommandAllocator> CommandAllocator2;
ComPtr<ID3D12GraphicsCommandList> CommandList2;
// フェンス
ComPtr<ID3D12Fence> Fence;
HANDLE FenceEvent;
UINT64 FenceValue;
// デバッグ
HRESULT Hr;
// バックバッファ
ComPtr<IDXGISwapChain4> SwapChain;
ComPtr<ID3D12Resource> BackBuffers[2];
UINT BackBufIdx;
ComPtr<ID3D12DescriptorHeap> BbvHeap;//"Bbv"は"BackBufView"の略
UINT BbvIncSize;
const DXGI_FORMAT BACK_BUFFER_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
float ClearColor[] = { 0.f,0.f,0.f,1 };
// デプスステンシルバッファ
ComPtr<ID3D12Resource> DepthStencilBuffer;
ComPtr<ID3D12DescriptorHeap> DsvHeap;//"Dsv"は"DepthStencilBufferView"の略
const DXGI_FORMAT DEPTH_STENCIL_FORMAT = DXGI_FORMAT_D32_FLOAT;
// パイプライン
ComPtr<ID3D12RootSignature> RootSignature;
ComPtr<ID3D12PipelineState> PipelineState;
D3D12_VIEWPORT Viewport;
D3D12_RECT ScissorRect;
//　コンスタントおよびテクスチャ用ディスクリプタヒープ
ComPtr<ID3D12DescriptorHeap> CbvTbvHeap;
UINT MaxCbvTbvIdxs = 0;
UINT CbvTbvIncSize = 0;
UINT CurrentCbvTbvIdx = 0;
//  MSAA
ComPtr<ID3D12Resource> MsaaRenderBuffer;
ComPtr<ID3D12DescriptorHeap> MsaaRtvHeap;
ComPtr<ID3D12Resource> MsaaDepthStencilBuffer;
ComPtr<ID3D12DescriptorHeap> MsaaDsvHeap;
UINT SAMPLE_COUNT = 8;
UINT SampleCount = 1;

//プライベートな関数--------------------------------------------------------------
void CreateDevice()
{
#ifdef _DEBUG
	//デバッグモードでは、デバッグレイヤーを有効化する
	ComPtr<ID3D12Debug> debug;
	Hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
	assert(SUCCEEDED(Hr));
	debug->EnableDebugLayer();
#endif
	//デバイスをつくる(簡易バージョン)
	{
		Hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0,
			IID_PPV_ARGS(Device.GetAddressOf()));
		assert(SUCCEEDED(Hr));
	}
	//コマンド
	{
		//コマンドアロケータをつくる
		Hr = Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(CommandAllocator.GetAddressOf()));
		assert(SUCCEEDED(Hr));

		//コマンドリストをつくる
		Hr = Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
			CommandAllocator.Get(), nullptr, IID_PPV_ARGS(CommandList.GetAddressOf()));
		assert(SUCCEEDED(Hr));

		//コマンドキューをつくる
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;		//GPUタイムアウトが有効
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;		//直接コマンドキュー
		Hr = Device->CreateCommandQueue(&desc, IID_PPV_ARGS(CommandQueue.GetAddressOf()));
		assert(SUCCEEDED(Hr));

		//コマンドアロケータをつくる
		Hr = Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(CommandAllocator2.GetAddressOf()));
		assert(SUCCEEDED(Hr));

		//コマンドリストをつくる
		Hr = Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
			CommandAllocator2.Get(), nullptr, IID_PPV_ARGS(CommandList2.GetAddressOf()));
		assert(SUCCEEDED(Hr));

	}
	//フェンス
	{
		//GPUの処理完了をチェックするフェンスをつくる
		Hr = Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(Fence.GetAddressOf()));
		assert(SUCCEEDED(Hr));
		FenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
		assert(FenceEvent != nullptr);
		FenceValue = 1;
	}
}
void CreateRenderTarget()
{
	//スワップチェインをつくる(ここにバックバッファが含まれている)
	{
		//DXGIファクトリをつくる
		ComPtr<IDXGIFactory4> dxgiFactory;
		Hr = CreateDXGIFactory2(0, IID_PPV_ARGS(dxgiFactory.GetAddressOf()));
		assert(SUCCEEDED(Hr));

		//スワップチェインをつくる
		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.BufferCount = 2; //バックバッファ2枚
		desc.Width = (UINT)clientWidth();
		desc.Height = (UINT)clientHeight();
		desc.Format = BACK_BUFFER_FORMAT;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.SampleDesc.Count = 1;
		ComPtr<IDXGISwapChain1> swapChain1;
		Hr = dxgiFactory->CreateSwapChainForHwnd(
			CommandQueue.Get(), hwnd(), &desc, nullptr, nullptr, swapChain1.GetAddressOf());
		assert(SUCCEEDED(Hr));

		//IDXGISwapChain4インターフェイスをサポートしているか尋ねる
		Hr = swapChain1->QueryInterface(IID_PPV_ARGS(SwapChain.GetAddressOf()));
		assert(SUCCEEDED(Hr));
	}
	//バックバッファ「ビュー」の入れ物である「ディスクリプタヒープ」をつくる
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = 2;//バックバッファビュー２つ
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;//RenderTargetView
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;//シェーダからアクセスしないのでNONEでOK
		Hr = Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(BbvHeap.GetAddressOf()));
		assert(SUCCEEDED(Hr));
	}
	//バックバッファ「ビュー」を「ディスクリプタヒープ」につくる
	{
		D3D12_CPU_DESCRIPTOR_HANDLE hBbvHeap
			= BbvHeap->GetCPUDescriptorHandleForHeapStart();

		BbvIncSize
			= Device->GetDescriptorHandleIncrementSize(
				D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		for (UINT idx = 0; idx < 2; idx++) {
			//バックバッファを取り出す
			Hr = SwapChain->GetBuffer(idx, IID_PPV_ARGS(BackBuffers[idx].GetAddressOf()));
			assert(SUCCEEDED(Hr));
			//バックバッファのビューをヒープにつくる
			hBbvHeap.ptr += BbvIncSize * idx;
			Device->CreateRenderTargetView(BackBuffers[idx].Get(), nullptr, hBbvHeap);
		}
	}
	//デプスステンシルバッファをつくる
	{
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;//DEFAULTだから後はUNKNOWNでよし
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;//2次元のテクスチャデータとして
		desc.Width = (UINT)clientWidth();//幅と高さはレンダーターゲットと同じ
		desc.Height = (UINT)clientHeight();//上に同じ
		desc.DepthOrArraySize = 1;//テクスチャ配列でもないし3Dテクスチャでもない
		desc.Format = DEPTH_STENCIL_FORMAT;//深度値書き込み用フォーマット
		desc.SampleDesc.Count = 1;//サンプルは1ピクセル当たり1つ
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//このバッファは深度ステンシルとして使用します
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.MipLevels = 1;
		//デプスステンシルバッファをクリアする値
		D3D12_CLEAR_VALUE clearValue = {};
		clearValue.DepthStencil.Depth = 1.0f;//深さ１(最大値)でクリア
		clearValue.Format = DEPTH_STENCIL_FORMAT;//32bit深度値としてクリア
		//デプスステンシルバッファを作る
		Hr = Device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE, //デプス書き込みに使用
			&clearValue,
			IID_PPV_ARGS(DepthStencilBuffer.GetAddressOf()));
		assert(SUCCEEDED(Hr));
	}
	//デプスステンシルバッファ「ビュー」の入れ物である「デスクリプタヒープ」をつくる
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};//深度に使うよという事がわかればいい
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;//デプスステンシルビューとして使う
		desc.NumDescriptors = 1;//深度ビュー1つのみ
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		Hr = Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(DsvHeap.GetAddressOf()));
		assert(SUCCEEDED(Hr));
	}
	//デプスステンシルバッファ「ビュー」を「ディスクリプタヒープ」につくる
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
		desc.Format = DXGI_FORMAT_D32_FLOAT;//デプス値に32bit使用
		desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2Dテクスチャ
		desc.Flags = D3D12_DSV_FLAG_NONE;//フラグは特になし
		D3D12_CPU_DESCRIPTOR_HANDLE hDsvHeap
			= DsvHeap->GetCPUDescriptorHandleForHeapStart();
		Device->CreateDepthStencilView(DepthStencilBuffer.Get(), &desc, hDsvHeap);
	}
}
void CreateMsaaRenderTarget()
{
	//MSAAができるかチェック
	for (SampleCount = SAMPLE_COUNT; SampleCount > 1; SampleCount--){
		
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS levels = 
			{ BACK_BUFFER_FORMAT, SampleCount };
		
		if (FAILED(Device->CheckFeatureSupport(
			D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &levels, sizeof(levels))))
			continue;

		if (levels.NumQualityLevels > 0)
			break;
	}
	assert(SampleCount > 1);

	//MSAAレンダーバッファをつくる
	{
		CD3DX12_HEAP_PROPERTIES prop(D3D12_HEAP_TYPE_DEFAULT);
		D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
			BACK_BUFFER_FORMAT,
			(UINT)clientWidth(),
			(UINT)clientHeight(),
			1, // only one texture.
			1, // mipmap level
			SampleCount,
			0, // quality
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
		D3D12_CLEAR_VALUE clearValue = {};
		clearValue.Format = BACK_BUFFER_FORMAT;
		memcpy(clearValue.Color, ClearColor, sizeof(float) * 4);
		Device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_RESOLVE_SOURCE,
			&clearValue,
			IID_PPV_ARGS(MsaaRenderBuffer.ReleaseAndGetAddressOf())
		);
	}
	//MSAAレンダーバッファ「ビュー」の入れ物である「ディスクリプタヒープ」をつくる
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = 1;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		Device->CreateDescriptorHeap(&desc,	IID_PPV_ARGS(MsaaRtvHeap.ReleaseAndGetAddressOf()));
	}
	//MSAAレンダーバッファ「ビュー」をつくる
	{
		D3D12_RENDER_TARGET_VIEW_DESC desc = {};
		desc.Format = BACK_BUFFER_FORMAT;
		desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
		auto hMsaaRtvHeap = MsaaRtvHeap->GetCPUDescriptorHandleForHeapStart();
		Device->CreateRenderTargetView(MsaaRenderBuffer.Get(), &desc, hMsaaRtvHeap);
	}

	//MSAAデプスステンシルバッファをつくる
	{
		CD3DX12_HEAP_PROPERTIES prop(D3D12_HEAP_TYPE_DEFAULT);
		D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
			DEPTH_STENCIL_FORMAT,
			(UINT)clientWidth(),
			(UINT)clientHeight(),
			1, // only one texture.
			1, // mipmap level.
			SampleCount,
			0, // quality
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
		D3D12_CLEAR_VALUE clearValue = {};
		clearValue.Format = DEPTH_STENCIL_FORMAT;
		clearValue.DepthStencil.Depth = 1.0f;
		clearValue.DepthStencil.Stencil = 0;
		Device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearValue,
			IID_PPV_ARGS(MsaaDepthStencilBuffer.ReleaseAndGetAddressOf())
		);
	}
	//MSAAデプスステンシルバッファ「ビュー」の入れ物である「ディスクリプタヒープ」をつくる
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = 1;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		Device->CreateDescriptorHeap(&desc,	IID_PPV_ARGS(MsaaDsvHeap.ReleaseAndGetAddressOf()));
	}
	//MSAAデプスステンシルバッファの「ビュー」をつくる
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
		desc.Format = DEPTH_STENCIL_FORMAT;
		desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
		auto hMsaaDsvHeap = MsaaDsvHeap->GetCPUDescriptorHandleForHeapStart();
		Device->CreateDepthStencilView(MsaaDepthStencilBuffer.Get(), &desc, hMsaaDsvHeap);
	}
}
void CreatePipeline()
{
	//ルートシグネチャ
	{
		//ディスクリプタレンジ。ディスクリプタヒープとシェーダを紐づける役割をもつ。
		D3D12_DESCRIPTOR_RANGE  range[2] = {};
		range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		range[0].BaseShaderRegister = 0;
		range[0].NumDescriptors = 1;
		range[0].RegisterSpace = 0;
		range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		range[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		range[1].BaseShaderRegister = 0;
		range[1].NumDescriptors = 1;
		range[1].RegisterSpace = 0;
		range[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		//ルートパラメタをディスクリプタテーブルとして使用
		D3D12_ROOT_PARAMETER rootParam[2] = {};
		rootParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParam[0].DescriptorTable.pDescriptorRanges = &range[0];
		rootParam[0].DescriptorTable.NumDescriptorRanges = 1;
		rootParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		rootParam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParam[1].DescriptorTable.pDescriptorRanges = &range[1];
		rootParam[1].DescriptorTable.NumDescriptorRanges = 1;
		rootParam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		//サンプラの記述。このサンプラがシェーダーの s0 にセットされる
		D3D12_STATIC_SAMPLER_DESC samplerDesc[1] = {};
		samplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;//補間しない(ニアレストネイバー)
		samplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;//横繰り返し
		samplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;//縦繰り返し
		samplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;//奥行繰り返し
		samplerDesc[0].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//ボーダーの時は黒
		samplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX;//ミップマップ最大値
		samplerDesc[0].MinLOD = 0.0f;//ミップマップ最小値
		samplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//オーバーサンプリングの際リサンプリングしない？
		samplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//ピクセルシェーダからのみ可視

		//ルートシグニチャの記述
		D3D12_ROOT_SIGNATURE_DESC desc = {};
		desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		desc.pParameters = rootParam;
		desc.NumParameters = _countof(rootParam);
		desc.pStaticSamplers = samplerDesc;//サンプラーの先頭アドレス
		desc.NumStaticSamplers = _countof(samplerDesc);//サンプラー数

		//ルートシグネチャをシリアライズ⇒blob(塊)をつくる。
		ComPtr<ID3DBlob> blob;
		Hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, nullptr);
		assert(SUCCEEDED(Hr));

		//ルートシグネチャをつくる
		Hr = Device->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(),
			IID_PPV_ARGS(RootSignature.GetAddressOf()));
		assert(SUCCEEDED(Hr));
	}

	//シェーダ読み込み
	BIN_FILE12 vs("assets\\VertexShader.cso");
	assert(vs.succeeded());
	BIN_FILE12 ps("assets\\PixelShader.cso");
	assert(ps.succeeded());

	UINT slot0 = 0;
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, slot0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    slot0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	D3D12_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.CullMode = D3D12_CULL_MODE_NONE;
	rasterDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	rasterDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	rasterDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.ForcedSampleCount = 0;
	rasterDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	D3D12_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].LogicOpEnable = false;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//全て書き込み
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;//小さい方を採用
	depthStencilDesc.StencilEnable = false;//ステンシルバッファは使わない

	//ここまでの記述をまとめてパイプラインステートオブジェクトをつくる
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};
	pipelineDesc.pRootSignature = RootSignature.Get();
	pipelineDesc.VS = { vs.code(), vs.size() };
	pipelineDesc.PS = { ps.code(), ps.size() };
	pipelineDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	pipelineDesc.RasterizerState = rasterDesc;
	pipelineDesc.BlendState = blendDesc;
	pipelineDesc.DepthStencilState = depthStencilDesc;
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pipelineDesc.SampleMask = UINT_MAX;
	//===
	pipelineDesc.SampleDesc.Count = SampleCount;
	
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineDesc.NumRenderTargets = 1;
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	Hr = Device->CreateGraphicsPipelineState(
		&pipelineDesc,
		IID_PPV_ARGS(PipelineState.GetAddressOf())
	);
	assert(SUCCEEDED(Hr));
}
void CreateDescriptorHeap(UINT numDescriptors)
{
	MaxCbvTbvIdxs = numDescriptors;

	//コンスタントバッファ、テクスチャバッファのディスクリプタヒープ
	CurrentCbvTbvIdx = 0;
	CbvTbvIncSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = numDescriptors;
	desc.NodeMask = 0;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	HRESULT hr = Device->CreateDescriptorHeap(
		&desc, IID_PPV_ARGS(CbvTbvHeap.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(Hr));
}
void SetViewport() {
	float aspect = baseWidth() / baseHeight();
	if (clientWidth() / clientHeight() >= aspect) {
		float vpWidth = clientHeight() * aspect;
		float left = clientWidth() - vpWidth;
		if (centered())left /= 2.0f;
		Viewport.TopLeftX = left;
		Viewport.TopLeftY = 0;
		Viewport.Width = vpWidth;
		Viewport.Height = clientHeight();
	}
	else {
		float vpHeight = clientWidth() / aspect;
		float top = (clientHeight() - vpHeight) / 2.0f;
		Viewport.TopLeftX = 0;
		Viewport.TopLeftY = top;
		Viewport.Width = clientWidth();
		Viewport.Height = vpHeight;
	}
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;

	//切り取り矩形を設定
	ScissorRect.left = 0;
	ScissorRect.top = 0;
	ScissorRect.right = (LONG)clientWidth();
	ScissorRect.bottom = (LONG)clientHeight();
}
void CreateSquareVertexBuffer();
void CreateCircleVertexBuffers();
void CreateWhiteTexture();
void CreateOrthoProj();
void InitConstantIdxCnt();
void InitPrintPosY();

//パブリックな関数---------------------------------------------------------------
//システム系
void createGraphic(int numDescriptors)
{
	CreateDevice();
	CreateRenderTarget();
	CreateMsaaRenderTarget();
	CreatePipeline();
	CreateDescriptorHeap(numDescriptors);
	SetViewport();
	//2D表示用
	CreateSquareVertexBuffer();
	CreateCircleVertexBuffers();
	CreateOrthoProj();
	CreateWhiteTexture();
}
void waitGPU()
{
	//現在のFence値がコマンド終了後にFenceに書き込まれるようにする
	UINT64 fvalue = FenceValue;
	CommandQueue->Signal(Fence.Get(), fvalue);
	FenceValue++;

	//まだコマンドキューが終了していないことを確認する
	if (Fence->GetCompletedValue() < fvalue)
	{
		//このFenceにおいて、fvalue の値になったらイベントを発生させる
		Fence->SetEventOnCompletion(fvalue, FenceEvent);
		//イベントが発生するまで待つ
		WaitForSingleObject(FenceEvent, INFINITE);
	}
}
void closeEventHandle()
{
	CloseHandle(FenceEvent);
}
//バッファ系
HRESULT createBuffer(UINT sizeInBytes, ComPtr<ID3D12Resource>& buffer)
{
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = D3D12_HEAP_TYPE_UPLOAD;
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	prop.CreationNodeMask = 1;
	prop.VisibleNodeMask = 1;
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment = 0;
	desc.Width = sizeInBytes;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc = { 1, 0 };
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;
	return Device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(buffer.ReleaseAndGetAddressOf()));
}
HRESULT updateBuffer(void* data, UINT sizeInBytes, ComPtr<ID3D12Resource>& buffer)
{
	UINT8* mappedBuffer;
	Hr = buffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedBuffer));
	if (FAILED(Hr))return E_FAIL;
	memcpy(mappedBuffer, data, sizeInBytes);
	buffer->Unmap(0, nullptr);
	return S_OK;
}
HRESULT mapBuffer(ComPtr<ID3D12Resource>& buffer, void** mappedBuffer)
{
	return buffer->Map(0, nullptr, mappedBuffer);
}
void unmapBuffer(ComPtr<ID3D12Resource>& buffer)
{
	buffer->Unmap(0, nullptr);
}
UINT alignedSize(size_t size)
{
	return (size+0xff)&~0xff;
}
HRESULT createTextureBuffer(unsigned char* pixels, UINT texWidth, UINT texHeight, ComPtr<ID3D12Resource>& TextureBuf)
{
	UINT bytePerPixel = 4;
	
	//１行のピッチを256の倍数にしておく(バッファサイズは256の倍数でなければいけない)
	const UINT64 alignedRowPitch = (texWidth * bytePerPixel + 0xff) & ~0xff;

	//アップロード用中間バッファをつくり、生データをコピーしておく
	ComPtr<ID3D12Resource> uploadBuf;
	{
		//テクスチャではなくフツーのバッファとしてつくる
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_UPLOAD;
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		prop.CreationNodeMask = 1;
		prop.VisibleNodeMask = 1;
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Alignment = 0;
		desc.Width = alignedRowPitch * texHeight;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc = { 1, 0 };
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		Hr = Device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadBuf));
		assert(SUCCEEDED(Hr));

		//生データをuploadbuffに一旦コピーします
		uint8_t* mapBuf = nullptr;
		Hr = uploadBuf->Map(0, nullptr, (void**)&mapBuf);//マップ
		auto srcAddress = pixels;
		auto originalRowPitch = texWidth * bytePerPixel;
		for (UINT y = 0; y < texHeight; ++y) {
			memcpy(mapBuf, srcAddress, originalRowPitch);
			//1行ごとの辻褄を合わせてやる
			mapBuf += alignedRowPitch;
			srcAddress += originalRowPitch;
		}
		uploadBuf->Unmap(0, nullptr);//アンマップ
	}

	//そして、最終コピー先であるテクスチャバッファを作る
	{
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;//CPUからアクセスしない。処理が速い。
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		prop.CreationNodeMask = 1;
		prop.VisibleNodeMask = 1;
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;//他のバッファと違う
		desc.Alignment = 0;
		desc.Width = texWidth;
		desc.Height = texHeight;//他のバッファと違う
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//他のバッファと違う
		desc.SampleDesc = { 1, 0 };
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;//他のバッファと違う
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		Hr = Device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(TextureBuf.ReleaseAndGetAddressOf()));
		assert(SUCCEEDED(Hr));
	}

	//GPUでuploadBufからtextureBufへコピーする長い道のりが始まります

	//まずコピー元ロケーションの準備・フットプリント指定
	D3D12_TEXTURE_COPY_LOCATION src = {};
	src.pResource = uploadBuf.Get();
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	src.PlacedFootprint.Footprint.Width = static_cast<UINT>(texWidth);
	src.PlacedFootprint.Footprint.Height = static_cast<UINT>(texHeight);
	src.PlacedFootprint.Footprint.Depth = static_cast<UINT>(1);
	src.PlacedFootprint.Footprint.RowPitch = static_cast<UINT>(alignedRowPitch);
	src.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//コピー先ロケーションの準備・サブリソースインデックス指定
	D3D12_TEXTURE_COPY_LOCATION dst = {};
	dst.pResource = TextureBuf.Get();
	dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dst.SubresourceIndex = 0;

	//コマンドリストでコピーを予約しますよ！！！
	CommandList2->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
	//ってことはバリアがいるのです
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = TextureBuf.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	CommandList2->ResourceBarrier(1, &barrier);
	//uploadBufアンロード
	CommandList2->DiscardResource(uploadBuf.Get(), nullptr);
	//コマンドリストを閉じて
	CommandList2->Close();
	//実行
	ID3D12CommandList* commandLists[] = { CommandList2.Get() };
	CommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
	//リソースがGPUに転送されるまで待機する
	waitGPU();

	//コマンドアロケータをリセット
	HRESULT Hr = CommandAllocator2->Reset();
	assert(SUCCEEDED(Hr));
	//コマンドリストをリセット
	Hr = CommandList2->Reset(CommandAllocator2.Get(), nullptr);
	assert(SUCCEEDED(Hr));

	return S_OK;
}
HRESULT createTextureBufferFromFile(const char* filename, ComPtr<ID3D12Resource>& textureBuffer, int* w, int* h)
{
	//ファイルを読み込み、生データを取り出す
	unsigned char* pixels = nullptr;
	int texWidth = 0, texHeight = 0, bytePerPixel = 4;
	pixels = stbi_load(filename, &texWidth, &texHeight, nullptr, bytePerPixel);
	if (pixels == nullptr)
	{
		MessageBoxA(0, filename, "ファイルがないっす", 0);
		exit(0);
	}
	if (w)*w = texWidth;
	if (h)*h = texHeight;

	createTextureBuffer(pixels, texWidth, texHeight, textureBuffer);

	//開放
	stbi_image_free(pixels);

	return S_OK;
}
//ディスクリプタ系
void createVertexBufferView(ComPtr<ID3D12Resource>& vertexBuffer,UINT sizeInBytes, UINT strideInBytes, D3D12_VERTEX_BUFFER_VIEW& vertexBufferView)
{
	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = sizeInBytes;
	vertexBufferView.StrideInBytes = strideInBytes;
}
void createIndexBufferView(ComPtr<ID3D12Resource>& indexBuffer, UINT sizeInBytes, D3D12_INDEX_BUFFER_VIEW& indexBufferView)
{
	indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = sizeInBytes;
	indexBufferView.Format = DXGI_FORMAT_R16_UINT;
}
UINT createConstantBufferView(ComPtr<ID3D12Resource>& constantBuffer)
{
	assert(CurrentCbvTbvIdx < MaxCbvTbvIdxs);
	D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
	desc.BufferLocation = constantBuffer->GetGPUVirtualAddress();
	desc.SizeInBytes = static_cast<UINT>(constantBuffer->GetDesc().Width);
	auto hCbvTbvHeap = CbvTbvHeap->GetCPUDescriptorHandleForHeapStart();
	hCbvTbvHeap.ptr += CbvTbvIncSize * CurrentCbvTbvIdx;
	Device->CreateConstantBufferView(&desc, hCbvTbvHeap);
	return CurrentCbvTbvIdx++;
}
UINT createTextureBufferView(ComPtr<ID3D12Resource>& textureBuffer)
{
	assert(CurrentCbvTbvIdx < MaxCbvTbvIdxs);
	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format = textureBuffer->GetDesc().Format;
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipLevels = 1;//ミップマップは使用しないので1
	auto hCbvTbvHeap = CbvTbvHeap->GetCPUDescriptorHandleForHeapStart();
	hCbvTbvHeap.ptr += CbvTbvIncSize * CurrentCbvTbvIdx;
	Device->CreateShaderResourceView(textureBuffer.Get(), &desc, hCbvTbvHeap);
	return CurrentCbvTbvIdx++;
}
//描画系
void clearColor(float r, float g, float b)
{
	ClearColor[0] = r; ClearColor[1] = g; ClearColor[2] = b;
}
void backgroundRect()
{
	rectModeCorner();
	rect(0, 0, width, height);
}
void beginRender()
{
	InitConstantIdxCnt();
	InitPrintPosY();

	//現在のバックバッファのインデックスを取得。このプログラムの場合0 or 1になる。
	BackBufIdx = SwapChain->GetCurrentBackBufferIndex();

	//バリアでバックバッファを描画ターゲットに切り替える
	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;//このバリアは状態遷移タイプ
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = BackBuffers[BackBufIdx].Get();//リソースはバックバッファ
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;//遷移前はPresent
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;//遷移後は描画ターゲット
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	CommandList->ResourceBarrier(1, &barrier);

	//バックバッファの場所を指すディスクリプタヒープハンドルを用意する
	auto hBbvHeap = BbvHeap->GetCPUDescriptorHandleForHeapStart();
	hBbvHeap.ptr += BackBufIdx * BbvIncSize;
	//デプスステンシルバッファのディスクリプタハンドルを用意する
	auto hDsvHeap = DsvHeap->GetCPUDescriptorHandleForHeapStart();
	//バックバッファとデプスステンシルバッファを描画ターゲットとして設定する
	CommandList->OMSetRenderTargets(1, &hBbvHeap, false, &hDsvHeap);
	//バックバッファをクリアする
	CommandList->ClearRenderTargetView(hBbvHeap, ClearColor, 0, nullptr);
	//デプスステンシルバッファをクリアする
	CommandList->ClearDepthStencilView(hDsvHeap, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	//ビューポートとシザー矩形をセット
	CommandList->RSSetViewports(1, &Viewport);
	CommandList->RSSetScissorRects(1, &ScissorRect);

	//ディスクリプタヒープをＧＰＵにセット
	CommandList->SetDescriptorHeaps(1, CbvTbvHeap.GetAddressOf());
	//パイプラインステートをセット
	CommandList->SetPipelineState(PipelineState.Get());
	//ルートシグニチャをセット
	CommandList->SetGraphicsRootSignature(RootSignature.Get());
}
void endRender()
{
	//バリアでバックバッファを表示用に切り替える
	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;//このバリアは状態遷移タイプ
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = BackBuffers[BackBufIdx].Get();//リソースはバックバッファ
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;//遷移前は描画ターゲット
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;//遷移後はPresent
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	CommandList->ResourceBarrier(1, &barrier);

	//コマンドリストをクローズする
	CommandList->Close();
	//コマンドリストを実行する
	ID3D12CommandList* commandLists[] = { CommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
	//描画完了を待つ
	waitGPU();

	//バックバッファを表示
	SwapChain->Present(1, 0);

	//コマンドアロケータをリセット
	Hr = CommandAllocator->Reset();
	assert(SUCCEEDED(Hr));
	//コマンドリストをリセット
	Hr = CommandList->Reset(CommandAllocator.Get(), nullptr);
	assert(SUCCEEDED(Hr));
}
void beginMsaaRender()
{
	//===
	InitConstantIdxCnt();
	InitPrintPosY();

	//MSAAレンダーバッファをターゲット状態に遷移
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		MsaaRenderBuffer.Get(),
		D3D12_RESOURCE_STATE_RESOLVE_SOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	CommandList->ResourceBarrier(1, &barrier);

	//MSAAレンダーバッファとMSAAデプスステンシルバッファをレンダーターゲットにセット
	auto hMsaaRtvHeap = MsaaRtvHeap->GetCPUDescriptorHandleForHeapStart();
	auto hMsaaDsvHeap = MsaaDsvHeap->GetCPUDescriptorHandleForHeapStart();
	CommandList->OMSetRenderTargets(1, &hMsaaRtvHeap, FALSE, &hMsaaDsvHeap);
	//バッファクリア
	CommandList->ClearRenderTargetView(hMsaaRtvHeap, ClearColor, 0, nullptr);
	CommandList->ClearDepthStencilView(hMsaaDsvHeap, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	//ビューポートとシザー矩形をセット
	CommandList->RSSetViewports(1, &Viewport);
	CommandList->RSSetScissorRects(1, &ScissorRect);

	//ディスクリプタヒープをＧＰＵにセット
	CommandList->SetDescriptorHeaps(1, CbvTbvHeap.GetAddressOf());
	//パイプラインステートをセット
	CommandList->SetPipelineState(PipelineState.Get());
	//ルートシグニチャをセット
	CommandList->SetGraphicsRootSignature(RootSignature.Get());
}
void endMsaaRender()
{
	//現在のバックバッファのインデックスを取得。このプログラムの場合0 or 1になる。
	BackBufIdx = SwapChain->GetCurrentBackBufferIndex();

	//MSAAレンダーバッファをバックバッファにコピーするための状態遷移
	D3D12_RESOURCE_BARRIER barriers[2] =
	{
		CD3DX12_RESOURCE_BARRIER::Transition(
			MsaaRenderBuffer.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_RESOLVE_SOURCE),
		CD3DX12_RESOURCE_BARRIER::Transition(
			BackBuffers[BackBufIdx].Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RESOLVE_DEST)
	};
	CommandList->ResourceBarrier(2, barriers);

	//MSAAレンダーバッファをバックバッファにコピー
	CommandList->ResolveSubresource(
		BackBuffers[BackBufIdx].Get(), 0,
		MsaaRenderBuffer.Get(), 0,
		BACK_BUFFER_FORMAT);

	//バックバッファをプレゼント状態に遷移
	D3D12_RESOURCE_BARRIER barrier =
	{
		CD3DX12_RESOURCE_BARRIER::Transition(
			BackBuffers[BackBufIdx].Get(),
			D3D12_RESOURCE_STATE_RESOLVE_DEST,
			D3D12_RESOURCE_STATE_PRESENT)
	};
	CommandList->ResourceBarrier(1, &barrier);

	//コマンドリストをクローズする
	CommandList->Close();
	//コマンドリストを実行する
	ID3D12CommandList* commandLists[] = { CommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	//バックバッファを表示
	SwapChain->Present(0, 0);

	//描画完了を待つ
	waitGPU();

	//コマンドアロケータをリセット
	Hr = CommandAllocator->Reset();
	assert(SUCCEEDED(Hr));
	//コマンドリストをリセット
	Hr = CommandList->Reset(CommandAllocator.Get(), nullptr);
	assert(SUCCEEDED(Hr));
}

//２D----------------------------------------------------------------------------
//正方形頂点バッファ。使いまわしする。
ComPtr<ID3D12Resource>   SquareVertexBuffer = nullptr;
D3D12_VERTEX_BUFFER_VIEW SquareVbv;
void CreateSquareVertexBuffer()
{
	//共有する頂点バッファ
	{
		unsigned numVertexElements = 5;//１頂点の要素数
		float vertices[] = {
			//position            texcoord
			-0.5f,  0.5f,  0.0f,  0.0f,  0.0f, //左上
			-0.5f, -0.5f,  0.0f,  0.0f,  1.0f, //左下
			 0.5f,  0.5f,  0.0f,  1.0f,  0.0f, //右上
			 0.5f, -0.5f,  0.0f,  1.0f,  1.0f, //右下
		};
		//データサイズを求めておく
		UINT sizeInBytes = sizeof(vertices);
		UINT strideInBytes = sizeof(float) * numVertexElements;
		//バッファをつくる
		Hr = createBuffer(sizeInBytes, SquareVertexBuffer);
		assert(SUCCEEDED(Hr));
		//バッファにデータを入れる
		Hr = updateBuffer(vertices, sizeInBytes, SquareVertexBuffer);
		assert(SUCCEEDED(Hr));
		//ビューをつくる
		createVertexBufferView(SquareVertexBuffer, sizeInBytes, strideInBytes, SquareVbv);
	}
}

//円の頂点バッファ（複数の大きさ）
const int NumCircles = 5;
int NumAngles[NumCircles] = { 8,16,32,64,128 };
ComPtr<ID3D12Resource>   CircleVertexBuffer[NumCircles];
D3D12_VERTEX_BUFFER_VIEW CircleVbv[NumCircles];
void CreateCircleVertexBuffers()
{
	for(int i=0; i<NumCircles; ++i){
		int num = NumAngles[i];
		const int numVertexElements = 5;//１頂点の要素数
		float* v = new float[num * numVertexElements]{};//オールゼロクリア
		float rad = 3.1415926f * 2 / num;
		int j = 0;
		int k = 0;
		v[k] = 0.5f; v[k + 1] = 0.0f;
		for (j = 1; j < num / 2; j++) {
			k += numVertexElements;
			v[k] = cosf(rad * j) * 0.5f, v[k + 1] = sinf(rad * j) * 0.5f;
			k += numVertexElements;
			v[k] = cosf(rad * j) * 0.5f, v[k + 1] = -sinf(rad * j) * 0.5f;
		}
		k += numVertexElements;
		v[k] = -0.5f; v[k + 1] = 0.0f;

		//データサイズを求めておく
		UINT sizeInBytes = sizeof(float) * num * numVertexElements;
		UINT strideInBytes = sizeof(float) * numVertexElements;
		//バッファをつくる
		Hr = createBuffer(sizeInBytes, CircleVertexBuffer[i]);
		assert(SUCCEEDED(Hr));
		//バッファにデータを入れる
		Hr = updateBuffer(v, sizeInBytes, CircleVertexBuffer[i]);
		assert(SUCCEEDED(Hr));
		//ビューをつくる
		createVertexBufferView(CircleVertexBuffer[i], 
			sizeInBytes, strideInBytes, CircleVbv[i]);

		delete[] v;
	}
}

//マップ用コンスタントバッファ構造体
struct CONST_BUF0 {
	XMMATRIX worldViewProj;
	XMFLOAT4 diffuse;
};
//コンスタント構造体
struct CONSTANT {
	ComPtr<ID3D12Resource> constBuffer0 = nullptr;
	CONST_BUF0* cb0 = nullptr;
	UINT cbvIdx = 0;
};
//コンスタント配列
std::vector<CONSTANT>Constants;
//#####debug#####
size_t numConstants() { return Constants.size(); }
//コンスタント配列を指すインデックス
UINT ConstantIdxCnt = 0;
void InitConstantIdxCnt()//beginRender()内で呼び出す
{
	ConstantIdxCnt = 0;
}
//コンスタントが足りなかったらつくる
void AutoCreateConstant()
{
	if (ConstantIdxCnt == Constants.size()) {
		CONSTANT tmp;
		createBuffer(alignedSize(sizeof(CONST_BUF0)), tmp.constBuffer0);
		mapBuffer(tmp.constBuffer0, (void**)&tmp.cb0);
		tmp.cbvIdx = createConstantBufferView(tmp.constBuffer0);
		Constants.emplace_back(tmp);
	}
}

//テクスチャ構造体
struct TEXTURE {
	ComPtr<ID3D12Resource> textureBuffer;
	UINT tbvIdx=-1;
	float texWidth=0;
	float texHeight=0;
};
//テクスチャ配列
std::vector<TEXTURE>Textures;
//テクスチャ重複チェック
std::unordered_map<std::string, int> DuplicateCheckMap;
//#####debug#####
size_t numLoadTextures() { return Textures.size(); }
//テクスチャを読み込む
int loadImage(const char* filename)
{
	auto itr = DuplicateCheckMap.find(filename);
	if (itr == DuplicateCheckMap.end()) {
		TEXTURE tmp;
		int w, h;
		createTextureBufferFromFile(filename, tmp.textureBuffer, &w, &h);
		tmp.tbvIdx = createTextureBufferView(tmp.textureBuffer);
		tmp.texWidth = (float)w;
		tmp.texHeight = (float)h;
		Textures.emplace_back(tmp);
		int idx = (int)Textures.size() - 1;
		DuplicateCheckMap[filename] = idx;
		return idx;
	}
	else {
		return itr->second;
	}
}
//テクスチャを張り付けた四角形の描画
void drawImage(UINT cbvIdx, UINT tbvIdx)
{
	//頂点をセット
	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	CommandList->IASetVertexBuffers(0, 1, &SquareVbv);
	//コンスタントをセット
	auto hCbvTbvHeap = CbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
	hCbvTbvHeap.ptr += CbvTbvIncSize * cbvIdx;
	CommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);
	//テクスチャをセット
	hCbvTbvHeap = CbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
	hCbvTbvHeap.ptr += CbvTbvIncSize * tbvIdx;
	CommandList->SetGraphicsRootDescriptorTable(1, hCbvTbvHeap);
	//描画
	CommandList->DrawInstanced(4, 1, 0, 0);

	//drawしたら必ずカウントアップ
	ConstantIdxCnt++;
}


float IMGR = 1, IMGG = 1, IMGB = 1, IMGA = 1;
void imageColor(float r, float g, float b, float a)
{
	IMGR = r; IMGG = g; IMGB = b; IMGA = a;
}
//矩形描画モード
constexpr int CENTER = 0;
constexpr int CORNER = 1;
int RectMode = 0;
void rectModeCorner()
{
	RectMode = CORNER;
}
void rectModeCenter()
{
	RectMode = CENTER;
}

XMMATRIX OrthoProj;
void CreateOrthoProj()
{
	OrthoProj = 
		XMMatrixScaling(2.0f / baseWidth(), 2.0f / baseHeight(), 1)
		* XMMatrixTranslation(-1.0f, 1.0f, 0);
}

//画像描画
void image(int textureIdx, float px, float py, float rad, float sx, float sy)
{
	//コンスタントが足りなかったらつくる
	AutoCreateConstant();

	auto& tex = Textures[textureIdx];
	auto& con = Constants[ConstantIdxCnt];

	//マトリックス⇒コンスタントにセット
	XMMATRIX world =
		XMMatrixScaling(tex.texWidth*sx, tex.texHeight*sy, 1)
		* XMMatrixRotationZ(rad);
	if (RectMode == CORNER) {
		world *= XMMatrixTranslation(px + tex.texWidth / 2, -(py + tex.texHeight / 2), 0);
	}
	else {
		world *= XMMatrixTranslation(px, -py, 0);
	}
	con.cb0->worldViewProj = world * OrthoProj;
	//ディフューズカラー⇒コンスタントにセット
	con.cb0->diffuse = { IMGR,IMGG,IMGB,IMGA };

	//描画
	drawImage(con.cbvIdx, tex.tbvIdx);
}

//初学者用ファイル名直接指定バージョン
void image(const char* filename, float px, float py, float rad, float sx, float sy)
{
	int idx = loadImage(filename);
	image(idx, px, py, rad, sx, sy);
}

//diffuse色のみのポリゴンに貼り付ける白テクスチャ
ComPtr<ID3D12Resource> WhiteTexture;
int WhiteTbvIdx;
void CreateWhiteTexture()//createDescriptorHeapから呼び出される
{
	BYTE pixels[] = { 
		0xff,0xff,0xff,0xff,
		0xff,0xff,0xff,0xff,
		0xff,0xff,0xff,0xff,
		0xff,0xff,0xff,0xff,
	};
	UINT w = 2;
	UINT h = 2;
	createTextureBuffer(pixels, w, h, WhiteTexture);
	WhiteTbvIdx = createTextureBufferView(WhiteTexture);
}

//塗りつぶす色
float FillR = 1, FillG = 1, FillB = 1, FillA = 1;
void fill(float r, float g, float b, float a)
{
	FillR = r; FillG = g; FillB = b; FillA = a;
}
void noFill()
{
	//塗りつぶしなし
	FillR = 0; FillG = 0; FillB = 0; FillA = 0;
}
//輪郭線の色
float StrokeR = 0, StrokeG = 0, StrokeB = 0, StrokeA = 1;
void stroke(float r, float g, float b, float a)
{
	StrokeR = r; StrokeG = g; StrokeB = b; StrokeA = a;
}
//輪郭線の太さ
float StrokeWeight = 1;
void strokeWeight(float sw)
{
	StrokeWeight = sw;
}
void noStroke()
{
	//輪郭線なし
	StrokeWeight = 0;
}

//点
void point(float px, float py)
{
	//コンスタントが足りなかったらつくる
	AutoCreateConstant();

	//マトリックス⇒コンスタントにセット
	XMMATRIX world =
		XMMatrixScaling(StrokeWeight, StrokeWeight, 1)
		* XMMatrixTranslation(px, -py, 0)
		* OrthoProj;
	auto& con = Constants[ConstantIdxCnt];
	con.cb0->worldViewProj = world;
	//ディフューズカラー⇒コンスタントにセット
	con.cb0->diffuse = { StrokeR,StrokeG,StrokeB,StrokeA };

	//大きさによる頂点バッファビューの選択
	int idx=0;
	if      (StrokeWeight <=  10)idx = 0;
	else if (StrokeWeight <=  50)idx = 1;
	else if (StrokeWeight <= 200)idx = 2;
	else if (StrokeWeight <= 800)idx = 3;
	else idx = 4;
	auto& Vbv = CircleVbv[idx];

	//頂点をセット
	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	CommandList->IASetVertexBuffers(0, 1, &Vbv);
	//コンスタントをセット
	auto hCbvTbvHeap = CbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
	hCbvTbvHeap.ptr += CbvTbvIncSize * con.cbvIdx;
	CommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);
	//テクスチャをセット
	hCbvTbvHeap = CbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
	hCbvTbvHeap.ptr += CbvTbvIncSize * WhiteTbvIdx;
	CommandList->SetGraphicsRootDescriptorTable(1, hCbvTbvHeap);
	//描画
	UINT numVertices = Vbv.SizeInBytes / Vbv.StrideInBytes;
	CommandList->DrawInstanced(numVertices, 1, 0, 0);
	ConstantIdxCnt++;
}
//終点に点を描くか否か
bool DrawEndPointFlag = true;
//線
void line(float sx, float sy, float ex, float ey)
{
	//コンスタントが足りなかったらつくる
	AutoCreateConstant();

	//マトリックス⇒コンスタントにセット
	float dx = ex - sx;
	float dy = ey - sy;
	float length = sqrtf(dx * dx + dy * dy);
	float rad = -atan2f(dy, dx);
	XMMATRIX world =
		XMMatrixTranslation(0.5f, 0, 0)
		* XMMatrixScaling(length, StrokeWeight, 1)
		* XMMatrixRotationZ(rad)
		* XMMatrixTranslation(sx, -sy, 0)
		* OrthoProj;
	auto& con = Constants[ConstantIdxCnt];
	con.cb0->worldViewProj = world;
	//ディフューズカラー⇒コンスタントにセット
	con.cb0->diffuse = { StrokeR,StrokeG,StrokeB,StrokeA };

	//描画
	drawImage(con.cbvIdx, WhiteTbvIdx);
	if (StrokeWeight > 1) {
		//始点
		point(sx, sy);
		//終点
		if (DrawEndPointFlag)point(ex, ey);
	}
}
void arrow(float sx, float sy, float ex, float ey, float arrowLen, float arrowDeg)
{
	//コンスタントが足りなかったらつくる
	AutoCreateConstant();
	//マトリックス⇒コンスタントにセット
	float dx = ex - sx;
	float dy = ey - sy;
	float length = sqrtf(dx * dx + dy * dy);
	float rad = -atan2f(dy, dx);
	XMMATRIX world =
		XMMatrixTranslation(0.5f, 0, 0)
		* XMMatrixScaling(length, StrokeWeight, 1)
		* XMMatrixRotationZ(rad)
		* XMMatrixTranslation(sx, -sy, 0)
		* OrthoProj;
	auto& con = Constants[ConstantIdxCnt];
	con.cb0->worldViewProj = world;
	//ディフューズカラー⇒コンスタントにセット
	con.cb0->diffuse = { StrokeR,StrokeG,StrokeB,StrokeA };
	//描画
	drawImage(con.cbvIdx, WhiteTbvIdx);
	if (StrokeWeight > 1) {
		//始点
		point(sx, sy);
		//終点
		point(ex, ey);
	}

	XMVECTOR v = XMVectorSet(0.5f, 0.0f, 0.0f, 1.0f);
	float arrowRad = 3.1415926f / 180 * (180-arrowDeg);
	{
		AutoCreateConstant();
		XMMATRIX world =
			XMMatrixTranslation(0.5f, 0, 0)
			* XMMatrixScaling(arrowLen, StrokeWeight, 1)
			* XMMatrixRotationZ(rad + arrowRad)
			* XMMatrixTranslation(ex, -ey, 0);
		auto& con = Constants[ConstantIdxCnt];
		con.cb0->worldViewProj = world * OrthoProj;
		//ディフューズカラー⇒コンスタントにセット
		con.cb0->diffuse = { StrokeR,StrokeG,StrokeB,StrokeA };
		//描画
		drawImage(con.cbvIdx, WhiteTbvIdx);
		XMVECTOR v_ = XMVector4Transform(v, world);
		point(XMVectorGetX(v_), -XMVectorGetY(v_));
	}
	{
		AutoCreateConstant();
		XMMATRIX world =
			XMMatrixTranslation(0.5f, 0, 0)
			* XMMatrixScaling(arrowLen, StrokeWeight, 1)
			* XMMatrixRotationZ(rad - arrowRad)
			* XMMatrixTranslation(ex, -ey, 0);
		auto& con = Constants[ConstantIdxCnt];
		con.cb0->worldViewProj = world * OrthoProj;
		//ディフューズカラー⇒コンスタントにセット
		con.cb0->diffuse = { StrokeR,StrokeG,StrokeB,StrokeA };
		//描画
		drawImage(con.cbvIdx, WhiteTbvIdx);
		XMVECTOR v_ = XMVector4Transform(v, world);
		point(XMVectorGetX(v_), -XMVectorGetY(v_));
	}

}
void arc(float ox, float oy, float ax, float ay, float bx, float by, float radius)
{
	ax -= ox;
	ay -= oy;
	float al = sqrtf(ax * ax + ay * ay);
	ax /= al;
	ay /= al;
	
	bx -= ox;
	by -= oy;
	float bl = sqrtf(bx * bx + by * by);
	bx /= bl;
	by /= bl;
	
	float rad = acosf(ax * bx + ay * by);
	float cross = ax * by - ay * bx;
	for (float r = 0; r < rad; r += 3.141592f/180) {
		float cr = cosf(r);
		float sr = sinf(r);
		if (cross > 0)sr *= -1;
		float x = ax * cr + ay * sr;
		sr *= -1;
		float y = ax * sr + ay * cr;
		point(ox + x * radius, oy + y * radius);
	}
}
//矩形
void rect(float px, float py, float w, float h, float rad)
{
	//コンスタントが足りなかったらつくる
	AutoCreateConstant();
	//2D用マトリックス⇒コンスタントにセット
	XMMATRIX world=
		XMMatrixScaling(w, h, 1)
		* XMMatrixRotationZ(rad);
	if (RectMode == CORNER) {
		world *= XMMatrixTranslation(px + w / 2, -(py + h / 2), 0);
	}
	else {
		world *= XMMatrixTranslation(px, -py, 0);
	}
	auto& con = Constants[ConstantIdxCnt];
	con.cb0->worldViewProj = world * OrthoProj;
	//ディフューズカラー⇒コンスタントにセット
	con.cb0->diffuse = { FillR,FillG,FillB,FillA };

	//描画
	if (FillA > 0.0f) {
		drawImage(con.cbvIdx, WhiteTbvIdx);
	}

	//輪郭
	if (StrokeWeight > 0) {
		XMVECTOR v0= XMVectorSet( -0.5f, 0.5f, 0.0f,1.0f );
		XMVECTOR v1= XMVectorSet( -0.5f,-0.5f, 0.0f,1.0f );
		XMVECTOR v2= XMVectorSet( 0.5f, 0.5f, 0.0f,1.0f );
		XMVECTOR v3= XMVectorSet( 0.5f,-0.5f, 0.0f,1.0f );
		XMVECTOR lt = XMVector4Transform(v0, world); 
		XMVECTOR lb = XMVector4Transform(v1, world);
		XMVECTOR rt = XMVector4Transform(v2, world);
		XMVECTOR rb = XMVector4Transform(v3, world);
		DrawEndPointFlag = false;
		line(XMVectorGetX(lt), -XMVectorGetY(lt), XMVectorGetX(lb), -XMVectorGetY(lb));
		line(XMVectorGetX(lb), -XMVectorGetY(lb), XMVectorGetX(rb), -XMVectorGetY(rb));
		line(XMVectorGetX(rb), -XMVectorGetY(rb), XMVectorGetX(rt), -XMVectorGetY(rt));
		line(XMVectorGetX(rt), -XMVectorGetY(rt), XMVectorGetX(lt), -XMVectorGetY(lt));
		DrawEndPointFlag = true;
	}
}
//円
void circle(float px, float py, float diameter)
{
	//コンスタントが足りなかったらつくる
	AutoCreateConstant();
	//マトリックス⇒コンスタントにセット
	XMMATRIX world =
		XMMatrixScaling(diameter, diameter, 1)
		* XMMatrixTranslation(px, -py, 0)
		* OrthoProj;
	auto& con = Constants[ConstantIdxCnt];
	con.cb0->worldViewProj = world;
	//ディフューズカラー⇒コンスタントにセット
	con.cb0->diffuse = { FillR,FillG,FillB,FillA };

	int idx = 0;
	if		(diameter <=  10) { idx = 0; }
	else if (diameter <=  50) { idx = 1; }
	else if (diameter <= 200) { idx = 2; }
	else if (diameter <= 800) { idx = 3; }
	else					  { idx = 4; }
	auto& Vbv = CircleVbv[idx];

	//頂点をセット
	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	CommandList->IASetVertexBuffers(0, 1, &Vbv);
	//コンスタントをセット
	auto hCbvTbvHeap = CbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
	hCbvTbvHeap.ptr += CbvTbvIncSize * con.cbvIdx;
	CommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);
	//テクスチャをセット
	hCbvTbvHeap = CbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
	hCbvTbvHeap.ptr += CbvTbvIncSize * WhiteTbvIdx;
	CommandList->SetGraphicsRootDescriptorTable(1, hCbvTbvHeap);
	//描画
	UINT numVertices = Vbv.SizeInBytes / Vbv.StrideInBytes;
	CommandList->DrawInstanced(numVertices, 1, 0, 0);
	ConstantIdxCnt++;

	//輪郭
	if (StrokeWeight > 0) {
		int numAngles = NumAngles[idx];
		float rad = 3.141592f * 2 / numAngles;
		float radius = diameter / 2;
		DrawEndPointFlag = false;
		float sx = px + radius;
		float sy = py;
		float ex, ey;
		for (int i = 0; i < numAngles; i++) {
			ex = px + cosf(rad * (i + 1)) * radius;
			ey = py + sinf(rad * (i + 1)) * radius;
			line(sx, sy, ex, ey);
			sx = ex;
			sy = ey;
		}
		DrawEndPointFlag = true;
	}
}

//フォント
//
//色
float FONTR = 0, FONTG = 0, FONTB = 0, FONTA = 1;
void fontColor(float r, float g, float b, float a)
{
	FONTR = r; FONTG = g; FONTB = b; FONTA = a;
}
//現在描画中のフォントフェイス構造体
struct CURRENT_FONT_FACE {
	std::string name; unsigned long charset; int idx; int size;
};
CURRENT_FONT_FACE CurFontFace{ "ＭＳ ゴシック",SHIFTJIS_CHARSET,0,50 };
//FontFace名ごとにｉｄを付けて管理するマップ
std::unordered_map<std::string, int> FontFaceIdxMap{ {CurFontFace.name, 0} };
int FontFaceIdxCnt = 0;
//描画するフォントフェイスを設定する
void fontFace(const char* fontname, unsigned charset)
{
	//フォント名とcharset
	CurFontFace.name = fontname;
	CurFontFace.charset = charset;

	//CurFontFace.idをセットする
	auto itr = FontFaceIdxMap.find(fontname);
	if (itr == FontFaceIdxMap.end()) {
		FontFaceIdxCnt++;
		assert(FontFaceIdxCnt < 32);//FontFaceこれ以上追加できません;
		FontFaceIdxMap[fontname] = FontFaceIdxCnt;
		CurFontFace.idx = FontFaceIdxCnt;
	}
	else {
		CurFontFace.idx = itr->second;
	}
}
//フォントサイズを設定する
void fontSize(int size)
{
	assert(size <= 2048);//"FontSize","2048より大きいサイズは指定できません";
	CurFontFace.size = size;
}

//フォント用テクスチャ構造体(下のマップに保存していくフォントの描画に必要なデータ達)
struct FONT_TEXTURE {
	ComPtr<ID3D12Resource> textureBuffer = nullptr;
	UINT tbvIdx = 0;
	float texWidth=0, texHeight=0;//テクスチャの幅、高さ
	float drawWidth=0, drawHeight=0;//描画幅、高さ
	float ofstX=0, ofstY=0;//描画するときにずらす値
};
//フォントテクスチャデータを管理するマップ
static std::unordered_map<DWORD, FONT_TEXTURE> FontTextureMap;
size_t numFontTextures() { return FontTextureMap.size(); }
//１文字分のフォントテクスチャをつくって上のマップに追加する
FONT_TEXTURE* CreateFontTexture(DWORD key)
{
	//フォント（サイズやフォントの種類）を決める！
	HFONT hFont = CreateFontA(
		CurFontFace.size, 0, 0, 0, 0, 0, 0, 0,
		CurFontFace.charset,
		OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY, FIXED_PITCH | FF_MODERN,
		CurFontFace.name.c_str()
	);
	assert(hFont); //"Font", "Create error"

	//デバイスコンテキスト取得
	HDC hdc = GetDC(NULL);
	//デバイスコンテキストにフォントを設定
	HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

	//フォントの各種寸法とアルファビットマップを取得
	TEXTMETRICA tm;
	GetTextMetricsA(hdc, &tm);
	GLYPHMETRICS gm;
	CONST MAT2 mat = { {0,1},{0,0},{0,0},{0,1} };
	UINT code = key & 0xffff;//keyから文字コードを取り出す
	DWORD alphaBmpSize = GetGlyphOutlineA(hdc, code, GGO_GRAY4_BITMAP, &gm, 0, NULL, &mat);
	BYTE* alphaBmpBuf = new BYTE[alphaBmpSize];
	GetGlyphOutlineA(hdc, code, GGO_GRAY4_BITMAP, &gm, alphaBmpSize, alphaBmpBuf, &mat);
	//α値の階調 (GGO_GRAY4_BITMAPは17階調。alphaBmpBuf[i]は０～１６の値となる)
	BYTE tone = 16;//最大値

	//デバイスコンテキストとフォントハンドルの開放
	DeleteObject(hFont);
	SelectObject(hdc, oldFont);
	ReleaseDC(NULL, hdc);

	//画像の幅と高さ
	UINT texWidth = (gm.gmBlackBoxX + 3) & 0xfffc;//4の倍数にする
	UINT texHeight = gm.gmBlackBoxY;

	//alphaBmpBufを元にフォント画像データをつくる
	BYTE* pixels = new BYTE[texWidth * texHeight * 4];
	UINT x, y, i;
	for (y = 0; y < texHeight; ++y) {
		for (x = 0; x < texWidth; x++) {
			i = y * texWidth + x;
			pixels[i*4 + 0] = 0xff;//r
			pixels[i*4 + 1] = 0xff;//g
			pixels[i*4 + 2] = 0xff;//b
			pixels[i*4 + 3] = alphaBmpBuf[i] * 255 / tone;//0～16を0～255に変換
		}
	}

	//FONT_TEXTURE(描画に必要なデータ達)をマップに登録
	FONT_TEXTURE& font = FontTextureMap[key];
	createTextureBuffer(pixels, texWidth, texHeight, font.textureBuffer);
	if(code==0x20||code== 0x8140)texHeight-=1;//スペースのunder lineを消す
	font.tbvIdx = createTextureBufferView(font.textureBuffer);
	font.texWidth = (float)texWidth;//テクスチャの幅
	font.texHeight = (float)texHeight;//テクスチャの高さ
	font.ofstX = (float)gm.gmptGlyphOrigin.x;
	font.ofstY = (float)tm.tmAscent - gm.gmptGlyphOrigin.y;//描画する時にずらす値
	font.drawWidth = (float)gm.gmCellIncX;//描画する幅
	font.drawHeight = (float)tm.tmHeight;//描画する高さ

	delete[] alphaBmpBuf;
	delete[] pixels;

	return &font;
}

int FontRectMode = CORNER;
void fontRectModeCorner()
{
	FontRectMode = CORNER;
}
void fontRectModeCenter()
{
	FontRectMode = CENTER;
}

//指定した文字列を指定したスクリーン座標で描画する
float text(const char* str, float x, float y)
{
	int len = (int)strlen(str);

	//ループしながら１文字ずつ描画していく
	for (int i = 0; i < len; i++) {

		//文字コードの決定(マルチバイトコードしか扱わない前提)
		WORD code;
		if (IsDBCSLeadByte(str[i])) {
			//2バイト文字のコードは[先導コード] + [文字コード]です
			code = (BYTE)str[i] << 8 | (BYTE)str[i + 1];
			i++;
		}
		else {
			//1バイト文字のコード
			code = str[i];
		}

		//マップ検索用key(フォントフェイスidx＋フォントサイズ＋文字コード)をつくる
		DWORD key = CurFontFace.idx << 27 | CurFontFace.size << 16 | code;

		//keyでマップ内にテクスチャがあるか探す
		FONT_TEXTURE* fontTex = 0;
		auto itr = FontTextureMap.find(key);
		if (itr == FontTextureMap.end()) {
			//なかったのでフォントのテクスチャをこの場でつくってアドレスをもらう
			fontTex = CreateFontTexture(key);
		}
		else {
			//あったのでアドレスを取得する
			fontTex = &itr->second;
		}

		//コンスタントが足りなかったらつくる
		AutoCreateConstant();
		
		XMMATRIX world;
		if (FontRectMode == CORNER) {
			world = XMMatrixTranslation(0.5f, -0.5f, 0)
			* XMMatrixScaling(fontTex->texWidth, fontTex->texHeight, 1)
			* XMMatrixTranslation(x + fontTex->ofstX, -(y + fontTex->ofstY), 0);
		}
		else{
			world = 
			XMMatrixScaling(fontTex->texWidth, fontTex->texHeight, 1)
			* XMMatrixTranslation(x, -y, 0);
		}
		auto& con = Constants[ConstantIdxCnt];
		con.cb0->worldViewProj = world * OrthoProj;
		con.cb0->diffuse = { FONTR,FONTG,FONTB,FONTA };
		
		//描画
		drawImage(con.cbvIdx, fontTex->tbvIdx);

		//次の文字の描画位置ｘを求めておく
		x += fontTex->drawWidth;
	}
	//横に続けて別の文字列を表示するための座標を返す
	return x;
}

float PrintInitX = 5;
float PrintInitY = 5;
float PrintY = PrintInitY;
void setPrintInitX(float initX)
{
	PrintInitX = initX;
}
void setPrintInitY(float initY)
{
	PrintInitY = initY;
}
void InitPrintPosY()
{
	PrintY = PrintInitY;
}
void print(const char* format, ...)
{
	char str[256];
	va_list args;
	va_start(args, format);
	vsprintf_s(str, format, args);
	va_end(args);

	float printX = PrintInitX;
	text(str, printX, PrintY);
	PrintY += CurFontFace.size;
}

USER_FONT::USER_FONT(const char* filename)
	:Filename(filename)
{
	AddFontResourceExA(filename, FR_PRIVATE, 0);
}
USER_FONT::~USER_FONT() {
	RemoveFontResourceExA(Filename.c_str(), FR_PRIVATE, 0);
}
