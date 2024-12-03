#pragma once
#include<vector>
#include"graphic.h"

//メッシュパーツ構造体
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
	UINT cbvTbvIdx=0;
};

//ボーン構造体
struct BONE
{
	//計算後の最終的な行列。これをシェーダに送る
	XMMATRIX world;
	//親からの相対姿勢行列
	XMMATRIX bindWorld;
	//アニメーションデータ。キーフレーム行列
	std::vector<XMMATRIX> keyframeWorlds;
	XMMATRIX currentFrameWorld;
	//この値を使って、子供インデックス配列をつくる
	int parentIdx;
	//子供インデックス配列
	std::vector<int> childIdxs;
};

//Skeletal Mesh
class SKELETAL_MESH
{
private:
	std::vector<MESH> Meshes;
	std::vector<BONE> Bones;
	UINT FrameCount = 0;
	UINT Interval;//キーフレームの間隔
public:
	SKELETAL_MESH();
	~SKELETAL_MESH();
	void create();
	void update(XMMATRIX& world, XMMATRIX& viewProj, XMFLOAT4& light);
	void draw();
private:
	//update()の中から呼び出される２つの関数
	void UpdateWorlds(BONE& bone, const XMMATRIX& parentWorld);
	XMMATRIX LerpMatrix(XMMATRIX& a, XMMATRIX& b, float t);
};

