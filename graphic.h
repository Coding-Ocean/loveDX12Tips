#pragma once
#include<d3d12.h>
#include<DirectXMath.h>
#include<wrl.h>
#include<string>
using namespace DirectX;
using namespace Microsoft::WRL;

//システム系
void createGraphic(int numDescriptors);
void waitGPU();
void closeEventHandle();
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
void backgroundRect();
void beginRender();
void endRender();
void beginMsaaRender();
void endMsaaRender();

//2D functions ----------------------------------------------------------------
void rectModeCorner();
void rectModeCenter();
void fill(float r, float g, float b, float a = 1);
void noFill();
void stroke(float r, float g, float b, float a = 1);
void strokeWeight(float sw);
void noStroke();
extern struct COLOR Red,Green,Blue,Yellow;
void stroke(const COLOR&, float a = 1);
void fill(const COLOR&, float a = 1);
//image
void imageColor(float r, float g, float b, float a=1);
int textureWidth(int idx);
int textureHeight(int idx);
int loadImage(const char* filename);
int cutImage(int idx, int left, int top, int w, int h);
void divideImage(int srcImg, int col, int row, int w, int h, int* dstImgs);
void image(int textureIdx, float px, float py, float rad=0, float sx = 1, float sy = 1);
void image(const char* filename, float px, float py, float rad=0, float sx=1, float sy=1);
void cursor();
//shapes
void point(float px, float py);
void line(float sx, float sy, float ex, float ey);
void arrow(float sx, float sy, float ex, float ey, float len=20, float deg=20);
void arc(float ox, float oy, float ax, float ay, float bx, float by, float radius);
void rect(float px, float py, float w, float h, float rad = 0);
void circle(float px, float py, float diameter);
//font
constexpr ULONG JP = 128;
constexpr ULONG EN = 0;
void fontFace(const char* fontname, unsigned charset);
void fontSize(int size);
void fontColor(float r, float g, float b, float a=1);
void fontColor(const COLOR& c, float a=1);
void fontShadowColor(float r, float g, float b, float a);
void fontRectModeCorner();
void fontRectModeCenter();
float text(const char* str, float x, float y);
float text(float x, float y, const char* format, ...);
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
