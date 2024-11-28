#pragma once
#include<d3d12.h>
#include<DirectXMath.h>
#include<wrl.h>
using namespace DirectX;
using namespace Microsoft::WRL;
#define WINDOW true
#define NO_WINDOW false

//�V�X�e���n
void window(LPCWSTR windowTitle, int clientWidth, int clientHeight, bool windowed = true, int clientPosX = -1, int clientPosY = -1);
bool quit();
void setClearColor(float r, float g, float b);
void beginRender();
void endRender();
void waitGPU();
void closeEventHandle();
//���\�[�X�o�b�t�@�n
HRESULT createBuffer(UINT sizeInBytes, ComPtr<ID3D12Resource>& buffer);
HRESULT updateBuffer(void* data, UINT sizeInBytes, ComPtr<ID3D12Resource>& buffer);
HRESULT mapBuffer(ComPtr<ID3D12Resource>& buffer, void** mappedBuffer);
HRESULT createTextureBuffer(const char* filename, ComPtr<ID3D12Resource>& TextureBuf);
//�o�[�e�b�N�X�o�b�t�@�E�C���f�b�N�X�o�b�t�@�@�f�B�X�N���v�^
void createVertexBufferView(ComPtr<ID3D12Resource>& vertexBuffer, UINT sizeInBytes, UINT strideInBytes, D3D12_VERTEX_BUFFER_VIEW& vertexBufferView);
void createIndexBufferView(ComPtr<ID3D12Resource>& indexBuffer, UINT sizeInBytes, D3D12_INDEX_BUFFER_VIEW& indexBufferView);
//�R���X�^���g�o�b�t�@�E�e�N�X�`���o�b�t�@�@�f�B�X�N���v�^
HRESULT createDescriptorHeap(UINT numDescriptors);
UINT createConstantBufferView(ComPtr<ID3D12Resource>& constantBuffer);
UINT createTextureBufferView(ComPtr<ID3D12Resource>& textureBuffer);
/*
HRESULT createDescriptorHeap(UINT numDescriptors, ComPtr<ID3D12DescriptorHeap>& cbvTbvHeap);
void createConstantBufferView(ComPtr<ID3D12Resource>& constantBuffer, D3D12_CPU_DESCRIPTOR_HANDLE& hCbvTbvHeap);
void createTextureBufferView(ComPtr<ID3D12Resource>& textureBuffer, D3D12_CPU_DESCRIPTOR_HANDLE& hCbvTbvHeap);
*/
void drawMesh(D3D12_VERTEX_BUFFER_VIEW& vbv, D3D12_INDEX_BUFFER_VIEW& ibv, UINT cbvTbvIdx);
void drawMesh(D3D12_VERTEX_BUFFER_VIEW& vbv, D3D12_INDEX_BUFFER_VIEW& ibv, UINT cbvIdx, UINT tbvIdx);
//Get�n
ComPtr<ID3D12Device>& device();
ComPtr<ID3D12GraphicsCommandList>& commandList();
UINT cbvTbvIncSize();
float aspect();
