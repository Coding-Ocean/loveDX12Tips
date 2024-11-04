#include "HIERARCHY_MESH.h"
#include "model.h"

HIERARCHY_MESH::HIERARCHY_MESH()
	:Interval(::Interval)
{
}

HIERARCHY_MESH::~HIERARCHY_MESH()
{
	for (auto& parts : Parts) {
		parts.constBuffer0->Unmap(0, nullptr);
		parts.constBuffer1->Unmap(0, nullptr);
	}
}

void HIERARCHY_MESH::create()
{
	//パーツ配列をつくる
	for (int i = 0; i < NumParts; ++i) {

		PARTS parts;
		
		//頂点バッファ
		{
			//データサイズを求めておく
			UINT sizeInBytes = sizeof(Vertices[i]);
			UINT strideInBytes = sizeof(float) * NumVertexElements;
			parts.numVertices = sizeInBytes / strideInBytes;
			//バッファをつくる
			Hr = createBuffer(sizeInBytes, parts.vertexBuffer);
			assert(SUCCEEDED(Hr));
			//バッファにデータを入れる
			Hr = updateBuffer(Vertices[i], sizeInBytes, parts.vertexBuffer);
			assert(SUCCEEDED(Hr));
			//バッファビューをつくる
			createVertexBufferView(parts.vertexBuffer, sizeInBytes, strideInBytes, parts.vertexBufferView);
		}
		//頂点インデックスバッファ
		{
			//データサイズを求めておく
			UINT sizeInBytes = sizeof(Indices[i]);
			parts.numIndices = sizeInBytes / sizeof(UINT16);
			//バッファをつくる
			Hr = createBuffer(sizeInBytes, parts.indexBuffer);
			assert(SUCCEEDED(Hr));
			//バッファにデータを入れる
			Hr = updateBuffer(Indices[i], sizeInBytes, parts.indexBuffer);
			assert(SUCCEEDED(Hr));
			//インデックスバッファビューをつくる
			createIndexBufferView(parts.indexBuffer, sizeInBytes, parts.indexBufferView);
		}
		//コンスタントバッファ０
		{
			//バッファをつくる
			Hr = createBuffer(256, parts.constBuffer0);
			assert(SUCCEEDED(Hr));
			//マップしておく
			Hr = mapBuffer(parts.constBuffer0, (void**)&parts.cb0);
			assert(SUCCEEDED(Hr));
		}
		//コンスタントバッファ１
		{
			//バッファをつくる
			Hr = createBuffer(256, parts.constBuffer1);
			assert(SUCCEEDED(Hr));
			//マップしておく
			Hr = mapBuffer(parts.constBuffer1, (void**)&parts.cb1);
			assert(SUCCEEDED(Hr));
			//データを入れる
			parts.cb1->ambient = { Ambient[i][0],Ambient[i][1],Ambient[i][2],Ambient[i][3] };
			parts.cb1->diffuse = { Diffuse[i][0],Diffuse[i][1],Diffuse[i][2],Diffuse[i][3] };
		}
		//テクスチャバッファ
		{
			Hr = createTextureBuffer(TextureFilename, parts.textureBuffer);
			assert(SUCCEEDED(Hr));
		}
		//ディスクリプタヒープ
		{
			//ディスクリプタ(ビュー)３つ分のヒープをつくる
			Hr = createDescriptorHeap(3, parts.cbvTbvHeap);
			assert(SUCCEEDED(Hr));
			//１つめのディスクリプタ(ビュー)をヒープにつくる
			auto hCbvTbvHeap = parts.cbvTbvHeap->GetCPUDescriptorHandleForHeapStart();
			createConstantBufferView(parts.constBuffer0, hCbvTbvHeap);
			//２つめのディスクリプタ(ビュー)をヒープにつくる
			hCbvTbvHeap.ptr += CbvTbvIncSize;
			createConstantBufferView(parts.constBuffer1, hCbvTbvHeap);
			//３つめのディスクリプタ(ビュー)をヒープにつくる
			hCbvTbvHeap.ptr += CbvTbvIncSize;
			createTextureBufferView(parts.textureBuffer, hCbvTbvHeap);
		}
	
		//階層マトリックス
		{
			//親のインデックス
			parts.parentIdx = ::ParentIdx[i];

			//親から見た相対姿勢行列
			parts.bindWorld = ::BindWorld[i];
			//parts.bindWorld = XMMatrixIdentity();

			//アニメーションデータ。キーフレーム行列
			for (int j = 0; j < ::NumKeyframes; j++){
				parts.keyframeWorlds.push_back(KeyframeWorlds[j][i]);
				//parts.keyframeWorlds.push_back(XMMatrixIdentity());
			}
		}

		Parts.push_back(parts);
	}

	//自分の子供のインデックスをchildIdxs配列にセット
	for (int i = 0; i < ::NumParts; i++){
		for (int j = 0; j < ::NumParts; j++){
			if (i == j)	{
				continue;
			}
			if (Parts[i].parentIdx == Parts[j].parentIdx - 1){
				Parts[i].childIdxs.push_back(j);
			}
		}
	}
}

