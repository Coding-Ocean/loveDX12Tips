#pragma once
#include<d3d12.h>
#include<DirectXMath.h>
#include<wrl.h>
using namespace DirectX;
using namespace Microsoft::WRL;
#define WINDOW true
#define NO_WINDOW false

//コンスタントバッファ構造体
struct CONST_BUF0 {
	XMMATRIX worldViewProj;
};
struct CONST_BUF1 {
	XMFLOAT4 diffuse;
};

//システム系
void window(LPCWSTR windowTitle, int clientWidth, int clientHeight, bool windowed = true, int clientPosX = -1, int clientPosY = -1);
bool quit();
void setClearColor(float r, float g, float b);
void beginRender();
void endRender();
void waitGPU();
void closeEventHandle();
//リソースバッファ系
HRESULT createBuffer(UINT sizeInBytes, ComPtr<ID3D12Resource>& buffer);
HRESULT updateBuffer(void* data, UINT sizeInBytes, ComPtr<ID3D12Resource>& buffer);
HRESULT mapBuffer(ComPtr<ID3D12Resource>& buffer, void** mappedBuffer);
void unmapBuffer(ComPtr<ID3D12Resource>& buffer);
UINT alignedSize(size_t size);
HRESULT createTextureBuffer(const char* filename, ComPtr<ID3D12Resource>& TextureBuf);
//バーテックスバッファ系ディスクリプタ
void createVertexBufferView(ComPtr<ID3D12Resource>& vertexBuffer, UINT sizeInBytes, UINT strideInBytes, D3D12_VERTEX_BUFFER_VIEW& vertexBufferView);
void createIndexBufferView(ComPtr<ID3D12Resource>& indexBuffer, UINT sizeInBytes, D3D12_INDEX_BUFFER_VIEW& indexBufferView);
//コンスタント系ディスクリプタ
HRESULT createDescriptorHeap(UINT numDescriptors);//ヒープ
UINT createConstantBufferView(ComPtr<ID3D12Resource>& constantBuffer);
UINT createTextureBufferView(ComPtr<ID3D12Resource>& textureBuffer);
//描画
void drawMesh(D3D12_VERTEX_BUFFER_VIEW& vertexBufferView, D3D12_INDEX_BUFFER_VIEW& vndexBufferView, UINT cbvTbvIdx);
void drawMesh(D3D12_VERTEX_BUFFER_VIEW* vertexBufferView, UINT cbvTbvIdx);
//Get系
ComPtr<ID3D12Device>& device();
ComPtr<ID3D12GraphicsCommandList>& commandList();
UINT cbvTbvIncSize();
float aspect();
