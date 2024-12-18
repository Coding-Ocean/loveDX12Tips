#pragma once
#include<d3d12.h>
#include<DirectXMath.h>
#include<wrl.h>
#include<string>
using namespace DirectX;
using namespace Microsoft::WRL;
#define WINDOW true
#define NO_WINDOW false
#define width clientWidth()
#define height clientHeight()


//システム系
void window(LPCWSTR windowTitle, int clientWidth, int clientHeight, bool windowed = true, int numDescriptors = 2000, int clientPosX = -1, int clientPosY = -1);
bool quit();
void waitGPU();
void closeEventHandle();
//コンスタントバッファ、テクスチャバッファ用ディスクリプタヒープ
HRESULT createDescriptorHeap(UINT numDescriptors=2000);
//バッファ系
HRESULT createBuffer(UINT sizeInBytes, ComPtr<ID3D12Resource>& buffer);
HRESULT updateBuffer(void* data, UINT sizeInBytes, ComPtr<ID3D12Resource>& buffer);
HRESULT mapBuffer(ComPtr<ID3D12Resource>& buffer, void** mappedBuffer);
void unmapBuffer(ComPtr<ID3D12Resource>& buffer);
UINT alignedSize(size_t size);
//ディスクリプタ系
void createVertexBufferView(ComPtr<ID3D12Resource>& vertexBuffer, UINT sizeInBytes, UINT strideInBytes, D3D12_VERTEX_BUFFER_VIEW& vertexBufferView);
void createIndexBufferView(ComPtr<ID3D12Resource>& indexBuffer, UINT sizeInBytes, D3D12_INDEX_BUFFER_VIEW& indexBufferView);
UINT createConstantBufferView(ComPtr<ID3D12Resource>& constantBuffer);
UINT createTextureBufferView(ComPtr<ID3D12Resource>& textureBuffer);
//描画系
void clearColor(float r, float g, float b);
void beginRender();
void endRender();
float clientWidth();
float clientHeight();


//===
void rectModeCorner();
void rectModeCenter();
void fill(float r, float g, float b, float a = 1);
void stroke(float r, float g, float b, float a = 1);
void strokeWeight(float sw);
//image
int loadImage(const char* filename);
void image(int textureIdx, float px, float py, float rad=0, float sx = 1, float sy = 1);
void image(const char* filename, float px, float py, float rad=0, float sx=1, float sy=1);
//shapes
void point(float px, float py);
void line(float sx, float sy, float ex, float ey);
void rect(float px, float py, float w, float h, float rad = 0);
void circle(float px, float py, float diameter);
//font
constexpr ULONG JP = 128;
constexpr ULONG EN = 0;
void fontFace(const char* fontname, unsigned charset);
void fontSize(int size);
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

//#####debug#####
size_t numConstants();
size_t numLoadTextures();
size_t numFontTextures();