void HIERARCHY_MESH::update(XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj, XMFLOAT4& lightPos)
{
	//どのキーフレームの間にいるのか
	int keyFrameIdx = FrameCount / Interval;
	//最後まで行ったら最初から
	if (keyFrameIdx + 1 >= Parts[0].keyframeWorlds.size()){
		keyFrameIdx = 0;
		FrameCount = 0;
	}
	//キーフレーム行列の線形補間
	float t = FrameCount % Interval;
	t /= Interval;
	for (int i = 0; i < ::NumParts; i++) {
		XMMATRIX a = Parts[i].keyframeWorlds[keyFrameIdx];
		XMMATRIX b = Parts[i].keyframeWorlds[keyFrameIdx + 1];
		Parts[i].currentFrameWorld = LerpMatrix(a, b, t);
	}
	//全パーツのworldを更新（再起関数）
	UpdateWorlds(Parts[0], world);
	//次のフレームへ
	FrameCount++;

	//コンスタントバッファ更新
	for (auto& parts : Parts) {
		parts.cb0->worldViewProj = parts.world * view * proj;
		parts.cb0->world = parts.world;
		parts.cb0->lightPos = lightPos;
	}
}
void HIERARCHY_MESH::UpdateWorlds(PARTS& parts, XMMATRIX& parentWorld)
{
	parts.world = parts.currentFrameWorld * parts.bindWorld * parentWorld;

	for(auto& childIdx : parts.childIdxs) {
		UpdateWorlds(Parts[childIdx], parts.world);
	}
}
//2つの行列の補間計算
XMMATRIX HIERARCHY_MESH::LerpMatrix(XMMATRIX& m1, XMMATRIX& m2, float t)
{
	//tを0から1の範囲に制限
	t = (t < 0) ? 0 : (t > 1) ? 1 : t;

	//行列からクォータニオンに変換
	XMVECTOR q1 = XMQuaternionRotationMatrix(m1);
	XMVECTOR q2 = XMQuaternionRotationMatrix(m2);
	//クォータニオン間でSLERP
	XMVECTOR q = XMQuaternionSlerp(q1, q2, t);
	//補間されたクォータニオンを行列に変換
	XMMATRIX m = XMMatrixRotationQuaternion(q);

	//行列から平行移動成分を抽出
	XMVECTOR t1 = m1.r[3];
	XMVECTOR t2 = m2.r[3];
	//平行移動成分を線形補間
	XMVECTOR t_ = XMVectorLerp(t1, t2, t);
	//mの回転成分を保持しつつ、補間された平行移動を適用
	m.r[3] = t_;

	return m;
}

void HIERARCHY_MESH::draw()
{
	for (auto& parts : Parts) {
		//頂点をセット
		CommandList->IASetVertexBuffers(0, 1, &parts.vertexBufferView);
		CommandList->IASetIndexBuffer(&parts.indexBufferView);
		//ディスクリプタヒープをＧＰＵにセット
		CommandList->SetDescriptorHeaps(1, parts.cbvTbvHeap.GetAddressOf());
		//ディスクリプタヒープをディスクリプタテーブルにセット
		auto hCbvTbvHeap = parts.cbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
		CommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);
		//描画
		CommandList->DrawIndexedInstanced(parts.numIndices, 1, 0, 0, 0);
	}
}
