#pragma once
#include"graphic.h"

//コンスタントバッファ構造体
struct CONST_BUF0 
{
	XMFLOAT4 lightPos;
	XMMATRIX viewProj;
	XMMATRIX world;
};
struct CONST_BUF1 
{
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
};

//メッシュ
struct MESH 
{
	//頂点バッファ
	UINT numVertices = 0;
	ComPtr<ID3D12Resource>   vertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
	//頂点インデックスバッファ
	UINT numIndices = 0;
	ComPtr<ID3D12Resource>  indexBuffer = nullptr;
	D3D12_INDEX_BUFFER_VIEW	indexBufferView = {};
	//コンスタントバッファ0
	ComPtr<ID3D12Resource> constBuffer0 = nullptr;
	CONST_BUF0* cb0 = nullptr;
	//コンスタントバッファ1
	ComPtr<ID3D12Resource> constBuffer1 = nullptr;
	CONST_BUF1* cb1 = nullptr;
	//テクスチャバッファ
	ComPtr<ID3D12Resource> textureBuffer = nullptr;
	//ディスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> cbvTbvHeap = nullptr;
};

//形が変わらないメッシュ
class STATIC_MESH
{
private:
	MESH Mesh;//今回はMeshひとつ

	//システム系
	HRESULT Hr = E_FAIL;
	UINT CbvTbvIncSize = cbvTbvIncSize();
	ComPtr<ID3D12GraphicsCommandList>& CommandList = commandList();
public:
	STATIC_MESH();
	~STATIC_MESH();
	void create();
	void update(XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj, XMFLOAT4& lightPos);
	void draw();
};

