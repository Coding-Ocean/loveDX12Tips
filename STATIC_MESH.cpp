#include "STATIC_MESH.h"
#include "model.h"

STATIC_MESH::STATIC_MESH()
{
}

STATIC_MESH::~STATIC_MESH()
{
	Mesh.constBuffer1->Unmap(0, nullptr);
	Mesh.constBuffer0->Unmap(0, nullptr);
}

void STATIC_MESH::create()
{
	//頂点バッファ
	{
		//データサイズを求めておく
		UINT sizeInBytes = sizeof(Vertices);
		UINT strideInBytes = sizeof(float) * NumVertexElements;
		Mesh.numVertices = sizeInBytes / strideInBytes;
		//バッファをつくる
		Hr = createBuffer(sizeInBytes, Mesh.vertexBuffer);
		assert(SUCCEEDED(Hr));
		//バッファにデータを入れる
		Hr = updateBuffer(Vertices, sizeInBytes, Mesh.vertexBuffer);
		assert(SUCCEEDED(Hr));
		//バッファビューをつくる
		createVertexBufferView(Mesh.vertexBuffer, sizeInBytes, strideInBytes, Mesh.vertexBufferView);
	}
	//頂点インデックスバッファ
	{
		//データサイズを求めておく
		UINT sizeInBytes = sizeof(Indices);
		Mesh.numIndices = sizeInBytes / sizeof(UINT16);
		//バッファをつくる
		Hr = createBuffer(sizeInBytes, Mesh.indexBuffer);
		assert(SUCCEEDED(Hr));
		//バッファにデータを入れる
		Hr = updateBuffer(Indices, sizeInBytes, Mesh.indexBuffer);
		assert(SUCCEEDED(Hr));
		//インデックスバッファビューをつくる
		createIndexBufferView(Mesh.indexBuffer, sizeInBytes, Mesh.indexBufferView);
	}
	//コンスタントバッファ０
	{
		//バッファをつくる
		Hr = createBuffer(256, Mesh.constBuffer0);
		assert(SUCCEEDED(Hr));
		//マップしておく
		Hr = mapBuffer(Mesh.constBuffer0, (void**)&Mesh.cb0);
		assert(SUCCEEDED(Hr));
	}
	//コンスタントバッファ１
	{
		//バッファをつくる
		Hr = createBuffer(256, Mesh.constBuffer1);
		assert(SUCCEEDED(Hr));
		//マップしておく
		Hr = mapBuffer(Mesh.constBuffer1, (void**)&Mesh.cb1);
		assert(SUCCEEDED(Hr));
		//データを入れる
		Mesh.cb1->ambient = { Ambient[0],Ambient[1],Ambient[2],Ambient[3] };
		Mesh.cb1->diffuse = { Diffuse[0],Diffuse[1],Diffuse[2],Diffuse[3] };
	}
	//テクスチャバッファ
	{
		Hr = createTextureBuffer(TextureFilename, Mesh.textureBuffer);
		assert(SUCCEEDED(Hr));
	}
	//ディスクリプタヒープ
	{
		//ディスクリプタ(ビュー)３つ分のヒープをつくる
		Hr = createDescriptorHeap(3, Mesh.cbvTbvHeap);
		assert(SUCCEEDED(Hr));
		//１つめのディスクリプタ(ビュー)をヒープにつくる
		auto hCbvTbvHeap = Mesh.cbvTbvHeap->GetCPUDescriptorHandleForHeapStart();
		createConstantBufferView(Mesh.constBuffer0, hCbvTbvHeap);
		//２つめのディスクリプタ(ビュー)をヒープにつくる
		hCbvTbvHeap.ptr += CbvTbvIncSize;
		createConstantBufferView(Mesh.constBuffer1, hCbvTbvHeap);
		//３つめのディスクリプタ(ビュー)をヒープにつくる
		hCbvTbvHeap.ptr += CbvTbvIncSize;
		createTextureBufferView(Mesh.textureBuffer, hCbvTbvHeap);
	}
}

void STATIC_MESH::update(XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj, XMFLOAT4& lightPos)
{
	Mesh.cb0->lightPos = lightPos;
	Mesh.cb0->viewProj = view * proj;
	Mesh.cb0->world = world;
}

void STATIC_MESH::draw()
{
	//頂点をセット
	CommandList->IASetVertexBuffers(0, 1, &Mesh.vertexBufferView);
	CommandList->IASetIndexBuffer(&Mesh.indexBufferView);
	//ディスクリプタヒープをＧＰＵにセット
	CommandList->SetDescriptorHeaps(1, Mesh.cbvTbvHeap.GetAddressOf());
	//ディスクリプタヒープをディスクリプタテーブルにセット
	auto hCbvTbvHeap = Mesh.cbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
	CommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);
	//描画
	CommandList->DrawIndexedInstanced(Mesh.numIndices, 1, 0, 0, 0);
}
