#include"graphic.h"
#include"model.h"

//�V�X�e��----------------------------------------------------------------------
HRESULT Hr;
ComPtr<ID3D12Device> Device;
ComPtr<ID3D12GraphicsCommandList> CommandList;
//���\�[�X----------------------------------------------------------------------
//���_�o�b�t�@
UINT NumVertices = 0;
ComPtr<ID3D12Resource> VertexBuf = nullptr;
D3D12_VERTEX_BUFFER_VIEW VertexBufView;
//���_�C���f�b�N�X�o�b�t�@
UINT NumIndices = 0;
ComPtr<ID3D12Resource> IndexBuf = nullptr;
D3D12_INDEX_BUFFER_VIEW	IndexBufView;
//�R���X�^���g�o�b�t�@
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
//�e�N�X�`���o�b�t�@
ComPtr<ID3D12Resource> TextureBuf = nullptr;
//�f�B�X�N���v�^�q�[�v
ComPtr<ID3D12DescriptorHeap> CbvTbvHeap = nullptr;// ConstBufView �� TextureBufView �� Heap
UINT CbvTbvIncSize = 0;

//�G���g���[�|�C���g
INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	Device = device();
	CommandList = commandList();

	//���\�[�X
	{
		//���_�o�b�t�@
		{
			//�T�C�Y�����߂�
			UINT sizeInBytes = sizeof(Vertices);
			UINT strideInBytes = sizeof(float) * NumVertexElements;
			NumVertices = sizeInBytes / strideInBytes;

			//�o�b�t�@������
			Hr = createBuffer(sizeInBytes, VertexBuf);
			assert(SUCCEEDED(Hr));

			//�o�b�t�@�Ƀf�[�^���R�s�[
			UINT8* mappedBuf;
			Hr = VertexBuf->Map(0, nullptr, reinterpret_cast<void**>(&mappedBuf));
			assert(SUCCEEDED(Hr));
			memcpy(mappedBuf, Vertices, sizeInBytes);
			VertexBuf->Unmap(0, nullptr);

			//�o�b�t�@�̃r���[�����������Ă����B�i�f�B�X�N���v�^�q�[�v�ɍ��Ȃ��Ă悢�j
			VertexBufView.BufferLocation = VertexBuf->GetGPUVirtualAddress();
			VertexBufView.SizeInBytes = sizeInBytes;
			VertexBufView.StrideInBytes = strideInBytes;
		}
		//���_�C���f�b�N�X�o�b�t�@
		{
			//�T�C�Y�����߂�
			UINT sizeInBytes = sizeof(Indices);
			NumIndices =  sizeInBytes / sizeof(unsigned short);

			//�C���f�b�N�X�o�b�t�@������
			Hr = createBuffer(sizeInBytes, IndexBuf);
			assert(SUCCEEDED(Hr));

			//������o�b�t�@�Ƀf�[�^���R�s�[
			UINT8* mappedBuf = nullptr;
			Hr = IndexBuf->Map(0, nullptr, (void**)&mappedBuf);
			assert(SUCCEEDED(Hr));
			memcpy(mappedBuf, Indices, sizeInBytes);
			IndexBuf->Unmap(0, nullptr);

			//�C���f�b�N�X�o�b�t�@�r���[������
			IndexBufView.BufferLocation = IndexBuf->GetGPUVirtualAddress();
			IndexBufView.SizeInBytes = sizeInBytes;
			IndexBufView.Format = DXGI_FORMAT_R16_UINT;
		}
		//�R���X�^���g�o�b�t�@�O
		{
			Hr = createBuffer(256, ConstBuf0);
			assert(SUCCEEDED(Hr));

			//�}�b�v����B�R���X�^���g�o�b�t�@��Unmap���Ȃ�
			Hr = ConstBuf0->Map(0, nullptr, (void**)&CB0);
			assert(SUCCEEDED(Hr));
		}
		//�R���X�^���g�o�b�t�@�P
		{
			Hr = createBuffer(256, ConstBuf0);
			assert(SUCCEEDED(Hr));

			//�}�b�v����B�R���X�^���g�o�b�t�@��Unmap���Ȃ��B
			Hr = ConstBuf1->Map(0, nullptr, (void**)&CB1);
			assert(SUCCEEDED(Hr));
			//�R���X�^���g�o�b�t�@�P�X�V
			CB1->diffuse = {Diffuse[0],Diffuse[1],Diffuse[2],Diffuse[3]};
		}
		//�e�N�X�`���o�b�t�@
		{
			createTextureBuffer(TextureFilename, TextureBuf);
		}
		//�f�B�X�N���v�^�q�[�v
		{
			//�u�r���[�v�̓��ꕨ�ł���u�f�B�X�N���v�^�q�[�v�v������
			{
				D3D12_DESCRIPTOR_HEAP_DESC desc = {};
				desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
				desc.NumDescriptors = 3;//�R���X�^���g�o�b�t�@�Q�ƃe�N�X�`���o�b�t�@�P��
				desc.NodeMask = 0;
				desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
				Hr = Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(CbvTbvHeap.ReleaseAndGetAddressOf()));
				assert(SUCCEEDED(Hr));
			}

			//�n���h��(�|�C���^)�����T�C�Y���擾���Ă���
			CbvTbvIncSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			//�q�[�v�̐擪�A�h���X���n���h���Ɏ擾
			auto hCbvTbvHeap = CbvTbvHeap->GetCPUDescriptorHandleForHeapStart();

			//�R���X�^���g�o�b�t�@�O�́u�r���[�v���u�f�B�X�N���v�^�q�[�v�v�ɂ���
			{
				D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
				desc.BufferLocation = ConstBuf0->GetGPUVirtualAddress();
				desc.SizeInBytes = static_cast<UINT>(ConstBuf0->GetDesc().Width);
				Device->CreateConstantBufferView(&desc, hCbvTbvHeap);
			}

			hCbvTbvHeap.ptr += CbvTbvIncSize;

			//�R���X�^���g�o�b�t�@�P�́u�r���[�v���u�f�B�X�N���v�^�q�[�v�v�ɂ���
			{
				D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
				desc.BufferLocation = ConstBuf1->GetGPUVirtualAddress();
				desc.SizeInBytes = static_cast<UINT>(ConstBuf1->GetDesc().Width);
				Device->CreateConstantBufferView(&desc, hCbvTbvHeap);
			}

			hCbvTbvHeap.ptr += CbvTbvIncSize;

			//�e�N�X�`���o�b�t�@�́u�r���[�v���u�f�B�X�N���v�^�q�[�v�v�ɂ���
			{
				D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
				desc.Format = TextureBuf->GetDesc().Format;
				desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
				desc.Texture2D.MipLevels = 1;//�~�b�v�}�b�v�͎g�p���Ȃ��̂�1
				Device->CreateShaderResourceView(TextureBuf.Get(), &desc, hCbvTbvHeap);
			}
		}
	}{}

	//���C�����[�v
	while (true)
	{
		//�E�B���h�E���b�Z�[�W�̎擾�A���o
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

		//�R���X�^���g�o�b�t�@�X�V
		{
			//��]�p���W�A��
			static float r = 0;
			r += 0.01f;
			//���[���h�}�g���b�N�X
			XMMATRIX world = XMMatrixRotationY(r);
			//�r���[�}�g���b�N�X
			XMFLOAT3 eye = { 0, 0, -2 }, focus = { 0, 0, 0 }, up = { 0, 1, 0 };
			XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&up));
			//�v���W�F�N�V�����}�g���b�N�X
			XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect(), 1.0f, 10.0f);
			CB0->worldViewProj = world * view * proj;
		}

		//�o�b�N�o�b�t�@�ɕ`��
		{
			//���_���Z�b�g
			CommandList->IASetVertexBuffers(0, 1, &VertexBufView);
			CommandList->IASetIndexBuffer(&IndexBufView);

			//�f�B�X�N���v�^�q�[�v���f�o�t�ɃZ�b�g
			CommandList->SetDescriptorHeaps(1, CbvTbvHeap.GetAddressOf());
			//�f�B�X�N���v�^�q�[�v���f�B�X�N���v�^�e�[�u���ɃZ�b�g
			auto hCbvTbvHeap = CbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
			CommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);
			//�`��
			CommandList->DrawIndexedInstanced(NumIndices, 1, 0, 0, 0);
		}
	}{}
	//���
	{
		waitDrawDone();
		ConstBuf1->Unmap(0, nullptr);
		ConstBuf0->Unmap(0, nullptr);
		CloseHandle(FenceEvent);
	}
}
