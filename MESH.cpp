#include "MESH.h"
#include "model.h"

MESH::MESH()
{
}

MESH::~MESH()
{
	Parts.constBuffer1->Unmap(0, nullptr);
	Parts.constBuffer0->Unmap(0, nullptr);
}

void MESH::create()
{
	//頂点バッファ
	{
		//データサイズを求めておく
		UINT sizeInBytes = sizeof(Vertices);
		UINT strideInBytes = sizeof(float) * NumVertexElements;
		Parts.numVertices = sizeInBytes / strideInBytes;
		//バッファをつくる
		Hr = createBuffer(sizeInBytes, Parts.vertexBuffer);
		assert(SUCCEEDED(Hr));
		//バッファにデータを入れる
		Hr = updateBuffer(Vertices, sizeInBytes, Parts.vertexBuffer);
		assert(SUCCEEDED(Hr));
		//バッファビューをつくる
		createVertexBufferView(Parts.vertexBuffer, sizeInBytes, strideInBytes, Parts.vertexBufferView);
	}
	//頂点インデックスバッファ
	{
		//データサイズを求めておく
		UINT sizeInBytes = sizeof(Indices);
		Parts.numIndices = sizeInBytes / sizeof(UINT16);
		//バッファをつくる
		Hr = createBuffer(sizeInBytes, Parts.indexBuffer);
		assert(SUCCEEDED(Hr));
		//バッファにデータを入れる
		Hr = updateBuffer(Indices, sizeInBytes, Parts.indexBuffer);
		assert(SUCCEEDED(Hr));
		//インデックスバッファビューをつくる
		createIndexBufferView(Parts.indexBuffer, sizeInBytes, Parts.indexBufferView);
	}
	//コンスタントバッファ０
	{
		//バッファをつくる
		Hr = createBuffer(256, Parts.constBuffer0);
		assert(SUCCEEDED(Hr));
		//マップしておく
		Hr = mapBuffer(Parts.constBuffer0, (void**)&Parts.cb0);
		assert(SUCCEEDED(Hr));
	}
	//コンスタントバッファ１
	{
		//バッファをつくる
		Hr = createBuffer(256, Parts.constBuffer1);
		assert(SUCCEEDED(Hr));
		//マップしておく
		Hr = mapBuffer(Parts.constBuffer1, (void**)&Parts.cb1);
		assert(SUCCEEDED(Hr));
		//データを入れる
		Parts.cb1->ambient = { Ambient[0],Ambient[1],Ambient[2],Ambient[3] };
		Parts.cb1->diffuse = { Diffuse[0],Diffuse[1],Diffuse[2],Diffuse[3] };
	}
	//テクスチャバッファ
	{
		Hr = createTextureBuffer(TextureFilename, Parts.textureBuffer);
		assert(SUCCEEDED(Hr));
	}
	//ディスクリプタヒープ
	{
		//ディスクリプタ(ビュー)３つ分のヒープをつくる
		Hr = createDescriptorHeap(3, Parts.cbvTbvHeap);
		assert(SUCCEEDED(Hr));
		CbvTbvIncSize = cbvTbvIncSize();
		//１つめのディスクリプタ(ビュー)をヒープにつくる
		auto hCbvTbvHeap = Parts.cbvTbvHeap->GetCPUDescriptorHandleForHeapStart();
		createConstantBufferView(Parts.constBuffer0, hCbvTbvHeap);
		//２つめのディスクリプタ(ビュー)をヒープにつくる
		hCbvTbvHeap.ptr += CbvTbvIncSize;
		createConstantBufferView(Parts.constBuffer1, hCbvTbvHeap);
		//３つめのディスクリプタ(ビュー)をヒープにつくる
		hCbvTbvHeap.ptr += CbvTbvIncSize;
		createTextureBufferView(Parts.textureBuffer, hCbvTbvHeap);
	}
}

void MESH::update(XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj, XMFLOAT4& lightPos)
{
	Parts.cb0->worldViewProj = world * view * proj;
	Parts.cb0->world = world;
	Parts.cb0->lightPos = lightPos;
}

void MESH::draw()
{
	//頂点をセット
	CommandList->IASetVertexBuffers(0, 1, &Parts.vertexBufferView);
	CommandList->IASetIndexBuffer(&Parts.indexBufferView);
	//ディスクリプタヒープをＧＰＵにセット
	CommandList->SetDescriptorHeaps(1, Parts.cbvTbvHeap.GetAddressOf());
	//ディスクリプタヒープをディスクリプタテーブルにセット
	auto hCbvTbvHeap = Parts.cbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
	CommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);
	//描画
	CommandList->DrawIndexedInstanced(Parts.numIndices, 1, 0, 0, 0);
}
