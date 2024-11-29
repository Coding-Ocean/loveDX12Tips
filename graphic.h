#pragma once
#include<d3d12.h>
#include<DirectXMath.h>
#include<wrl.h>
using namespace DirectX;
using namespace Microsoft::WRL;

#define WINDOW true
#define NO_WINDOW false

//コンスタントバッファ構造体。Header.hlsliと同じ並びにしておく
struct CONST_BUF0 {
	XMFLOAT4 lightPos;
	XMFLOAT4 eyePos;
	XMMATRIX worldViewProj;
	XMMATRIX world;
};
struct CONST_BUF1 {
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
};

//システム系
void window(LPCWSTR windowTitle, int clientWidth, int clientHeight, bool windowed = true, int clientPosX = -1, int clientPosY = -1);
bool quit();
void waitGPU();
void closeEventHandle();
//ディスクリプタヒープ
HRESULT createDescriptorHeap(UINT numDescriptors);
//リソースバッファ系
HRESULT createBuffer(UINT sizeInBytes, ComPtr<ID3D12Resource>& buffer);
UINT alignedSize(size_t size);
HRESULT updateBuffer(void* data, UINT sizeInBytes, ComPtr<ID3D12Resource>& buffer);
HRESULT mapBuffer(ComPtr<ID3D12Resource>& buffer, void** mappedBuffer);
void unmapBuffer(ComPtr<ID3D12Resource>& buffer);
HRESULT createTextureBuffer(const char* filename, ComPtr<ID3D12Resource>& TextureBuf);
//リソースビュー系
void createVertexBufferView(ComPtr<ID3D12Resource>& vertexBuffer, UINT sizeInBytes, UINT strideInBytes, D3D12_VERTEX_BUFFER_VIEW& vertexBufferView);
void createIndexBufferView(ComPtr<ID3D12Resource>& indexBuffer, UINT sizeInBytes, D3D12_INDEX_BUFFER_VIEW& indexBufferView);
UINT createConstantBufferView(ComPtr<ID3D12Resource>& constantBuffer);
UINT createTextureBufferView(ComPtr<ID3D12Resource>& textureBuffer);
//描画系
void setClearColor(float r, float g, float b);
void beginRender();
void drawMesh(D3D12_VERTEX_BUFFER_VIEW& vertexBufferView, D3D12_INDEX_BUFFER_VIEW& indexBufferView, UINT cbvTbvIdx);
void endRender();
//Get系
ComPtr<ID3D12Device>& device();
ComPtr<ID3D12GraphicsCommandList>& commandList();
UINT cbvTbvIncSize();
float aspect();
