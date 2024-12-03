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
//�R���X�^���g�o�b�t�@�\����
struct CONST_BUF0
{
	XMFLOAT4 lightPos;
	XMMATRIX viewProj;
	XMMATRIX boneWorlds[2];
};
struct CONST_BUF1
{
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
};
//�V�X�e���n
void window(LPCWSTR windowTitle, int clientWidth, int clientHeight, bool windowed = true, int clientPosX = -1, int clientPosY = -1);
bool quit();
void waitGPU();
void closeEventHandle();
//�R���X�^���g����уe�N�X�`���p�f�B�X�N���v�^�q�[�v
HRESULT createDescriptorHeap(UINT numDescriptors);
//�o�b�t�@�n
HRESULT createBuffer(UINT sizeInBytes, ComPtr<ID3D12Resource>& buffer);
UINT alignedSize(size_t size);
HRESULT updateBuffer(void* data, UINT sizeInBytes, ComPtr<ID3D12Resource>& buffer);
HRESULT mapBuffer(ComPtr<ID3D12Resource>& buffer, void** mappedBuffer);
void unmapBuffer(ComPtr<ID3D12Resource>& buffer);
HRESULT createTextureBuffer(const char* filename, ComPtr<ID3D12Resource>& TextureBuffer);
//�f�B�X�N���v�^�n
void createVertexBufferView(ComPtr<ID3D12Resource>& vertexBuffer, UINT sizeInBytes, UINT strideInBytes, D3D12_VERTEX_BUFFER_VIEW& vertexBufferView);
void createIndexBufferView(ComPtr<ID3D12Resource>& indexBuffer, UINT sizeInBytes, D3D12_INDEX_BUFFER_VIEW& indexBufferView);
UINT createConstantBufferView(ComPtr<ID3D12Resource>& constantBuffer);
UINT createTextureBufferView(ComPtr<ID3D12Resource>& textureBuffer);
//�`��
void setClearColor(float r, float g, float b);
void beginRender();
void drawMesh(D3D12_VERTEX_BUFFER_VIEW& vbv, D3D12_INDEX_BUFFER_VIEW& ibv, UINT cbvTbvIdx);
void endRender();
//Get�n
ComPtr<ID3D12Device>& device();
ComPtr<ID3D12GraphicsCommandList>& commandList();
UINT cbvTbvIncSize();
float aspect();
