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

//�O���[�o���ϐ�-----------------------------------------------------------------
// �E�B���h�E
LPCWSTR	WindowTitle;
int ClientWidth;
int ClientHeight;
int ClientPosX;
int ClientPosY;
float Aspect;
DWORD WindowStyle;
HWND HWnd;
// �f�o�C�X
ComPtr<ID3D12Device> Device;
// �R�}���h
ComPtr<ID3D12CommandAllocator> CommandAllocator;
ComPtr<ID3D12GraphicsCommandList> CommandList;
ComPtr<ID3D12CommandQueue> CommandQueue;
//===
// �R�}���h�Q�B�e�N�X�`�������鎞�Ɏg�p
ComPtr<ID3D12CommandAllocator> CommandAllocator2;
ComPtr<ID3D12GraphicsCommandList> CommandList2;
// �t�F���X
ComPtr<ID3D12Fence> Fence;
HANDLE FenceEvent;
UINT64 FenceValue;
// �f�o�b�O
HRESULT Hr;
// �o�b�N�o�b�t�@
ComPtr<IDXGISwapChain4> SwapChain;
ComPtr<ID3D12Resource> BackBuffers[2];
UINT BackBufIdx;
ComPtr<ID3D12DescriptorHeap> BbvHeap;//"Bbv"��"BackBufView"�̗�
UINT BbvIncSize;
float ClearColor[] = { 0,0,0,1 };
// �f�v�X�X�e���V���o�b�t�@
ComPtr<ID3D12Resource> DepthStencilBuffer;
ComPtr<ID3D12DescriptorHeap> DsvHeap;//"Dsv"��"DepthStencilBufferView"�̗�
// �p�C�v���C��
ComPtr<ID3D12RootSignature> RootSignature;
ComPtr<ID3D12PipelineState> PipelineState;
D3D12_VIEWPORT Viewport;
D3D12_RECT ScissorRect;
//�@�R���X�^���g����уe�N�X�`���p�f�B�X�N���v�^�q�[�v
ComPtr<ID3D12DescriptorHeap> CbvTbvHeap;
UINT CbvTbvIncSize = 0;
UINT CurrentCbvTbvIdx = 0;

