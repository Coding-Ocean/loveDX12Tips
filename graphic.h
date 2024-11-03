#pragma once
#include<cmath>
#include<d3d12.h>
#define _XM_NO_INTRINSICS_
#include<DirectXMath.h>
#include<wrl.h>
using namespace DirectX;
using namespace Microsoft::WRL;
#define WINDOW true
#define NO_WINDOW false

//システム系
void window(LPCWSTR windowTitle, int clientWidth, int clientHeight, bool windowed = true, int clientPosX = -1, int clientPosY = -1);
bool quit();
void clear(const float* clearColor);
void present();
void waitGPU();
void closeEventHandle();
//リソースバッファ系
HRESULT createBuffer(UINT sizeInBytes, ComPtr<ID3D12Resource>& buffer);
HRESULT updateBuffer(void* data, UINT sizeInBytes, ComPtr<ID3D12Resource>& buffer);
HRESULT mapBuffer(ComPtr<ID3D12Resource>& buffer, void** mappedBuffer);
HRESULT createTextureBuffer(const char* filename, ComPtr<ID3D12Resource>& TextureBuf);
void createVertexBufferView(ComPtr<ID3D12Resource>& vertexBuffer, UINT sizeInBytes, UINT strideInBytes, D3D12_VERTEX_BUFFER_VIEW& vertexBufferView);
void createIndexBufferView(ComPtr<ID3D12Resource>& indexBuffer, UINT sizeInBytes, D3D12_INDEX_BUFFER_VIEW& indexBufferView);
//ディスクリプタヒープ系
HRESULT createDescriptorHeap(UINT numDescriptors, ComPtr<ID3D12DescriptorHeap>& cbvTbvHeap);
void createConstantBufferView(ComPtr<ID3D12Resource>& constantBuffer, D3D12_CPU_DESCRIPTOR_HANDLE& hCbvTbvHeap);
void createTextureBufferView(ComPtr<ID3D12Resource>& textureBuffer, D3D12_CPU_DESCRIPTOR_HANDLE& hCbvTbvHeap);
//Get系
ComPtr<ID3D12Device>& device();
ComPtr<ID3D12GraphicsCommandList>& commandList();
UINT cbvTbvIncSize();
float aspect();
