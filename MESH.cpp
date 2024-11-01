#include "MESH.h"
#include "model.h"

MESH::MESH()
{
}

MESH::~MESH()
{
	for (auto& parts : Parts) {
		parts.ConstBuffer0->Unmap(0, nullptr);
		parts.ConstBuffer1->Unmap(0, nullptr);
	}
}

void MESH::create()
{
	//パーツ配列をつくる
	for (int i = 0; i < NumParts; ++i) {

		PARTS parts;
		
		//頂点バッファ
		{
			//データサイズを求めておく
			UINT sizeInBytes = sizeof(Vertices[i]);
			UINT strideInBytes = sizeof(float) * NumVertexElements;
			parts.NumVertices = sizeInBytes / strideInBytes;
			//バッファをつくる
			Hr = createBuffer(sizeInBytes, parts.VertexBuffer);
			assert(SUCCEEDED(Hr));
			//バッファにデータを入れる
			Hr = updateBuffer(Vertices[i], sizeInBytes, parts.VertexBuffer);
			assert(SUCCEEDED(Hr));
			//バッファビューをつくる
			createVertexBufferView(parts.VertexBuffer, sizeInBytes, strideInBytes, parts.VertexBufferView);
		}
		//頂点インデックスバッファ
		{
			//データサイズを求めておく
			UINT sizeInBytes = sizeof(Indices[i]);
			parts.NumIndices = sizeInBytes / sizeof(UINT16);
			//バッファをつくる
			Hr = createBuffer(sizeInBytes, parts.IndexBuffer);
			assert(SUCCEEDED(Hr));
			//バッファにデータを入れる
			Hr = updateBuffer(Indices[i], sizeInBytes, parts.IndexBuffer);
			assert(SUCCEEDED(Hr));
			//インデックスバッファビューをつくる
			createIndexBufferView(parts.IndexBuffer, sizeInBytes, parts.IndexBufferView);
		}
		//コンスタントバッファ０
		{
			//バッファをつくる
			Hr = createBuffer(256, parts.ConstBuffer0);
			assert(SUCCEEDED(Hr));
			//マップしておく
			Hr = mapBuffer(parts.ConstBuffer0, (void**)&parts.CB0);
			assert(SUCCEEDED(Hr));
		}
		//コンスタントバッファ１
		{
			//バッファをつくる
			Hr = createBuffer(256, parts.ConstBuffer1);
			assert(SUCCEEDED(Hr));
			//マップしておく
			Hr = mapBuffer(parts.ConstBuffer1, (void**)&parts.CB1);
			assert(SUCCEEDED(Hr));
			//データを入れる
			parts.CB1->ambient = { Ambient[0],Ambient[1],Ambient[2],Ambient[3] };
			parts.CB1->diffuse = { Diffuse[0],Diffuse[1],Diffuse[2],Diffuse[3] };
		}
		//テクスチャバッファ
		{
			Hr = createTextureBuffer(TextureFilename, parts.TextureBuffer);
			assert(SUCCEEDED(Hr));
		}
		//ディスクリプタヒープ
		{
			//ディスクリプタ(ビュー)３つ分のヒープをつくる
			Hr = createDescriptorHeap(3, parts.CbvTbvHeap);
			assert(SUCCEEDED(Hr));
			CbvTbvIncSize = cbvTbvIncSize();
			//１つめのディスクリプタ(ビュー)をヒープにつくる
			auto hCbvTbvHeap = parts.CbvTbvHeap->GetCPUDescriptorHandleForHeapStart();
			createConstantBufferView(parts.ConstBuffer0, hCbvTbvHeap);
			//２つめのディスクリプタ(ビュー)をヒープにつくる
			hCbvTbvHeap.ptr += CbvTbvIncSize;
			createConstantBufferView(parts.ConstBuffer1, hCbvTbvHeap);
			//３つめのディスクリプタ(ビュー)をヒープにつくる
			hCbvTbvHeap.ptr += CbvTbvIncSize;
			createTextureBufferView(parts.TextureBuffer, hCbvTbvHeap);
		}
	
		//階層マトリックス
		{
			//親のインデックス
			parts.parentIdx = ParentIdx[i];

			//親から見た相対姿勢行列
			parts.bindWorld = BindWorld[i];
			//parts.bindWorld = XMMatrixIdentity();

			//アニメーションデータ。キーフレーム行列
			for (int j = 0; j < NumKeyframes; j++)
			{
				parts.keyframeWorlds.push_back(KeyframeWorlds[j][i]);
				//parts.keyframeWorlds.push_back(XMMatrixIdentity());
			}
		}

		Parts.push_back(parts);
	}
	//自分の子供のインデックスをchildIdxs配列にセット
	for (int i = 0; i < NumParts; i++)
	{
		for (int j = 0; j < NumParts; j++)
		{
			if (i == j)
			{
				continue;
			}
			if (Parts[i].parentIdx == Parts[j].parentIdx - 1)
			{
				Parts[i].childIdxs.push_back(j);
			}
		}
	}
}