//�v���C�x�[�g�Ȋ֐�--------------------------------------------------------------
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
	//�E�B���h�E�N���X�o�^
	WNDCLASSEX windowClass = {};
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WndProc;
	windowClass.hInstance = GetModuleHandle(0);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	windowClass.lpszClassName = L"GAME_WINDOW";
	RegisterClassEx(&windowClass);
	//�\���ʒu�A�E�B���h�E�̑傫������
	RECT windowRect = { 0, 0, ClientWidth, ClientHeight };
	AdjustWindowRect(&windowRect, WindowStyle, FALSE);
	int windowPosX = ClientPosX + windowRect.left;
	int windowPosY = ClientPosY + windowRect.top;
	int windowWidth = windowRect.right - windowRect.left;
	int windowHeight = windowRect.bottom - windowRect.top;
	//�E�B���h�E������
	HWnd = CreateWindowEx(
		NULL,
		L"GAME_WINDOW",
		WindowTitle,
		WindowStyle,
		windowPosX,
		windowPosY,
		windowWidth,
		windowHeight,
		NULL,		//�e�E�B���h�E�Ȃ�
		NULL,		//���j���[�Ȃ�
		GetModuleHandle(0),
		NULL);		//�����E�B���h�E�Ȃ�
}
void CreateDevice()
{
#ifdef _DEBUG
	//�f�o�b�O���[�h�ł́A�f�o�b�O���C���[��L��������
	ComPtr<ID3D12Debug> debug;
	Hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
	assert(SUCCEEDED(Hr));
	debug->EnableDebugLayer();
#endif
	//�f�o�C�X������(�ȈՃo�[�W����)
	{
		Hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0,
			IID_PPV_ARGS(Device.GetAddressOf()));
		assert(SUCCEEDED(Hr));
	}
	//�R�}���h
	{
		//�R�}���h�A���P�[�^������
		Hr = Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(CommandAllocator.GetAddressOf()));
		assert(SUCCEEDED(Hr));

		//�R�}���h���X�g������
		Hr = Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
			CommandAllocator.Get(), nullptr, IID_PPV_ARGS(CommandList.GetAddressOf()));
		assert(SUCCEEDED(Hr));

		//�R�}���h�L���[������
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;		//GPU�^�C���A�E�g���L��
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;		//���ڃR�}���h�L���[
		Hr = Device->CreateCommandQueue(&desc, IID_PPV_ARGS(CommandQueue.GetAddressOf()));
		assert(SUCCEEDED(Hr));

		//�R�}���h�A���P�[�^������
		Hr = Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(CommandAllocator2.GetAddressOf()));
		assert(SUCCEEDED(Hr));

		//�R�}���h���X�g������
		Hr = Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
			CommandAllocator2.Get(), nullptr, IID_PPV_ARGS(CommandList2.GetAddressOf()));
		assert(SUCCEEDED(Hr));

	}
	//�t�F���X
	{
		//GPU�̏����������`�F�b�N����t�F���X������
		Hr = Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(Fence.GetAddressOf()));
		assert(SUCCEEDED(Hr));
		FenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
		assert(FenceEvent != nullptr);
		FenceValue = 1;
	}
}
void CreateRenderTarget()
{
	//�X���b�v�`�F�C��������(�����Ƀo�b�N�o�b�t�@���܂܂�Ă���)
	{
		//DXGI�t�@�N�g��������
		ComPtr<IDXGIFactory4> dxgiFactory;
		Hr = CreateDXGIFactory2(0, IID_PPV_ARGS(dxgiFactory.GetAddressOf()));
		assert(SUCCEEDED(Hr));

		//�X���b�v�`�F�C��������
		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.BufferCount = 2; //�o�b�N�o�b�t�@2��
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

		//IDXGISwapChain4�C���^�[�t�F�C�X���T�|�[�g���Ă��邩�q�˂�
		Hr = swapChain1->QueryInterface(IID_PPV_ARGS(SwapChain.GetAddressOf()));
		assert(SUCCEEDED(Hr));
	}
	//�o�b�N�o�b�t�@�u�r���[�v�̓��ꕨ�ł���u�f�B�X�N���v�^�q�[�v�v������
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = 2;//�o�b�N�o�b�t�@�r���[�Q��
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;//RenderTargetView
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;//�V�F�[�_����A�N�Z�X���Ȃ��̂�NONE��OK
		Hr = Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(BbvHeap.GetAddressOf()));
		assert(SUCCEEDED(Hr));
	}
	//�o�b�N�o�b�t�@�u�r���[�v���u�f�B�X�N���v�^�q�[�v�v�ɂ���
	{
		D3D12_CPU_DESCRIPTOR_HANDLE hBbvHeap
			= BbvHeap->GetCPUDescriptorHandleForHeapStart();

		BbvIncSize
			= Device->GetDescriptorHandleIncrementSize(
				D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		for (UINT idx = 0; idx < 2; idx++) {
			//�o�b�N�o�b�t�@�����o��
			Hr = SwapChain->GetBuffer(idx, IID_PPV_ARGS(BackBuffers[idx].GetAddressOf()));
			assert(SUCCEEDED(Hr));
			//�o�b�N�o�b�t�@�̃r���[���q�[�v�ɂ���
			hBbvHeap.ptr += BbvIncSize * idx;
			Device->CreateRenderTargetView(BackBuffers[idx].Get(), nullptr, hBbvHeap);
		}
	}
	//�f�v�X�X�e���V���o�b�t�@������
	{
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;//DEFAULT��������UNKNOWN�ł悵
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;//2�����̃e�N�X�`���f�[�^�Ƃ���
		desc.Width = ClientWidth;//���ƍ����̓����_�[�^�[�Q�b�g�Ɠ���
		desc.Height = ClientHeight;//��ɓ���
		desc.DepthOrArraySize = 1;//�e�N�X�`���z��ł��Ȃ���3D�e�N�X�`���ł��Ȃ�
		desc.Format = DXGI_FORMAT_D32_FLOAT;//�[�x�l�������ݗp�t�H�[�}�b�g
		desc.SampleDesc.Count = 1;//�T���v����1�s�N�Z��������1��
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//���̃o�b�t�@�͐[�x�X�e���V���Ƃ��Ďg�p���܂�
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.MipLevels = 1;
		//�f�v�X�X�e���V���o�b�t�@���N���A����l
		D3D12_CLEAR_VALUE depthClearValue = {};
		depthClearValue.DepthStencil.Depth = 1.0f;//�[���P(�ő�l)�ŃN���A
		depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;//32bit�[�x�l�Ƃ��ăN���A
		//�f�v�X�X�e���V���o�b�t�@�����
		Hr = Device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE, //�f�v�X�������݂Ɏg�p
			&depthClearValue,
			IID_PPV_ARGS(DepthStencilBuffer.GetAddressOf()));
		assert(SUCCEEDED(Hr));
	}
	//�f�v�X�X�e���V���o�b�t�@�u�r���[�v�̓��ꕨ�ł���u�f�X�N���v�^�q�[�v�v������
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};//�[�x�Ɏg����Ƃ��������킩��΂���
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;//�f�v�X�X�e���V���r���[�Ƃ��Ďg��
		desc.NumDescriptors = 1;//�[�x�r���[1�̂�
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		Hr = Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(DsvHeap.GetAddressOf()));
		assert(SUCCEEDED(Hr));
	}
	//�f�v�X�X�e���V���o�b�t�@�u�r���[�v���u�f�B�X�N���v�^�q�[�v�v�ɂ���
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
		desc.Format = DXGI_FORMAT_D32_FLOAT;//�f�v�X�l��32bit�g�p
		desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
		desc.Flags = D3D12_DSV_FLAG_NONE;//�t���O�͓��ɂȂ�
		D3D12_CPU_DESCRIPTOR_HANDLE hDsvHeap
			= DsvHeap->GetCPUDescriptorHandleForHeapStart();
		Device->CreateDepthStencilView(DepthStencilBuffer.Get(), &desc, hDsvHeap);
	}
}
//===2D�p
void CreatePipeline()
{
	//===
	//���[�g�V�O�l�`��
	{
		//�f�B�X�N���v�^�����W�B�f�B�X�N���v�^�q�[�v�ƃV�F�[�_��R�Â�����������B
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

		//���[�g�p�����^���f�B�X�N���v�^�e�[�u���Ƃ��Ďg�p
		D3D12_ROOT_PARAMETER rootParam[2] = {};
		rootParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParam[0].DescriptorTable.pDescriptorRanges = &range[0];
		rootParam[0].DescriptorTable.NumDescriptorRanges = 1;
		rootParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		rootParam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParam[1].DescriptorTable.pDescriptorRanges = &range[1];
		rootParam[1].DescriptorTable.NumDescriptorRanges = 1;
		rootParam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		//�T���v���̋L�q�B���̃T���v�����V�F�[�_�[�� s0 �ɃZ�b�g�����
		D3D12_STATIC_SAMPLER_DESC samplerDesc[1] = {};
		samplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;//��Ԃ��Ȃ�(�j�A���X�g�l�C�o�[)
		samplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//���J��Ԃ�
		samplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//�c�J��Ԃ�
		samplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//���s�J��Ԃ�
		samplerDesc[0].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//�{�[�_�[�̎��͍�
		samplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX;//�~�b�v�}�b�v�ő�l
		samplerDesc[0].MinLOD = 0.0f;//�~�b�v�}�b�v�ŏ��l
		samplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//�I�[�o�[�T���v�����O�̍ۃ��T���v�����O���Ȃ��H
		samplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//�s�N�Z���V�F�[�_����̂݉�

		//���[�g�V�O�j�`���̋L�q
		D3D12_ROOT_SIGNATURE_DESC desc = {};
		desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		desc.pParameters = rootParam;
		desc.NumParameters = _countof(rootParam);
		desc.pStaticSamplers = samplerDesc;//�T���v���[�̐擪�A�h���X
		desc.NumStaticSamplers = _countof(samplerDesc);//�T���v���[��

		//���[�g�V�O�l�`�����V���A���C�Y��blob(��)������B
		ComPtr<ID3DBlob> blob;
		Hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, nullptr);
		assert(SUCCEEDED(Hr));

		//���[�g�V�O�l�`��������
		Hr = Device->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(),
			IID_PPV_ARGS(RootSignature.GetAddressOf()));
		assert(SUCCEEDED(Hr));
	}

	//�V�F�[�_�ǂݍ���
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
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//�S�ď�������
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;//�����������̗p
	depthStencilDesc.StencilEnable = false;//�X�e���V���o�b�t�@�͎g��Ȃ�

	//�����܂ł̋L�q���܂Ƃ߂ăp�C�v���C���X�e�[�g�I�u�W�F�N�g������
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

	//�o�͗̈��ݒ�
	Viewport.TopLeftX = 0.0f;
	Viewport.TopLeftY = 0.0f;
	Viewport.Width = (float)ClientWidth;
	Viewport.Height = (float)ClientHeight;
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;
	
	//�؂����`��ݒ�
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

