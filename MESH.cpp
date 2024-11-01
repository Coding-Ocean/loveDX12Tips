#include "MESH.h"
#include "model.h"

MESH::MESH()
{
}

MESH::~MESH()
{
	Parts.ConstBuffer1->Unmap(0, nullptr);
	Parts.ConstBuffer0->Unmap(0, nullptr);
}

void MESH::create()
{
	//頂点バッファ
	{
		//データサイズを求めておく
		UINT sizeInBytes = sizeof(Vertices);
		UINT strideInBytes = sizeof(float) * NumVertexElements;
		Parts.NumVertices = sizeInBytes / strideInBytes;
		//バッファをつくる
		Hr = createBuffer(sizeInBytes, Parts.VertexBuffer);
		assert(SUCCEEDED(Hr));
		//バッファにデータを入れる
		Hr = updateBuffer(Vertices, sizeInBytes, Parts.VertexBuffer);
		assert(SUCCEEDED(Hr));
		//バッファビューをつくる
		createVertexBufferView(Parts.VertexBuffer, sizeInBytes, strideInBytes, Parts.VertexBufferView);
	}
	//頂点インデックスバッファ
	{
		//データサイズを求めておく
		UINT sizeInBytes = sizeof(Indices);
		Parts.NumIndices = sizeInBytes / sizeof(UINT16);
		//バッファをつくる
		Hr = createBuffer(sizeInBytes, Parts.IndexBuffer);
		assert(SUCCEEDED(Hr));
		//バッファにデータを入れる
		Hr = updateBuffer(Indices, sizeInBytes, Parts.IndexBuffer);
		assert(SUCCEEDED(Hr));
		//インデックスバッファビューをつくる
		createIndexBufferView(Parts.IndexBuffer, sizeInBytes, Parts.IndexBufferView);
	}
	//コンスタントバッファ０
	{
		//バッファをつくる
		Hr = createBuffer(256, Parts.ConstBuffer0);
		assert(SUCCEEDED(Hr));
		//マップしておく
		Hr = mapBuffer(Parts.ConstBuffer0, (void**)&Parts.CB0);
		assert(SUCCEEDED(Hr));
	}
	//コンスタントバッファ１
	{
		//バッファをつくる
		Hr = createBuffer(256, Parts.ConstBuffer1);
		assert(SUCCEEDED(Hr));
		//マップしておく
		Hr = mapBuffer(Parts.ConstBuffer1, (void**)&Parts.CB1);
		assert(SUCCEEDED(Hr));
		//データを入れる
		Parts.CB1->ambient = { Ambient[0],Ambient[1],Ambient[2],Ambient[3] };
		Parts.CB1->diffuse = { Diffuse[0],Diffuse[1],Diffuse[2],Diffuse[3] };
	}
	//テクスチャバッファ
	{
		Hr = createTextureBuffer(TextureFilename, Parts.TextureBuffer);
		assert(SUCCEEDED(Hr));
	}
	//ディスクリプタヒープ
	{
		//ディスクリプタ(ビュー)３つ分のヒープをつくる
		Hr = createDescriptorHeap(3, Parts.CbvTbvHeap);
		assert(SUCCEEDED(Hr));
		CbvTbvIncSize = cbvTbvIncSize();
		//１つめのディスクリプタ(ビュー)をヒープにつくる
		auto hCbvTbvHeap = Parts.CbvTbvHeap->GetCPUDescriptorHandleForHeapStart();
		createConstantBufferView(Parts.ConstBuffer0, hCbvTbvHeap);
		//２つめのディスクリプタ(ビュー)をヒープにつくる
		hCbvTbvHeap.ptr += CbvTbvIncSize;
		createConstantBufferView(Parts.ConstBuffer1, hCbvTbvHeap);
		//３つめのディスクリプタ(ビュー)をヒープにつくる
		hCbvTbvHeap.ptr += CbvTbvIncSize;
		createTextureBufferView(Parts.TextureBuffer, hCbvTbvHeap);
	}
}

void MESH::update(XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj)
{
	Parts.CB0->worldViewProj = world * view * proj;
	Parts.CB0->world = world;
	Parts.CB0->lightPos = { 0,1,0,0 };
}

void MESH::draw()
{
	//頂点をセット
	CommandList->IASetVertexBuffers(0, 1, &Parts.VertexBufferView);
	CommandList->IASetIndexBuffer(&Parts.IndexBufferView);
	//ディスクリプタヒープをＧＰＵにセット
	CommandList->SetDescriptorHeaps(1, Parts.CbvTbvHeap.GetAddressOf());
	//ディスクリプタヒープをディスクリプタテーブルにセット
	auto hCbvTbvHeap = Parts.CbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
	CommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);
	//描画
	CommandList->DrawIndexedInstanced(Parts.NumIndices, 1, 0, 0, 0);
}