//2つの行列の間の線形補間を計算し、結果の行列を返す 
// クォータニオン→回転行列＝XMMatrixRotationQuaternion    
// 回転行列→クォータニオン＝XMQuaternionRotationMatrix
XMMATRIX MESH::LerpMatrix(XMMATRIX& a, XMMATRIX& b, float t)
{

	//それぞれの行列から平行移動成分を、それぞれベクトルに取り出す
	XMFLOAT3 vA, vB;
	vA.x = a._41;
	vA.y = a._42;
	vA.z = a._43;
	vB.x = b._41;
	vB.y = b._42;
	vB.z = b._43;

	//それぞれの行列からスケーリング成分を、それぞれベクトルに取り出す
	XMFLOAT3 vA1(a._11, a._12, a._13);
	XMVECTOR a1 = XMLoadFloat3(&vA1);
	XMVECTOR length = XMVector3Length(a1);
	float Sax = 0;//スケーリングX成分
	XMStoreFloat(&Sax, length);
	XMFLOAT3 vA2(a._21, a._22, a._23);
	XMVECTOR a2 = XMLoadFloat3(&vA2);
	length = XMVector3Length(a2);
	float Say = 0;//スケーリングY成分
	XMStoreFloat(&Say, length);
	XMFLOAT3 vA3(a._31, a._32, a._33);
	XMVECTOR a3 = XMLoadFloat3(&vA3);
	length = XMVector3Length(a3);
	float Saz = 0;//スケーリングZ成分
	XMStoreFloat(&Saz, length);

	XMFLOAT3 vB1(b._11, b._12, b._13);
	XMVECTOR b1 = XMLoadFloat3(&vB1);
	length = XMVector3Length(b1);
	float Sbx = 0;//スケーリングX成分
	XMStoreFloat(&Sbx, length);
	XMFLOAT3 vB2(b._21, b._22, b._23);
	XMVECTOR b2 = XMLoadFloat3(&vB2);
	length = XMVector3Length(b2);
	float Sby = 0;//スケーリングY成分
	XMStoreFloat(&Sby, length);
	XMFLOAT3 vB3(b._31, b._32, b._33);
	XMVECTOR b3 = XMLoadFloat3(&vB3);
	length = XMVector3Length(b3);
	float Sbz = 0;//スケーリングZ成分
	XMStoreFloat(&Sbz, length);

	//それぞれの行列から回転成分を、それぞれクォータニオンに取り出す
	//回転とスケーリングは合成さｓれているため、回転を取り出す前に、行列からスケーリング成分を除去しなくてはならない。（それぞれスケーリング係数で行を割る）
	a._11 /= Sax; a._12 /= Sax; a._13 /= Sax;
	a._21 /= Say; a._22 /= Say;	a._23 /= Say;
	a._31 /= Saz; a._32 /= Saz;	a._33 /= Saz;

	b._11 /= Sbx; b._12 /= Sbx;	b._13 /= Sbx;
	b._21 /= Sby; b._22 /= Sby;	b._23 /= Sby;
	b._31 /= Sbz; b._32 /= Sbz;	b._33 /= Sbz;

	XMVECTOR qA = XMQuaternionRotationMatrix(a);
	XMVECTOR qB = XMQuaternionRotationMatrix(b);

	//それぞれを、ｔの比率で線形補間する
	XMVECTOR vvA = XMLoadFloat3(&vA);
	XMVECTOR vvB = XMLoadFloat3(&vB);
	XMVECTOR vR = (1.0 - t) * vvA + t * vvB;

	XMVECTOR qR = XMQuaternionSlerp(qA, qB, t);

	//ベクトルとクォータニオンを行列に戻す
	XMMATRIX ret = XMMatrixRotationQuaternion(qR);

	ret._41 = XMVectorGetX(vR);
	ret._42 = XMVectorGetY(vR);
	ret._43 = XMVectorGetZ(vR);
	ret._44 = 1.0;

	return ret;
}

void MESH::update(int frameCount, int interval, XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj, XMFLOAT4& lightPos)
{
	int keyFrameIdx = frameCount / interval;
	if (keyFrameIdx + 1 >= Parts[0].keyframeWorlds.size())
	{
		return;
	}

	//キーフレーム行列の線形補間
	float t = frameCount % interval;
	t /= interval;
	for (int i = 0; i < NumParts; i++)
	{
		XMMATRIX a = Parts[i].keyframeWorlds[keyFrameIdx];
		XMMATRIX b = Parts[i].keyframeWorlds[keyFrameIdx + 1];
		Parts[i].currentFrameWorld = LerpMatrix(a, b, t);
	}

	//全パーツのfinalWorldを更新（再起関数）
	UpdateFinalWorld(Parts[0], world);

	//コンスタントバッファ更新
	for (auto& parts : Parts) {
		parts.CB0->worldViewProj = parts.finalWorld * view * proj;
		parts.CB0->world = parts.finalWorld;
		parts.CB0->lightPos = lightPos;
	}
}
void MESH::UpdateFinalWorld(PARTS& parts, XMMATRIX& parentWorld)
{
	parts.finalWorld = parts.currentFrameWorld * parts.bindWorld * parentWorld;

	for (int i = 0; i < parts.childIdxs.size(); i++)
	{
		UpdateFinalWorld(Parts[parts.childIdxs[i]], parts.finalWorld);
	}
}

void MESH::draw()
{
	for (auto& parts : Parts) {
		//頂点をセット
		CommandList->IASetVertexBuffers(0, 1, &parts.VertexBufferView);
		CommandList->IASetIndexBuffer(&parts.IndexBufferView);
		//ディスクリプタヒープをＧＰＵにセット
		CommandList->SetDescriptorHeaps(1, parts.CbvTbvHeap.GetAddressOf());
		//ディスクリプタヒープをディスクリプタテーブルにセット
		auto hCbvTbvHeap = parts.CbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
		CommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);
		//描画
		CommandList->DrawIndexedInstanced(parts.NumIndices, 1, 0, 0, 0);
	}
}
