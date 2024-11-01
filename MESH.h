#pragma once
#include"graphic.h"

//コンスタントバッファ構造体
struct CONST_BUF0 
{
	XMMATRIX worldViewProj;
	XMMATRIX world;
	XMFLOAT4 lightPos;
};
struct CONST_BUF1 
{
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
};

//パーツメッシュ
struct PARTS 
{
	//頂点バッファ
	UINT NumVertices = 0;
	ComPtr<ID3D12Resource>   VertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView = {};
	//頂点インデックスバッファ
	UINT NumIndices = 0;
	ComPtr<ID3D12Resource>  IndexBuffer = nullptr;
	D3D12_INDEX_BUFFER_VIEW	IndexBufferView = {};
	//コンスタントバッファ
	CONST_BUF0* CB0 = nullptr;
	CONST_BUF1* CB1 = nullptr;
	ComPtr<ID3D12Resource> ConstBuffer0 = nullptr;
	ComPtr<ID3D12Resource> ConstBuffer1 = nullptr;
	//テクスチャバッファ
	ComPtr<ID3D12Resource> TextureBuffer = nullptr;
	//ディスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> CbvTbvHeap = nullptr;
};

class MESH
{
private:
	PARTS Parts;//今回はPartsひとつ
	UINT CbvTbvIncSize = 0;
	HRESULT Hr;
	ComPtr<ID3D12GraphicsCommandList>& CommandList=commandList();
public:
	MESH();
	~MESH();
	void create();
	void update(XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj);
	void draw();
};