//�p�u���b�N�Ȋ֐�---------------------------------------------------------------
//�V�X�e���n
void window(LPCWSTR windowTitle, int clientWidth, int clientHeight, bool windowed, int numDescriptors, int clientPosX, int clientPosY)
{
	WindowTitle = windowTitle;
	ClientWidth = clientWidth;
	ClientHeight = clientHeight;
	ClientPosX = (GetSystemMetrics(SM_CXSCREEN) - ClientWidth) / 2;//�����\��
	if (clientPosX>=0)ClientPosX = clientPosX;
	ClientPosY = (GetSystemMetrics(SM_CYSCREEN) - ClientHeight) / 2;//�����\��
	if (clientPosY>=0)ClientPosY = clientPosY;
	Aspect = static_cast<float>(ClientWidth) / ClientHeight;
	WindowStyle = WS_POPUP;//Alt + F4�ŕ���
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
	//���݂�Fence�l���R�}���h�I�����Fence�ɏ������܂��悤�ɂ���
	UINT64 fvalue = FenceValue;
	CommandQueue->Signal(Fence.Get(), fvalue);
	FenceValue++;

	//�܂��R�}���h�L���[���I�����Ă��Ȃ����Ƃ��m�F����
	if (Fence->GetCompletedValue() < fvalue)
	{
		//����Fence�ɂ����āAfvalue �̒l�ɂȂ�����C�x���g�𔭐�������
		Fence->SetEventOnCompletion(fvalue, FenceEvent);
		//�C�x���g����������܂ő҂�
		WaitForSingleObject(FenceEvent, INFINITE);
	}
}
void closeEventHandle()
{
	CloseHandle(FenceEvent);
}
//�R���X�^���g�o�b�t�@�A�e�N�X�`���o�b�t�@�̃f�B�X�N���v�^�q�[�v
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
	
	//�f�B�t���[�Y�F�݂̂̃|���S���ɓ\��t����e�N�X�`��������
	CreateWhiteTexture();

	return hr;
}
//�o�b�t�@�n
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
	
	//�P�s�̃s�b�`��256�̔{���ɂ��Ă���(�o�b�t�@�T�C�Y��256�̔{���łȂ���΂����Ȃ�)
	const UINT64 alignedRowPitch = (texWidth * bytePerPixel + 0xff) & ~0xff;

	//�A�b�v���[�h�p���ԃo�b�t�@������A���f�[�^���R�s�[���Ă���
	ComPtr<ID3D12Resource> uploadBuf;
	{
		//�e�N�X�`���ł͂Ȃ��t�c�[�̃o�b�t�@�Ƃ��Ă���
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

		//���f�[�^��uploadbuff�Ɉ�U�R�s�[���܂�
		uint8_t* mapBuf = nullptr;
		Hr = uploadBuf->Map(0, nullptr, (void**)&mapBuf);//�}�b�v
		auto srcAddress = pixels;
		auto originalRowPitch = texWidth * bytePerPixel;
		for (UINT y = 0; y < texHeight; ++y) {
			memcpy(mapBuf, srcAddress, originalRowPitch);
			//1�s���Ƃ̒�������킹�Ă��
			mapBuf += alignedRowPitch;
			srcAddress += originalRowPitch;
		}
		uploadBuf->Unmap(0, nullptr);//�A���}�b�v
	}

	//�����āA�ŏI�R�s�[��ł���e�N�X�`���o�b�t�@�����
	{
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;//CPU����A�N�Z�X���Ȃ��B�����������B
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		prop.CreationNodeMask = 1;
		prop.VisibleNodeMask = 1;
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;//���̃o�b�t�@�ƈႤ
		desc.Alignment = 0;
		desc.Width = texWidth;
		desc.Height = texHeight;//���̃o�b�t�@�ƈႤ
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//���̃o�b�t�@�ƈႤ
		desc.SampleDesc = { 1, 0 };
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;//���̃o�b�t�@�ƈႤ
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

	//GPU��uploadBuf����textureBuf�փR�s�[���钷�����̂肪�n�܂�܂�

	//�܂��R�s�[�����P�[�V�����̏����E�t�b�g�v�����g�w��
	D3D12_TEXTURE_COPY_LOCATION src = {};
	src.pResource = uploadBuf.Get();
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	src.PlacedFootprint.Footprint.Width = static_cast<UINT>(texWidth);
	src.PlacedFootprint.Footprint.Height = static_cast<UINT>(texHeight);
	src.PlacedFootprint.Footprint.Depth = static_cast<UINT>(1);
	src.PlacedFootprint.Footprint.RowPitch = static_cast<UINT>(alignedRowPitch);
	src.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//�R�s�[�惍�P�[�V�����̏����E�T�u���\�[�X�C���f�b�N�X�w��
	D3D12_TEXTURE_COPY_LOCATION dst = {};
	dst.pResource = TextureBuf.Get();
	dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dst.SubresourceIndex = 0;

	//�R�}���h���X�g�ŃR�s�[��\�񂵂܂���I�I�I
	CommandList2->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
	//���Ă��Ƃ̓o���A������̂ł�
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = TextureBuf.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	CommandList2->ResourceBarrier(1, &barrier);
	//uploadBuf�A�����[�h
	CommandList2->DiscardResource(uploadBuf.Get(), nullptr);
	//�R�}���h���X�g�����
	CommandList2->Close();
	//���s
	ID3D12CommandList* commandLists[] = { CommandList2.Get() };
	CommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
	//���\�[�X��GPU�ɓ]�������܂őҋ@����
	waitGPU();

	//�R�}���h�A���P�[�^�����Z�b�g
	HRESULT Hr = CommandAllocator2->Reset();
	assert(SUCCEEDED(Hr));
	//�R�}���h���X�g�����Z�b�g
	Hr = CommandList2->Reset(CommandAllocator2.Get(), nullptr);
	assert(SUCCEEDED(Hr));

	return S_OK;
}
HRESULT createTextureBufferFromFile(const char* filename, ComPtr<ID3D12Resource>& textureBuffer, int* w, int* h)
{
	//�t�@�C����ǂݍ��݁A���f�[�^�����o��
	unsigned char* pixels = nullptr;
	int texWidth = 0, texHeight = 0, bytePerPixel = 4;
	pixels = stbi_load(filename, &texWidth, &texHeight, nullptr, bytePerPixel);
	if (pixels == nullptr)
	{
		MessageBoxA(0, filename, "�t�@�C�����Ȃ�����", 0);
		exit(0);
	}
	if (w)*w = texWidth;
	if (h)*h = texHeight;

	createTextureBuffer(pixels, texWidth, texHeight, textureBuffer);

	//�J��
	stbi_image_free(pixels);

	return S_OK;
}
//�f�B�X�N���v�^�n
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
	desc.Texture2D.MipLevels = 1;//�~�b�v�}�b�v�͎g�p���Ȃ��̂�1
	auto hCbvTbvHeap = CbvTbvHeap->GetCPUDescriptorHandleForHeapStart();
	hCbvTbvHeap.ptr += CbvTbvIncSize * CurrentCbvTbvIdx;
	Device->CreateShaderResourceView(textureBuffer.Get(), &desc, hCbvTbvHeap);
	return CurrentCbvTbvIdx++;
}
//�`��n
void clearColor(float r, float g, float b)
{
	ClearColor[0] = r; ClearColor[1] = g; ClearColor[2] = b;
}
void beginRender()
{
	//���݂̃o�b�N�o�b�t�@�̃C���f�b�N�X���擾�B���̃v���O�����̏ꍇ0 or 1�ɂȂ�B
	BackBufIdx = SwapChain->GetCurrentBackBufferIndex();

	//�o���A�Ńo�b�N�o�b�t�@��`��^�[�Q�b�g�ɐ؂�ւ���
	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;//���̃o���A�͏�ԑJ�ڃ^�C�v
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = BackBuffers[BackBufIdx].Get();//���\�[�X�̓o�b�N�o�b�t�@
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;//�J�ڑO��Present
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;//�J�ڌ�͕`��^�[�Q�b�g
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	CommandList->ResourceBarrier(1, &barrier);

	//�o�b�N�o�b�t�@�̏ꏊ���w���f�B�X�N���v�^�q�[�v�n���h����p�ӂ���
	auto hBbvHeap = BbvHeap->GetCPUDescriptorHandleForHeapStart();
	hBbvHeap.ptr += BackBufIdx * BbvIncSize;
	//�f�v�X�X�e���V���o�b�t�@�̃f�B�X�N���v�^�n���h����p�ӂ���
	auto hDsvHeap = DsvHeap->GetCPUDescriptorHandleForHeapStart();
	//�o�b�N�o�b�t�@�ƃf�v�X�X�e���V���o�b�t�@��`��^�[�Q�b�g�Ƃ��Đݒ肷��
	CommandList->OMSetRenderTargets(1, &hBbvHeap, false, &hDsvHeap);
	//�o�b�N�o�b�t�@���N���A����
	CommandList->ClearRenderTargetView(hBbvHeap, ClearColor, 0, nullptr);
	//�f�v�X�X�e���V���o�b�t�@���N���A����
	CommandList->ClearDepthStencilView(hDsvHeap, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	//�r���[�|�[�g�ƃV�U�[��`���Z�b�g
	CommandList->RSSetViewports(1, &Viewport);
	CommandList->RSSetScissorRects(1, &ScissorRect);

	//�p�C�v���C���X�e�[�g���Z�b�g
	CommandList->SetPipelineState(PipelineState.Get());
	//���[�g�V�O�j�`�����Z�b�g
	CommandList->SetGraphicsRootSignature(RootSignature.Get());
	//�f�B�X�N���v�^�q�[�v���f�o�t�ɃZ�b�g
	CommandList->SetDescriptorHeaps(1, CbvTbvHeap.GetAddressOf());
}
void endRender()
{
	//�o���A�Ńo�b�N�o�b�t�@��\���p�ɐ؂�ւ���
	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;//���̃o���A�͏�ԑJ�ڃ^�C�v
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = BackBuffers[BackBufIdx].Get();//���\�[�X�̓o�b�N�o�b�t�@
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;//�J�ڑO�͕`��^�[�Q�b�g
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;//�J�ڌ��Present
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	CommandList->ResourceBarrier(1, &barrier);

	//�R�}���h���X�g���N���[�Y����
	CommandList->Close();
	//�R�}���h���X�g�����s����
	ID3D12CommandList* commandLists[] = { CommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
	//�`�抮����҂�
	waitGPU();

	//�o�b�N�o�b�t�@��\��
	SwapChain->Present(1, 0);

	//�R�}���h�A���P�[�^�����Z�b�g
	Hr = CommandAllocator->Reset();
	assert(SUCCEEDED(Hr));
	//�R�}���h���X�g�����Z�b�g
	Hr = CommandList->Reset(CommandAllocator.Get(), nullptr);
	assert(SUCCEEDED(Hr));
}
//Get�n
float clientWidth() { return (float)ClientWidth; }
float clientHeight() { return (float)ClientHeight; }

//�QD----------------------------------------------------------------------------

//�����`���_�o�b�t�@�B�g���܂킵����B
ComPtr<ID3D12Resource>   SquareVertexBuffer = nullptr;
D3D12_VERTEX_BUFFER_VIEW SquareVbv;
void CreateSquareVertexBuffer()
{
	//���L���钸�_�o�b�t�@
	{
		unsigned numVertexElements = 5;//�P���_�̗v�f��
		float vertices[] = {
			//position            texcoord
			-0.5f,  0.5f,  0.0f,  0.0f,  0.0f, //����
			-0.5f, -0.5f,  0.0f,  0.0f,  1.0f, //����
			 0.5f,  0.5f,  0.0f,  1.0f,  0.0f, //�E��
			 0.5f, -0.5f,  0.0f,  1.0f,  1.0f, //�E��
		};
		//�f�[�^�T�C�Y�����߂Ă���
		UINT sizeInBytes = sizeof(vertices);
		UINT strideInBytes = sizeof(float) * numVertexElements;
		//�o�b�t�@������
		Hr = createBuffer(sizeInBytes, SquareVertexBuffer);
		assert(SUCCEEDED(Hr));
		//�o�b�t�@�Ƀf�[�^������
		Hr = updateBuffer(vertices, sizeInBytes, SquareVertexBuffer);
		assert(SUCCEEDED(Hr));
		//�r���[������
		createVertexBufferView(SquareVertexBuffer, sizeInBytes, strideInBytes, SquareVbv);
	}
}

//�~�̒��_�o�b�t�@�i�����̑傫���j
const int NumCircles = 5;
int NumAngles[NumCircles] = { 8,16,32,64,128 };
ComPtr<ID3D12Resource>   CircleVertexBuffer[NumCircles];
D3D12_VERTEX_BUFFER_VIEW CircleVbv[NumCircles];
void CreateCircleVertexBuffers()
{
	for(int i=0; i<NumCircles; ++i){
		int num = NumAngles[i];
		const int numVertexElements = 5;//�P���_�̗v�f��
		float* v = new float[num * numVertexElements]{};//�I�[���[���N���A
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

		//�f�[�^�T�C�Y�����߂Ă���
		UINT sizeInBytes = sizeof(float) * num * numVertexElements;
		UINT strideInBytes = sizeof(float) * numVertexElements;
		//�o�b�t�@������
		Hr = createBuffer(sizeInBytes, CircleVertexBuffer[i]);
		assert(SUCCEEDED(Hr));
		//�o�b�t�@�Ƀf�[�^������
		Hr = updateBuffer(v, sizeInBytes, CircleVertexBuffer[i]);
		assert(SUCCEEDED(Hr));
		//�r���[������
		createVertexBufferView(CircleVertexBuffer[i], 
			sizeInBytes, strideInBytes, CircleVbv[i]);

		delete[] v;
	}
}

//�}�b�v�p�R���X�^���g�o�b�t�@�\����
struct CONST_BUF0 {
	XMMATRIX worldViewProj;
	XMFLOAT4 diffuse;
};

//�R���X�^���g�\����
struct CONSTANT {
	ComPtr<ID3D12Resource> constBuffer0 = nullptr;
	CONST_BUF0* cb0 = nullptr;
	UINT cbvIdx = 0;
};
//�R���X�^���g�z��
std::vector<CONSTANT>Constants;
//�R���X�^���g�z����w���C���f�b�N�X
UINT ConstantIdxCnt = 0;
void InitConstantIdxCnt()//quit���ŌĂяo��
{
	ConstantIdxCnt = 0;
}
//�R���X�^���g������Ȃ����������
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

//�e�N�X�`���\����
struct TEXTURE {
	ComPtr<ID3D12Resource> textureBuffer;
	UINT tbvIdx=-1;
	float texWidth=0;
	float texHeight=0;
};
//�e�N�X�`���z��
std::vector<TEXTURE>Textures;
//�e�N�X�`���d���`�F�b�N
std::unordered_map<std::string, int> DuplicateCheckMap;
//#####debug#####
size_t numLoadTextures() { return Textures.size(); }
//�e�N�X�`����ǂݍ���
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
//�e�N�X�`���𒣂�t�����l�p�`�̕`��
void drawImage(UINT cbvIdx, UINT tbvIdx)
{
	//���_���Z�b�g
	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	CommandList->IASetVertexBuffers(0, 1, &SquareVbv);
	//�R���X�^���g���Z�b�g
	auto hCbvTbvHeap = CbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
	hCbvTbvHeap.ptr += CbvTbvIncSize * cbvIdx;
	CommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);
	//�e�N�X�`�����Z�b�g
	hCbvTbvHeap = CbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
	hCbvTbvHeap.ptr += CbvTbvIncSize * tbvIdx;
	CommandList->SetGraphicsRootDescriptorTable(1, hCbvTbvHeap);
	//�`��
	CommandList->DrawInstanced(4, 1, 0, 0);

	//draw������K���J�E���g�A�b�v
	ConstantIdxCnt++;
}

//�h��Ԃ��F
float FillR = 1, FillG = 1, FillB = 1, FillA = 1;
void fill(float r, float g, float b, float a)
{
	FillR = r; FillG = g; FillB = b; FillA = a;
}


//��`�`�惂�[�h
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

//�摜�`��
void image(int textureIdx, float px, float py, float rad, float sx, float sy)
{
	//�R���X�^���g������Ȃ����������
	AutoCreateConstant();
	//�}�g���b�N�X�˃R���X�^���g�ɃZ�b�g
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
	//�f�B�t���[�Y�J���[�˃R���X�^���g�ɃZ�b�g
	Constants[ConstantIdxCnt].cb0->diffuse = { FillR,FillG,FillB,FillA };

	//�`��
	drawImage(Constants[ConstantIdxCnt].cbvIdx, Textures[textureIdx].tbvIdx);
}
//���w�җp�t�@�C�������ڎw��o�[�W����
void image(const char* filename, float px, float py, float rad, float sx, float sy)
{
	int idx = loadImage(filename);
	image(idx, px, py, rad, sx, sy);
}

//diffuse�F�݂̂̃|���S���ɓ\��t���锒�e�N�X�`��
ComPtr<ID3D12Resource> WhiteTexture;
int WhiteTbvIdx;
void CreateWhiteTexture()//createDescriptorHeap����Ăяo�����
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

//�֊s���̐F
float StrokeR = 1, StrokeG = 1, StrokeB = 1, StrokeA = 1;
void stroke(float r, float g, float b, float a)
{
	StrokeR = r; StrokeG = g; StrokeB = b; StrokeA = a;
}
//�֊s���̑���
float StrokeWeight = 1;
void strokeWeight(float sw)
{
	StrokeWeight = sw;
}

//�_
void point(float px, float py)
{
	//�R���X�^���g������Ȃ����������
	AutoCreateConstant();
	//�}�g���b�N�X�˃R���X�^���g�ɃZ�b�g
	XMMATRIX world =
		XMMatrixScaling(StrokeWeight, StrokeWeight, 1)
		* XMMatrixTranslation(px, -py, 0)
		* OrthoProj;
	Constants[ConstantIdxCnt].cb0->worldViewProj = world;
	//�f�B�t���[�Y�J���[�˃R���X�^���g�ɃZ�b�g
	Constants[ConstantIdxCnt].cb0->diffuse = { StrokeR,StrokeG,StrokeB,StrokeA };

	int idx=0;
	if      (StrokeWeight <=  10)idx = 0;
	else if (StrokeWeight <=  50)idx = 1;
	else if (StrokeWeight <= 200)idx = 2;
	else if (StrokeWeight <= 800)idx = 3;
	else idx = 4;
	auto& Vbv = CircleVbv[idx];

	//���_���Z�b�g
	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	CommandList->IASetVertexBuffers(0, 1, &Vbv);
	//�R���X�^���g���Z�b�g
	auto hCbvTbvHeap = CbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
	hCbvTbvHeap.ptr += CbvTbvIncSize * Constants[ConstantIdxCnt].cbvIdx;
	CommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);
	//�e�N�X�`�����Z�b�g
	hCbvTbvHeap = CbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
	hCbvTbvHeap.ptr += CbvTbvIncSize * WhiteTbvIdx;
	CommandList->SetGraphicsRootDescriptorTable(1, hCbvTbvHeap);
	//�`��
	UINT numVertices = Vbv.SizeInBytes / Vbv.StrideInBytes;
	CommandList->DrawInstanced(numVertices, 1, 0, 0);
	ConstantIdxCnt++;
}
//�I�_�ɓ_��`�����ۂ�
bool DrawEndPointFlag = true;
//��
void line(float sx, float sy, float ex, float ey)
{
	//�R���X�^���g������Ȃ����������
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
	//�f�B�t���[�Y�J���[�˃R���X�^���g�ɃZ�b�g
	Constants[ConstantIdxCnt].cb0->diffuse = { StrokeR,StrokeG,StrokeB,StrokeA };

	//�`��
	drawImage(Constants[ConstantIdxCnt].cbvIdx, WhiteTbvIdx);
	if (StrokeWeight > 1) {
		//�n�_
		point(sx, sy);
		//�I�_
		if (DrawEndPointFlag)point(ex, ey);
	}
}
//��`
void rect(float px, float py, float w, float h, float rad)
{
	//�R���X�^���g������Ȃ����������
	AutoCreateConstant();
	//2D�p�}�g���b�N�X�˃R���X�^���g�ɃZ�b�g
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
	//�f�B�t���[�Y�J���[�˃R���X�^���g�ɃZ�b�g
	Constants[ConstantIdxCnt].cb0->diffuse = { FillR,FillG,FillB,FillA };

	//�`��
	if (FillA > 0.0f) {
		drawImage(Constants[ConstantIdxCnt].cbvIdx, WhiteTbvIdx);
	}

	//�֊s
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
//�~
void circle(float px, float py, float diameter)
{
	//�R���X�^���g������Ȃ����������
	AutoCreateConstant();
	//�}�g���b�N�X�˃R���X�^���g�ɃZ�b�g
	XMMATRIX world =
		XMMatrixScaling(diameter, diameter, 1)
		* XMMatrixTranslation(px, -py, 0)
		* OrthoProj;
	Constants[ConstantIdxCnt].cb0->worldViewProj = world;
	//�f�B�t���[�Y�J���[�˃R���X�^���g�ɃZ�b�g
	Constants[ConstantIdxCnt].cb0->diffuse = { FillR,FillG,FillB,FillA };

	int idx = 0;
	if		(diameter <=  10) { idx = 0; }
	else if (diameter <=  50) { idx = 1; }
	else if (diameter <= 200) { idx = 2; }
	else if (diameter <= 800) { idx = 3; }
	else					  { idx = 4; }
	auto& Vbv = CircleVbv[idx];

	//���_���Z�b�g
	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	CommandList->IASetVertexBuffers(0, 1, &Vbv);
	//�R���X�^���g���Z�b�g
	auto hCbvTbvHeap = CbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
	hCbvTbvHeap.ptr += CbvTbvIncSize * Constants[ConstantIdxCnt].cbvIdx;
	CommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);
	//�e�N�X�`�����Z�b�g
	hCbvTbvHeap = CbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
	hCbvTbvHeap.ptr += CbvTbvIncSize * WhiteTbvIdx;
	CommandList->SetGraphicsRootDescriptorTable(1, hCbvTbvHeap);
	//�`��
	UINT numVertices = Vbv.SizeInBytes / Vbv.StrideInBytes;
	CommandList->DrawInstanced(numVertices, 1, 0, 0);
	ConstantIdxCnt++;

	//�֊s
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

//�t�H���g
// 
//���ݕ`�撆�̃t�H���g�t�F�C�X�\����
struct CURRENT_FONT_FACE {
	std::string name; unsigned long charset; int idx; int size;
};
CURRENT_FONT_FACE CurFontFace{ "�l�r �S�V�b�N",SHIFTJIS_CHARSET,0,50 };
//FontFace�����Ƃɂ�����t���ĊǗ�����}�b�v
std::unordered_map<std::string, int> FontFaceIdxMap{ {CurFontFace.name, 0} };
int FontFaceIdxCnt = 0;
//�`�悷��t�H���g�t�F�C�X��ݒ肷��
void fontFace(const char* fontname, unsigned charset)
{
	//�t�H���g����charset
	CurFontFace.name = fontname;
	CurFontFace.charset = charset;

	//CurFontFace.id���Z�b�g����
	auto itr = FontFaceIdxMap.find(fontname);
	if (itr == FontFaceIdxMap.end()) {
		FontFaceIdxCnt++;
		assert(FontFaceIdxCnt < 32);//FontFace����ȏ�ǉ��ł��܂���;
		FontFaceIdxMap[fontname] = FontFaceIdxCnt;
		CurFontFace.idx = FontFaceIdxCnt;
	}
	else {
		CurFontFace.idx = itr->second;
	}
}
//�t�H���g�T�C�Y��ݒ肷��
void fontSize(int size)
{
	assert(size <= 2048);//"FontSize","2048���傫���T�C�Y�͎w��ł��܂���";
	CurFontFace.size = size;
}

//�t�H���g�p�e�N�X�`���\����(���̃}�b�v�ɕۑ����Ă����t�H���g�̕`��ɕK�v�ȃf�[�^�B)
struct FONT_TEXTURE {
	ComPtr<ID3D12Resource> textureBuffer = nullptr;
	UINT tbvIdx = 0;
	float texWidth=0, texHeight=0;//�e�N�X�`���̕��A����
	float drawWidth=0, drawHeight=0;//�`�敝�A����
	float ofstX=0, ofstY=0;//�`�悷��Ƃ��ɂ��炷�l
};
//�t�H���g�e�N�X�`���f�[�^���Ǘ�����}�b�v
static std::unordered_map<DWORD, FONT_TEXTURE> FontTextureMap;
size_t numFontTextures() { return FontTextureMap.size(); }
//�P�������̃t�H���g�e�N�X�`���������ď�̃}�b�v�ɒǉ�����
FONT_TEXTURE* CreateFontTexture(DWORD key)
{
	//�t�H���g�i�T�C�Y��t�H���g�̎�ށj�����߂�I
	HFONT hFont = CreateFontA(
		CurFontFace.size, 0, 0, 0, 0, 0, 0, 0,
		CurFontFace.charset,
		OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY, FIXED_PITCH | FF_MODERN,
		CurFontFace.name.c_str()
	);
	assert(hFont); //"Font", "Create error"

	//�f�o�C�X�R���e�L�X�g�擾
	HDC hdc = GetDC(NULL);
	//�f�o�C�X�R���e�L�X�g�Ƀt�H���g��ݒ�
	HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

	//�t�H���g�̊e�퐡�@�ƃA���t�@�r�b�g�}�b�v���擾
	TEXTMETRICA tm;
	GetTextMetricsA(hdc, &tm);
	GLYPHMETRICS gm;
	CONST MAT2 mat = { {0,1},{0,0},{0,0},{0,1} };
	UINT code = key & 0xffff;//key���當���R�[�h�����o��
	DWORD alphaBmpSize = GetGlyphOutlineA(hdc, code, GGO_GRAY4_BITMAP, &gm, 0, NULL, &mat);
	BYTE* alphaBmpBuf = new BYTE[alphaBmpSize];
	GetGlyphOutlineA(hdc, code, GGO_GRAY4_BITMAP, &gm, alphaBmpSize, alphaBmpBuf, &mat);
	//���l�̊K�� (GGO_GRAY4_BITMAP��17�K���BalphaBmpBuf[i]�͂O�`�P�U�̒l�ƂȂ�)
	BYTE tone = 16;//�ő�l

	//�f�o�C�X�R���e�L�X�g�ƃt�H���g�n���h���̊J��
	DeleteObject(hFont);
	SelectObject(hdc, oldFont);
	ReleaseDC(NULL, hdc);

	//�摜�̕��ƍ���
	UINT texWidth = (gm.gmBlackBoxX + 3) & 0xfffc;//4�̔{���ɂ���
	UINT texHeight = gm.gmBlackBoxY;

	//alphaBmpBuf�����Ƀt�H���g�摜�f�[�^������
	BYTE* pixels = new BYTE[texWidth * texHeight * 4];
	UINT x, y, i;
	for (y = 0; y < texHeight; ++y) {
		for (x = 0; x < texWidth; x++) {
			i = y * texWidth + x;
			if (i < alphaBmpSize) {
				pixels[i*4 + 0] = 0xff;//r
				pixels[i*4 + 1] = 0xff;//g
				pixels[i*4 + 2] = 0xff;//b
				pixels[i*4 + 3] = alphaBmpBuf[i] * 255 / tone;//0�`16��0�`255�ɕϊ�
			}
		}
	}

	//FONT_TEXTURE(�`��ɕK�v�ȃf�[�^�B)���}�b�v�ɓo�^
	createTextureBuffer(pixels, texWidth, texHeight, FontTextureMap[key].textureBuffer);
	FontTextureMap[key].tbvIdx = createTextureBufferView(FontTextureMap[key].textureBuffer);
	FontTextureMap[key].texWidth = (float)texWidth;
	FontTextureMap[key].texHeight = (float)texHeight;//�e�N�X�`���̕��ƍ���
	FontTextureMap[key].drawWidth = (float)gm.gmCellIncX;
	FontTextureMap[key].drawHeight = (float)tm.tmHeight;//�`�悷�镝�ƍ���
	FontTextureMap[key].ofstX = (float)gm.gmptGlyphOrigin.x;
	FontTextureMap[key].ofstY = (float)tm.tmAscent - gm.gmptGlyphOrigin.y;//�`�悷�鎞�ɂ��炷�l

	delete[] alphaBmpBuf;
	delete[] pixels;

	return &FontTextureMap[key];
}

int FontRectMode = CORNER;

//�w�肵����������w�肵���X�N���[�����W�ŕ`�悷��
float text(const char* str, float x, float y)
{
	int len = (int)strlen(str);

	//���[�v���Ȃ���P�������`�悵�Ă���
	for (int i = 0; i < len; i++) {

		//�����R�[�h�̌���(�}���`�o�C�g�R�[�h��������Ȃ��O��)
		WORD code;
		if (IsDBCSLeadByte(str[i])) {
			//2�o�C�g�����̃R�[�h��[�擱�R�[�h] + [�����R�[�h]�ł�
			code = (BYTE)str[i] << 8 | (BYTE)str[i + 1];
			i++;
		}
		else {
			//1�o�C�g�����̃R�[�h
			code = str[i];
		}

		//�}�b�v�����pkey(�t�H���g�t�F�C�Xidx�{�t�H���g�T�C�Y�{�����R�[�h)������
		DWORD key = CurFontFace.idx << 27 | CurFontFace.size << 16 | code;

		//key�Ń}�b�v���Ƀe�N�X�`�������邩�T��
		FONT_TEXTURE* fontTex = 0;
		auto itr = FontTextureMap.find(key);
		if (itr == FontTextureMap.end()) {
			//�Ȃ������̂Ńt�H���g�̃e�N�X�`�������̏�ł����ăA�h���X�����炤
			fontTex = CreateFontTexture(key);
		}
		else {
			//�������̂ŃA�h���X���擾����
			fontTex = &itr->second;
		}

		//�R���X�^���g������Ȃ����������
		AutoCreateConstant();
		
		XMMATRIX world;
		if (FontRectMode == CORNER) {
			world = XMMatrixTranslation(0.5f, -0.5f, 0);//Px,Py�̈ʒu�ɉ摜�̍��オ����
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
		
		//�`��
		drawImage(Constants[ConstantIdxCnt].cbvIdx, fontTex->tbvIdx);

		//���̕����̕`��ʒu�������߂Ă���
		x += fontTex->drawWidth;
	}
	//���ɑ����ĕʂ̕������\�����邽�߂̍��W��Ԃ�
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
