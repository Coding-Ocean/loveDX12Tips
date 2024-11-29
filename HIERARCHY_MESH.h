#pragma once
#include<vector>
#include"graphic.h"

//メッシュパーツ
struct MESH 
{
	//頂点バッファ
	ComPtr<ID3D12Resource>   vertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbv = {};
	//頂点インデックスバッファ
	ComPtr<ID3D12Resource>  indexBuffer = nullptr;
	D3D12_INDEX_BUFFER_VIEW	ibv = {};
	//コンスタントバッファ０
	ComPtr<ID3D12Resource> constBuffer0 = nullptr;
	CONST_BUF0* cb0 = nullptr;
	//コンスタントバッファ１
	ComPtr<ID3D12Resource> constBuffer1 = nullptr;
	CONST_BUF1* cb1 = nullptr;
	//テクスチャバッファ
	ComPtr<ID3D12Resource> textureBuffer = nullptr;
	//ディスクリプタインデックス
	UINT cbvTbvIdx;

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

