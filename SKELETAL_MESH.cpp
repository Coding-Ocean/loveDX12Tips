#include "SKELETAL_MESH.h"
#include "model.h"

SKELETAL_MESH::SKELETAL_MESH()
	:Interval(::Interval)
{
}

SKELETAL_MESH::~SKELETAL_MESH()
{
	unmapBuffer(Mesh.constBuffer0);
	unmapBuffer(Mesh.constBuffer1);
}

void SKELETAL_MESH::create()
{
	HRESULT Hr;

	//メッシュをつくる
	{
		//頂点バッファ
		{
			//データサイズを求めておく
			UINT sizeInBytes = sizeof(::Vertices);
			UINT strideInBytes = sizeof(float) * ::NumVertexElements;
			//バッファをつくる
			Hr = createBuffer(sizeInBytes, Mesh.vertexBuffer);
			assert(SUCCEEDED(Hr));
			//バッファにデータを入れる
			Hr = updateBuffer(::Vertices, sizeInBytes, Mesh.vertexBuffer);
			assert(SUCCEEDED(Hr));
			//ビューをつくる
			createVertexBufferView(Mesh.vertexBuffer, sizeInBytes, strideInBytes, Mesh.vbv);
		}
		//頂点インデックスバッファ
		{
			//データサイズを求めておく
			UINT sizeInBytes = sizeof(::Indices);
			//バッファをつくる
			Hr = createBuffer(sizeInBytes, Mesh.indexBuffer);
			assert(SUCCEEDED(Hr));
			//バッファにデータを入れる
			Hr = updateBuffer(::Indices, sizeInBytes, Mesh.indexBuffer);
			assert(SUCCEEDED(Hr));
			//ビューをつくる
			createIndexBufferView(Mesh.indexBuffer, sizeInBytes, Mesh.ibv);
		}
		//コンスタントバッファ０
		{
			//バッファをつくる
			Hr = createBuffer(alignedSize(sizeof(CONST_BUF0)), Mesh.constBuffer0);
			assert(SUCCEEDED(Hr));
			//マップしておく
			Hr = mapBuffer(Mesh.constBuffer0, (void**)&Mesh.cb0);
			assert(SUCCEEDED(Hr));
			//ビューをつくり、インデックスを受け取っておく
			Mesh.cbvTbvIdx = createConstantBufferView(Mesh.constBuffer0);
		}
		//コンスタントバッファ１
		{
			//バッファをつくる
			Hr = createBuffer(alignedSize(sizeof(CONST_BUF1)), Mesh.constBuffer1);
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
			Hr = createTextureBuffer(TextureFilename, Mesh.textureBuffer);
			assert(SUCCEEDED(Hr));
			//ビューをつくる
			createTextureBufferView(Mesh.textureBuffer);
		}
	}

	//ボーンマトリックス
	{
		for (int i = 0; i < ::NumBones; ++i) {
			BONE bone;

			//親から見た相対姿勢行列
			bone.bindWorld = ::BindWorld[i];

			//アニメーションデータ。キーフレーム行列
			for (int j = 0; j < ::NumKeyframes; j++) {
				bone.keyframeWorlds.push_back(KeyframeWorlds[i][j]);
			}

			//親のインデックス
			bone.parentIdx = ::ParentIdx[i];

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

void SKELETAL_MESH::update(XMMATRIX& world, XMMATRIX& viewProj, XMFLOAT4& lightPos)
{
	//どのキーフレームの間にいるのか
	int keyFrameIdx = FrameCount / Interval;
	if (keyFrameIdx + 1 >= Bones[0].keyframeWorlds.size()) {
		keyFrameIdx = 0;
		FrameCount = 0;
	}
	//キーフレーム行列の線形補間
	float t = static_cast<float>(FrameCount % Interval) / Interval;
	for (int i = 0; i < ::NumBones; i++) {
		XMMATRIX a = Bones[i].keyframeWorlds[keyFrameIdx];
		XMMATRIX b = Bones[i].keyframeWorlds[keyFrameIdx + 1];
		Bones[i].currentFrameWorld = LerpMatrix(a, b, t);
	}
	//全てのボーンworldを更新（再起関数）
	UpdateWorlds(Bones[0], world);
	//次のフレームへ
	FrameCount++;

	//コンスタントバッファ0更新
	Mesh.cb0->lightPos = lightPos;
	Mesh.cb0->viewProj = viewProj;
	for (int i = 0; i < ::NumBones; ++i) {
		Mesh.cb0->boneWorlds[i] = Bones[i].world;
	}
}
void SKELETAL_MESH::UpdateWorlds(BONE& bone, const XMMATRIX& parentWorld)
{
	//回転の中心を原点に移動させる逆行列
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
	XMVECTOR tr1 = m1.r[3];
	XMVECTOR tr2 = m2.r[3];
	//平行移動成分を線形補間
	XMVECTOR tr = XMVectorLerp(tr1, tr2, t);
	//mの回転成分を保持しつつ、補間された平行移動を適用
	m.r[3] = tr;

	return m;
}

void SKELETAL_MESH::draw()
{
	drawMesh(Mesh.vbv, Mesh.ibv, Mesh.cbvTbvIdx);
}
