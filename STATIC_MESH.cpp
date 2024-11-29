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
	HRESULT Hr = E_FAIL;

	//頂点バッファ
	{
		//データサイズを求めておく
		UINT sizeInBytes = sizeof(Vertices);
		UINT strideInBytes = sizeof(float) * NumVertexElements;
		//バッファをつくる
		Hr = createBuffer(sizeInBytes, Mesh.vertexBuffer);
		assert(SUCCEEDED(Hr));
		//バッファにデータを入れる
		Hr = updateBuffer(Vertices, sizeInBytes, Mesh.vertexBuffer);
		assert(SUCCEEDED(Hr));
		//バッファビューをつくる
		createVertexBufferView(Mesh.vertexBuffer, sizeInBytes, strideInBytes, Mesh.vbv);
	}
	//頂点インデックスバッファ
	{
		//データサイズを求めておく
		UINT sizeInBytes = sizeof(Indices);
		//バッファをつくる
		Hr = createBuffer(sizeInBytes, Mesh.indexBuffer);
		assert(SUCCEEDED(Hr));
		//バッファにデータを入れる
		Hr = updateBuffer(Indices, sizeInBytes, Mesh.indexBuffer);
		assert(SUCCEEDED(Hr));
		//インデックスバッファビューをつくる
		createIndexBufferView(Mesh.indexBuffer, sizeInBytes, Mesh.ibv);
	}
	//コンスタントバッファ０
	{
		//バッファをつくる
		Hr = createBuffer(256, Mesh.constBuffer0);
		assert(SUCCEEDED(Hr));
		//マップしておく
		Hr = mapBuffer(Mesh.constBuffer0, (void**)&Mesh.cb0);
		assert(SUCCEEDED(Hr));
		//ビューをつくる
		Mesh.cbvTbvIdx = createConstantBufferView(Mesh.constBuffer0);
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
		//ビューをつくる
		createConstantBufferView(Mesh.constBuffer1);
	}
	//テクスチャバッファ
	{
		//ファイルを読み込んで、バッファをつくる
		Hr = createTextureBuffer(TextureFilename, Mesh.textureBuffer);
		assert(SUCCEEDED(Hr));
		//ビューをつくる
		createTextureBufferView(Mesh.textureBuffer);
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
	drawMesh(Mesh.vbv, Mesh.ibv, Mesh.cbvTbvIdx);
}
