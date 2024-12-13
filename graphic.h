#pragma once
#include<d3d12.h>
#include<DirectXMath.h>
#include<wrl.h>
#include<string>
using namespace DirectX;
using namespace Microsoft::WRL;
#define WINDOW true
#define NO_WINDOW false

//===
//�}�b�v�p�R���X�^���g�o�b�t�@�\����
struct CONST_BUF0 {
	XMMATRIX worldViewProj;
	XMFLOAT4 diffuse;
};

//�V�X�e���n
void window(LPCWSTR windowTitle, int clientWidth, int clientHeight, bool windowed = true, int clientPosX = -1, int clientPosY = -1);
bool quit();
void waitGPU();
void closeEventHandle();
//�R���X�^���g�o�b�t�@�A�e�N�X�`���o�b�t�@�p�f�B�X�N���v�^�q�[�v
HRESULT createDescriptorHeap(UINT numDescriptors);
//�o�b�t�@�n
HRESULT createBuffer(UINT sizeInBytes, ComPtr<ID3D12Resource>& buffer);
HRESULT updateBuffer(void* data, UINT sizeInBytes, ComPtr<ID3D12Resource>& buffer);
HRESULT mapBuffer(ComPtr<ID3D12Resource>& buffer, void** mappedBuffer);
void unmapBuffer(ComPtr<ID3D12Resource>& buffer);
UINT alignedSize(size_t size);
HRESULT createTextureBuffer(const char* filename, ComPtr<ID3D12Resource>& TextureBuf,int* w=nullptr, int* h=nullptr);
//�f�B�X�N���v�^�n
void createVertexBufferView(ComPtr<ID3D12Resource>& vertexBuffer, UINT sizeInBytes, UINT strideInBytes, D3D12_VERTEX_BUFFER_VIEW& vertexBufferView);
void createIndexBufferView(ComPtr<ID3D12Resource>& indexBuffer, UINT sizeInBytes, D3D12_INDEX_BUFFER_VIEW& indexBufferView);
UINT createConstantBufferView(ComPtr<ID3D12Resource>& constantBuffer);
UINT createTextureBufferView(ComPtr<ID3D12Resource>& textureBuffer);
UINT createTextureBufferAndView(const char* filename, ComPtr<ID3D12Resource>& TextureBuf, int* w = nullptr, int* h = nullptr);
//�`��n
void setClearColor(float r, float g, float b);
void beginRender();
void drawImage(UINT cbvIdx, UINT tbvIdx);
//void drawMesh(D3D12_VERTEX_BUFFER_VIEW& vertexBufferView, D3D12_INDEX_BUFFER_VIEW& vndexBufferView, UINT cbvTbvIdx);
//void drawMesh(D3D12_VERTEX_BUFFER_VIEW& vertexBufferView, UINT cbvTbvIdx);
void endRender();
//Get�n
ComPtr<ID3D12Device>& device();
ComPtr<ID3D12GraphicsCommandList>& commandList();
UINT cbvTbvIncSize();
float clientWidth();
float clientHeight();
float aspect();

//===
constexpr ULONG JP = 128;
constexpr ULONG EN = 0;
void fontFace(const char* fontname, unsigned charset);
void fontSize(int size);
void fontColor(float r, float g, float b, float a=1);
float text(const char* str, float x, float y);

void setPrintInitX(float initX);
void setPrintInitY(float initY);
void print(const char* format, ...);

class USER_FONT
{
private:
	std::string Filename;
public:
	USER_FONT(const char* filename);
	~USER_FONT();
};
