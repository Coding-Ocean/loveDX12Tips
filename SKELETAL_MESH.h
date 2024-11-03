#pragma once
#include<vector>
#include"graphic.h"

//コンスタントバッファ構造体
struct CONST_BUF0 
{
	XMMATRIX viewProj;
	XMFLOAT4 lightPos;
	XMMATRIX boneWorlds[2];
};
struct CONST_BUF1 
{
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
};

//パーツメッシュ構造体
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
	CONST_BUF1* cb1 = nullptr;
	ComPtr<ID3D12Resource> constBuffer1 = nullptr;
	//テクスチャバッファ
	ComPtr<ID3D12Resource> textureBuffer = nullptr;
};

//ボーン構造体
struct BONE
{
	//計算後の最終的な行列
	XMMATRIX world;
	//親からの相対姿勢行列
	XMMATRIX bindWorld;
	//アニメーションデータ。キーフレーム行列
	std::vector<XMMATRIX> keyframeWorlds;
	XMMATRIX currentFrameWorld;
	//　この値を使って、子供インデックス配列をつくる
	int parentIdx;
	//　子供インデックス配列
	std::vector<int> childIdxs;
};

//Skeletal Mesh
class SKELETAL_MESH
{
private:
	std::vector<PARTS> Parts;
	std::vector<BONE> Bones;

	UINT FrameCount = 0;
	UINT Interval;//キーフレームの間隔

	CONST_BUF0* Cb0 = nullptr;
	ComPtr<ID3D12Resource> ConstBuffer0 = nullptr;
	ComPtr<ID3D12DescriptorHeap> CbvTbvHeap = nullptr;

	//システム系
	HRESULT Hr = E_FAIL;
	UINT CbvTbvIncSize = cbvTbvIncSize();
	ComPtr<ID3D12GraphicsCommandList>& CommandList = commandList();
public:
	SKELETAL_MESH();
	~SKELETAL_MESH();
	void create();
	void update(XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj, XMFLOAT4& light);
	void draw();
private:
	//update()の中から呼び出される２つの関数
	XMMATRIX LerpMatrix(XMMATRIX& a, XMMATRIX& b, float t);
	void UpdateWorld(BONE& b, const XMMATRIX& m);
};

