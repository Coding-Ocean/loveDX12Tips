#pragma once
#include<vector>
#include"graphic.h"

//コンスタントバッファ構造体
struct CONST_BUF0 
{
	XMFLOAT4 lightPos;
	XMMATRIX ViewProj;
	XMMATRIX world;
};
struct CONST_BUF1 
{
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
};

//メッシュパーツ
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
	//コンスタントバッファ
	CONST_BUF0* cb0 = nullptr;
	CONST_BUF1* cb1 = nullptr;
	ComPtr<ID3D12Resource> constBuffer0 = nullptr;
	ComPtr<ID3D12Resource> constBuffer1 = nullptr;
	//テクスチャバッファ
	ComPtr<ID3D12Resource> textureBuffer = nullptr;
	//ディスクリプタヒープ(わかりやすさ優先で複数用意してしまいます)
	ComPtr<ID3D12DescriptorHeap> cbvTbvHeap = nullptr;

	//階層行列データ
	//　コンスタントバッファに渡す所謂ワールド行列。行列計算によって最終的に求める。
	XMMATRIX world;
	//  親から見た相対姿勢行列
	XMMATRIX bindWorld;
	//　アニメーションデータ。キーフレーム行列
	std::vector<XMMATRIX> keyframeWorlds;
	XMMATRIX currentFrameWorld;
	//　この値を使って、子供インデックス配列をつくる
	int parentIdx;
	//　子供インデックス配列
	std::vector<int> childIdxs;
};

//階層メッシュ
class HIERARCHY_MESH
{
private:
	std::vector<MESH> Meshes;
	UINT FrameCount = 0;
	UINT Interval;//キーフレームの間隔

	//システム系
	HRESULT Hr = E_FAIL;
	UINT CbvTbvIncSize = cbvTbvIncSize();
	ComPtr<ID3D12GraphicsCommandList>& CommandList = commandList();
public:
	HIERARCHY_MESH();
	~HIERARCHY_MESH();
	void create();
	void update(XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj, XMFLOAT4& light);
	void draw();
private:
	void UpdateWorlds(MESH& mesh, XMMATRIX& parentWorld);
	XMMATRIX LerpMatrix(XMMATRIX& a, XMMATRIX& b, float t);
};

