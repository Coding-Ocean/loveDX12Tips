#pragma once
#include<vector>
#define _XM_NO_INTRINSICS_
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

	//アニメーションデータ。キーフレーム行列
	std::vector<XMMATRIX> keyframeWorlds;
	XMMATRIX currentFrameWorld;

	//階層行列データ
	//　姿勢行列
	XMMATRIX bindWorld;
	//　これがこのパーツのワールド行列になる
	XMMATRIX finalWorld;
	//　このパーツの子供インデックス
	int parentIdx;
	std::vector<int> childIdxs;
};

class MESH
{
private:
	std::vector<PARTS> Parts;
	UINT CbvTbvIncSize = 0;
	HRESULT Hr;
	ComPtr<ID3D12GraphicsCommandList>& CommandList=commandList();
public:
	MESH();
	~MESH();
	void create();
	void update(int frameCount, int interval, XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj, XMFLOAT4& light);
	void draw();
private:
	//update()の中で呼び出される２つの関数
	XMMATRIX LerpMatrix(XMMATRIX& a, XMMATRIX& b, float t);
	void UpdateFinalWorld(PARTS& p, XMMATRIX& m);
};

