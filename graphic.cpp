#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3d12.lib")

#include<Windows.h>
#include<dxgi1_6.h>
#include<cassert>
#include<map>
#include<unordered_map>
#include<memory>

#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
#include"BIN_FILE12.h"
#include"graphic.h"

//グローバル変数-----------------------------------------------------------------
// ウィンドウ
LPCWSTR	WindowTitle;
int ClientWidth;
int ClientHeight;
int ClientPosX;
int ClientPosY;
float Aspect;
DWORD WindowStyle;
HWND HWnd;
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
float ClearColor[] = { 0,0,0,1 };
// デプスステンシルバッファ
ComPtr<ID3D12Resource> DepthStencilBuffer;
ComPtr<ID3D12DescriptorHeap> DsvHeap;//"Dsv"は"DepthStencilBufferView"の略
// パイプライン
ComPtr<ID3D12RootSignature> RootSignature;
ComPtr<ID3D12PipelineState> PipelineState;
D3D12_VIEWPORT Viewport;
D3D12_RECT ScissorRect;
//　コンスタントおよびテクスチャ用ディスクリプタヒープ
ComPtr<ID3D12DescriptorHeap> CbvTbvHeap;
UINT CbvTbvIncSize = 0;
UINT CurrentCbvTbvIdx = 0;

//プライベートな関数--------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hwnd, msg, wp, lp);
	}
}
void CreateWindows()
{
	//ウィンドウクラス登録
	WNDCLASSEX windowClass = {};
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WndProc;
	windowClass.hInstance = GetModuleHandle(0);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	windowClass.lpszClassName = L"GAME_WINDOW";
	RegisterClassEx(&windowClass);
	//表示位置、ウィンドウの大きさ調整
	RECT windowRect = { 0, 0, ClientWidth, ClientHeight };
	AdjustWindowRect(&windowRect, WindowStyle, FALSE);
	int windowPosX = ClientPosX + windowRect.left;
	int windowPosY = ClientPosY + windowRect.top;
	int windowWidth = windowRect.right - windowRect.left;
	int windowHeight = windowRect.bottom - windowRect.top;
	//ウィンドウをつくる
	HWnd = CreateWindowEx(
		NULL,
		L"GAME_WINDOW",
		WindowTitle,
		WindowStyle,
		windowPosX,
		windowPosY,
		windowWidth,
		windowHeight,
		NULL,		//親ウィンドウなし
		NULL,		//メニューなし
		GetModuleHandle(0),
		NULL);		//複数ウィンドウなし
}
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
		desc.Width = ClientWidth;
		desc.Height = ClientHeight;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.SampleDesc.Count = 1;
		ComPtr<IDXGISwapChain1> swapChain1;
		Hr = dxgiFactory->CreateSwapChainForHwnd(
			CommandQueue.Get(), HWnd, &desc, nullptr, nullptr, swapChain1.GetAddressOf());
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
		desc.Width = ClientWidth;//幅と高さはレンダーターゲットと同じ
		desc.Height = ClientHeight;//上に同じ
		desc.DepthOrArraySize = 1;//テクスチャ配列でもないし3Dテクスチャでもない
		desc.Format = DXGI_FORMAT_D32_FLOAT;//深度値書き込み用フォーマット
		desc.SampleDesc.Count = 1;//サンプルは1ピクセル当たり1つ
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//このバッファは深度ステンシルとして使用します
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.MipLevels = 1;
		//デプスステンシルバッファをクリアする値
		D3D12_CLEAR_VALUE depthClearValue = {};
		depthClearValue.DepthStencil.Depth = 1.0f;//深さ１(最大値)でクリア
		depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;//32bit深度値としてクリア
		//デプスステンシルバッファを作る
		Hr = Device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE, //デプス書き込みに使用
			&depthClearValue,
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
//===2D用
void CreatePipeline()
{
	//===
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
		samplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//横繰り返し
		samplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//縦繰り返し
		samplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//奥行繰り返し
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
	blendDesc.AlphaToCoverageEnable = true;
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
	pipelineDesc.SampleDesc.Count = 1;
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineDesc.NumRenderTargets = 1;
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	Hr = Device->CreateGraphicsPipelineState(
		&pipelineDesc,
		IID_PPV_ARGS(PipelineState.GetAddressOf())
	);
	assert(SUCCEEDED(Hr));

	//出力領域を設定
	Viewport.TopLeftX = 0.0f;
	Viewport.TopLeftY = 0.0f;
	Viewport.Width = (float)ClientWidth;
	Viewport.Height = (float)ClientHeight;
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;
	
	//切り取り矩形を設定
	ScissorRect.left = 0;
	ScissorRect.top = 0;
	ScissorRect.right = ClientWidth;
	ScissorRect.bottom = ClientHeight;
}

