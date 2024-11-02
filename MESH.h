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
	UINT numVertices = 0;
	ComPtr<ID3D12Resource>   vertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
	//頂点インデックスバッファ
	UINT numIndices = 0;
	ComPtr<ID3D12Resource>  indexBuffer = nullptr;
	D3D12_INDEX_BUFFER_VIEW	indexBufferView = {};
	//コンスタントバッファ
	CONST_BUF0* cb0 = nullptr;
	CONST_BUF1* cb1 = nullptr;
	ComPtr<ID3D12Resource> constBuffer0 = nullptr;
	ComPtr<ID3D12Resource> constBuffer1 = nullptr;
	//テクスチャバッファ
	ComPtr<ID3D12Resource> textureBuffer = nullptr;
	//ディスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> cbvTbvHeap = nullptr;
};

class MESH
{
private:
	PARTS Parts;//今回はPartsひとつ

	//システム系
	HRESULT Hr = E_FAIL;
	UINT CbvTbvIncSize = cbvTbvIncSize();
	ComPtr<ID3D12GraphicsCommandList>& CommandList = commandList();
public:
	MESH();
	~MESH();
	void create();
	void update(XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj, XMFLOAT4& lightPos);
	void draw();
};

