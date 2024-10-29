#pragma once
#include<d3d12.h>
#include<DirectXMath.h>
#include<wrl.h>//ComPtr
using namespace DirectX;
using namespace Microsoft::WRL;//ComPtr

void window();
void beginDraw(const float* clearColor);
void endDraw();
void waitDrawDone();

HRESULT createBuffer(UINT sizeInBytes, ComPtr<ID3D12Resource>& buffer);
void createTextureBuffer(const char* filename, ComPtr<ID3D12Resource>& TextureBuf);

ComPtr<ID3D12Device>& device();
ComPtr<ID3D12GraphicsCommandList>& commandList();
float aspect();
