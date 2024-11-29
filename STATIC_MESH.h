#pragma once
#include"graphic.h"

//メッシュ
struct MESH 
{
	//頂点バッファ
	ComPtr<ID3D12Resource>   vertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbv = {};
	//頂点インデックスバッファ
	ComPtr<ID3D12Resource>  indexBuffer = nullptr;
	D3D12_INDEX_BUFFER_VIEW	ibv = {};
	//コンスタントバッファ0
	ComPtr<ID3D12Resource> constBuffer0 = nullptr;
	CONST_BUF0* cb0 = nullptr;
	//コンスタントバッファ1
	ComPtr<ID3D12Resource> constBuffer1 = nullptr;
	CONST_BUF1* cb1 = nullptr;
	//テクスチャバッファ
	ComPtr<ID3D12Resource> textureBuffer = nullptr;
	//ディスクリプタインデックス
	UINT cbvTbvIdx = 0;
};

//形が変わらないメッシュ
class STATIC_MESH
{
private:
	MESH Mesh;//今回はMeshひとつ
public:
	STATIC_MESH();
	~STATIC_MESH();
	void create();
	void update(XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj, XMFLOAT4& lightPos);
	void draw();
};