//===
void CreateSquareVertexBuffer();
void CreateCircleVertexBuffers();
void CreateWhiteTexture();
void CreateOrthoProj();

void InitConstantIdxCnt();
void InitPrintPosY();

//パブリックな関数---------------------------------------------------------------
//システム系
void window(LPCWSTR windowTitle, int clientWidth, int clientHeight, bool windowed, int numDescriptors, int clientPosX, int clientPosY)
{
	WindowTitle = windowTitle;
	ClientWidth = clientWidth;
	ClientHeight = clientHeight;
	ClientPosX = (GetSystemMetrics(SM_CXSCREEN) - ClientWidth) / 2;//中央表示
	if (clientPosX>=0)ClientPosX = clientPosX;
	ClientPosY = (GetSystemMetrics(SM_CYSCREEN) - ClientHeight) / 2;//中央表示
	if (clientPosY>=0)ClientPosY = clientPosY;
	Aspect = static_cast<float>(ClientWidth) / ClientHeight;
	WindowStyle = WS_POPUP;//Alt + F4で閉じる
	if (windowed) WindowStyle = WS_OVERLAPPEDWINDOW;

	CreateWindows();
	CreateDevice();
	CreateRenderTarget();
	CreatePipeline();

	//===
	CreateSquareVertexBuffer();
	CreateCircleVertexBuffers();
	CreateOrthoProj();
	createDescriptorHeap(numDescriptors);

	ShowWindow(HWnd, SW_SHOW);
}
bool quit()
{
	MSG msg = { 0 };
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if(msg.message == WM_QUIT)return true;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//===
	InitConstantIdxCnt();
	InitPrintPosY();

	return false;
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
//コンスタントバッファ、テクスチャバッファのディスクリプタヒープ
HRESULT createDescriptorHeap(UINT numDescriptors)
{
	CurrentCbvTbvIdx = 0;
	CbvTbvIncSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = numDescriptors;
	desc.NodeMask = 0;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	HRESULT hr =  Device->CreateDescriptorHeap(
		&desc, IID_PPV_ARGS(CbvTbvHeap.ReleaseAndGetAddressOf()));
	
	//ディフューズ色のみのポリゴンに貼り付けるテクスチャをつくる
	CreateWhiteTexture();

	return hr;
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
void beginRender()
{
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

	//パイプラインステートをセット
	CommandList->SetPipelineState(PipelineState.Get());
	//ルートシグニチャをセット
	CommandList->SetGraphicsRootSignature(RootSignature.Get());
	//ディスクリプタヒープをＧＰＵにセット
	CommandList->SetDescriptorHeaps(1, CbvTbvHeap.GetAddressOf());
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
//Get系
float clientWidth() { return (float)ClientWidth; }
float clientHeight() { return (float)ClientHeight; }

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
//コンスタント配列を指すインデックス
UINT ConstantIdxCnt = 0;
void InitConstantIdxCnt()//quit内で呼び出す
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
//#####debug#####
size_t numConstants() { return Constants.size(); }

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

//塗りつぶす色
float FillR = 1, FillG = 1, FillB = 1, FillA = 1;
void fill(float r, float g, float b, float a)
{
	FillR = r; FillG = g; FillB = b; FillA = a;
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
		XMMatrixScaling(2.0f / ClientWidth, 2.0f / ClientHeight, 1)
		* XMMatrixTranslation(-1.0f, 1.0f, 0);
}

//画像描画
void image(int textureIdx, float px, float py, float rad, float sx, float sy)
{
	//コンスタントが足りなかったらつくる
	AutoCreateConstant();
	//マトリックス⇒コンスタントにセット
	XMMATRIX world =
		XMMatrixScaling(Textures[textureIdx].texWidth*sx, Textures[textureIdx].texHeight*sy, 1)
		* XMMatrixRotationZ(rad);
	if (RectMode == CORNER) {
		world *= XMMatrixTranslation(px + Textures[textureIdx].texWidth / 2, -(py + Textures[textureIdx].texHeight / 2), 0);
	}
	else {
		world *= XMMatrixTranslation(px, -py, 0);
	}
	Constants[ConstantIdxCnt].cb0->worldViewProj = world * OrthoProj;
	//ディフューズカラー⇒コンスタントにセット
	Constants[ConstantIdxCnt].cb0->diffuse = { FillR,FillG,FillB,FillA };

	//描画
	drawImage(Constants[ConstantIdxCnt].cbvIdx, Textures[textureIdx].tbvIdx);
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

//輪郭線の色
float StrokeR = 1, StrokeG = 1, StrokeB = 1, StrokeA = 1;
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
	Constants[ConstantIdxCnt].cb0->worldViewProj = world;
	//ディフューズカラー⇒コンスタントにセット
	Constants[ConstantIdxCnt].cb0->diffuse = { StrokeR,StrokeG,StrokeB,StrokeA };

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
	hCbvTbvHeap.ptr += CbvTbvIncSize * Constants[ConstantIdxCnt].cbvIdx;
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
	Constants[ConstantIdxCnt].cb0->worldViewProj = world;
	//ディフューズカラー⇒コンスタントにセット
	Constants[ConstantIdxCnt].cb0->diffuse = { StrokeR,StrokeG,StrokeB,StrokeA };

	//描画
	drawImage(Constants[ConstantIdxCnt].cbvIdx, WhiteTbvIdx);
	if (StrokeWeight > 1) {
		//始点
		point(sx, sy);
		//終点
		if (DrawEndPointFlag)point(ex, ey);
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
	Constants[ConstantIdxCnt].cb0->worldViewProj = world * OrthoProj;
	//ディフューズカラー⇒コンスタントにセット
	Constants[ConstantIdxCnt].cb0->diffuse = { FillR,FillG,FillB,FillA };

	//描画
	if (FillA > 0.0f) {
		drawImage(Constants[ConstantIdxCnt].cbvIdx, WhiteTbvIdx);
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
	Constants[ConstantIdxCnt].cb0->worldViewProj = world;
	//ディフューズカラー⇒コンスタントにセット
	Constants[ConstantIdxCnt].cb0->diffuse = { FillR,FillG,FillB,FillA };

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
	hCbvTbvHeap.ptr += CbvTbvIncSize * Constants[ConstantIdxCnt].cbvIdx;
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
			if (i < alphaBmpSize) {
				pixels[i*4 + 0] = 0xff;//r
				pixels[i*4 + 1] = 0xff;//g
				pixels[i*4 + 2] = 0xff;//b
				pixels[i*4 + 3] = alphaBmpBuf[i] * 255 / tone;//0～16を0～255に変換
			}
		}
	}

	//FONT_TEXTURE(描画に必要なデータ達)をマップに登録
	createTextureBuffer(pixels, texWidth, texHeight, FontTextureMap[key].textureBuffer);
	FontTextureMap[key].tbvIdx = createTextureBufferView(FontTextureMap[key].textureBuffer);
	FontTextureMap[key].texWidth = (float)texWidth;
	FontTextureMap[key].texHeight = (float)texHeight;//テクスチャの幅と高さ
	FontTextureMap[key].drawWidth = (float)gm.gmCellIncX;
	FontTextureMap[key].drawHeight = (float)tm.tmHeight;//描画する幅と高さ
	FontTextureMap[key].ofstX = (float)gm.gmptGlyphOrigin.x;
	FontTextureMap[key].ofstY = (float)tm.tmAscent - gm.gmptGlyphOrigin.y;//描画する時にずらす値

	delete[] alphaBmpBuf;
	delete[] pixels;

	return &FontTextureMap[key];
}

int FontRectMode = CORNER;

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
			world = XMMatrixTranslation(0.5f, -0.5f, 0);//Px,Pyの位置に画像の左上がくる
		}
		else {
			world = XMMatrixIdentity();
		}
		world *=
			XMMatrixScaling(fontTex->texWidth, fontTex->texHeight, 1)
			* XMMatrixTranslation(x+fontTex->ofstX, -(y+fontTex->ofstY), 0)
			* OrthoProj;
		Constants[ConstantIdxCnt].cb0->worldViewProj = world;
		Constants[ConstantIdxCnt].cb0->diffuse = { FillR,FillG,FillB,FillA };
		
		//描画
		drawImage(Constants[ConstantIdxCnt].cbvIdx, fontTex->tbvIdx);

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
