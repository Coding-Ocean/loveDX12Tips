#include "SKELETAL_MESH.h"
#include "model.h"

SKELETAL_MESH::SKELETAL_MESH()
	:Interval(::Interval)
{
}

SKELETAL_MESH::~SKELETAL_MESH()
{
	for (auto& parts : Parts) {
		parts.constBuffer0->Unmap(0, nullptr);
		parts.constBuffer1->Unmap(0, nullptr);
	}
}

void SKELETAL_MESH::create()
{
	//パーツ配列をつくる(今回は１つだけ)
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
			Hr = createBuffer((sizeof(CONST_BUF0) + 0xff) & ~0xff, parts.constBuffer0);
			assert(SUCCEEDED(Hr));
			//マップしておく
			Hr = mapBuffer(parts.constBuffer0, (void**)&parts.cb0);
			assert(SUCCEEDED(Hr));
		}
		//コンスタントバッファ１
		{
			//バッファをつくる
			Hr = createBuffer((sizeof(CONST_BUF1) + 0xff) & ~0xff, parts.constBuffer1);
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

		Parts.push_back(parts);
	}

	//ボーン階層マトリックス
	{
		for (int i = 0; i < ::NumBones; ++i) {
			BONE bone;
			
			//親のインデックス
			bone.parentIdx = ::ParentIdx[i];

			//親から見た相対姿勢行列
			bone.bindWorld = ::BindWorld[i];

			//アニメーションデータ。キーフレーム行列
			for (int j = 0; j < ::NumKeyframes; j++) {
				bone.keyframeWorlds.push_back(KeyframeWorlds[j][i]);
			}

			Bones.push_back(bone);
		}
	}
	//自分の子供のインデックスをchildIdxs配列にセット
	for (int i = 0; i < ::NumBones; i++){
		for (int j = 0; j < ::NumBones; j++){
			if (i == j)	{
				continue;
			}
			if (Bones[i].parentIdx == Bones[j].parentIdx - 1){
				Bones[i].childIdxs.push_back(j);
			}
		}
	}
}

void SKELETAL_MESH::update(XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj, XMFLOAT4& lightPos)
{
	//どのキーフレームの間にいるのか
	int keyFrameIdx = FrameCount / Interval;
	if (keyFrameIdx + 1 >= Bones[0].keyframeWorlds.size()){
		keyFrameIdx = 0;
		FrameCount = 0;
	}
	//キーフレーム行列の線形補間
	float t = FrameCount % Interval;
	t /= Interval;
	for (int i = 0; i < ::NumBones; i++) {
		XMMATRIX a = Bones[i].keyframeWorlds[keyFrameIdx];
		XMMATRIX b = Bones[i].keyframeWorlds[keyFrameIdx + 1];
		Bones[i].currentFrameWorld = LerpMatrix(a, b, t);
	}
	//全てのボーンworldを更新（再起関数）
	UpdateWorlds(Bones[0], world);
	//次のフレームへ
	FrameCount++;

	//コンスタントバッファ0更新(ループしてるけど今回Partsはひとつ)
	for (auto& parts : Parts) {
		parts.cb0->viewProj = view * proj;
		parts.cb0->lightPos = lightPos;
		for (int i = 0; i < NumBones; ++i) {
			parts.cb0->boneWorlds[i] = Bones[i].world;
		}
	}
}
void SKELETAL_MESH::UpdateWorlds(BONE& bone, const XMMATRIX& parentWorld)
{
	XMMATRIX invBindWorld = XMMatrixInverse(nullptr, bone.bindWorld);
	bone.world = invBindWorld * bone.currentFrameWorld * bone.bindWorld * parentWorld;

	for(auto& childIdx : bone.childIdxs) {
		UpdateWorlds(Bones[childIdx], bone.world);
	}
}
//2つの行列の補間計算
XMMATRIX SKELETAL_MESH::LerpMatrix(XMMATRIX& m1, XMMATRIX& m2, float t)
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

void SKELETAL_MESH::draw()
{
	for (auto& parts : Parts) {//ループしてるけど今回Partsはひとつ
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
