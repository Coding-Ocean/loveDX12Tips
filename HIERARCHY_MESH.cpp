#include "HIERARCHY_MESH.h"
#include "model.h"

HIERARCHY_MESH::HIERARCHY_MESH()
	:Interval(::Interval)
{
}

HIERARCHY_MESH::~HIERARCHY_MESH()
{
	for (auto& mesh : Meshes) {
		unmapBuffer(mesh.constBuffer0);
		unmapBuffer(mesh.constBuffer1);
	}
}

void HIERARCHY_MESH::create()
{
	HRESULT Hr = E_FAIL;

	//パーツ配列をつくる
	for (int i = 0; i < NumMeshes; ++i) {

		MESH mesh;
		
		//頂点バッファ
		{
			//データサイズを求めておく
			UINT sizeInBytes = sizeof(Vertices[i]);
			UINT strideInBytes = sizeof(float) * NumVertexElements;
			//バッファをつくる
			Hr = createBuffer(sizeInBytes, mesh.vertexBuffer);
			assert(SUCCEEDED(Hr));
			//バッファにデータを入れる
			Hr = updateBuffer(Vertices[i], sizeInBytes, mesh.vertexBuffer);
			assert(SUCCEEDED(Hr));
			//バッファビューをつくる
			createVertexBufferView(mesh.vertexBuffer, sizeInBytes, strideInBytes, mesh.vbv);
		}
		//頂点インデックスバッファ
		{
			//データサイズを求めておく
			UINT sizeInBytes = sizeof(Indices[i]);
			//バッファをつくる
			Hr = createBuffer(sizeInBytes, mesh.indexBuffer);
			assert(SUCCEEDED(Hr));
			//バッファにデータを入れる
			Hr = updateBuffer(Indices[i], sizeInBytes, mesh.indexBuffer);
			assert(SUCCEEDED(Hr));
			//インデックスバッファビューをつくる
			createIndexBufferView(mesh.indexBuffer, sizeInBytes, mesh.ibv);
		}
		//コンスタントバッファ０
		{
			//バッファをつくる
			Hr = createBuffer(alignedSize(sizeof(CONST_BUF0)), mesh.constBuffer0);
			assert(SUCCEEDED(Hr));
			//マップしておく
			Hr = mapBuffer(mesh.constBuffer0, (void**)&mesh.cb0);
			assert(SUCCEEDED(Hr));
			//ビューをつくる
			mesh.cbvTbvIdx = createConstantBufferView(mesh.constBuffer0);
		}
		//コンスタントバッファ１
		{
			//バッファをつくる
			Hr = createBuffer(alignedSize(sizeof(CONST_BUF1)), mesh.constBuffer1);
			assert(SUCCEEDED(Hr));
			//マップしておく
			Hr = mapBuffer(mesh.constBuffer1, (void**)&mesh.cb1);
			assert(SUCCEEDED(Hr));
			//データを入れる
			mesh.cb1->ambient = { Ambient[i][0],Ambient[i][1],Ambient[i][2],Ambient[i][3] };
			mesh.cb1->diffuse = { Diffuse[i][0],Diffuse[i][1],Diffuse[i][2],Diffuse[i][3] };
			//ビューをつくる
			createConstantBufferView(mesh.constBuffer1);
		}
		//テクスチャバッファ
		{
			Hr = createTextureBuffer(TextureFilename, mesh.textureBuffer);
			assert(SUCCEEDED(Hr));
			//ビューをつくる
			createTextureBufferView(mesh.textureBuffer);
		}
	
		//階層マトリックス
		{
			//親のインデックス
			mesh.parentIdx = ::ParentIdx[i];

			//親から見た相対姿勢行列
			mesh.bindWorld = ::BindWorld[i];
			//mesh.bindWorld = XMMatrixIdentity();

			//アニメーションデータ。キーフレーム行列
			for (int j = 0; j < ::NumKeyframes; j++){
				mesh.keyframeWorlds.push_back(KeyframeWorlds[j][i]);
				//mesh.keyframeWorlds.push_back(XMMatrixIdentity());
			}
		}

		Meshes.push_back(mesh);
	}

	//自分の子供のインデックスをchildIdxs配列にセット
	for (int i = 0; i < ::NumMeshes; i++){
		for (int j = 0; j < ::NumMeshes; j++){
			if (i == j)	{
				continue;
			}
			if (Meshes[i].parentIdx == Meshes[j].parentIdx - 1){
				Meshes[i].childIdxs.push_back(j);
			}
		}
	}
}

void HIERARCHY_MESH::update(XMMATRIX& world, XMMATRIX& viewProj, XMFLOAT4& lightPos)
{
	//どのキーフレームの間にいるのか
	int keyFrameIdx = FrameCount / Interval;
	//最後まで行ったら最初から
	if (keyFrameIdx + 1 >= Meshes[0].keyframeWorlds.size()){
		keyFrameIdx = 0;
		FrameCount = 0;
	}
	//キーフレーム行列の線形補間
	float t = static_cast<float>(FrameCount % Interval);
	t /= Interval;
	for (int i = 0; i < ::NumMeshes; i++) {
		XMMATRIX a = Meshes[i].keyframeWorlds[keyFrameIdx];
		XMMATRIX b = Meshes[i].keyframeWorlds[keyFrameIdx + 1];
		Meshes[i].currentFrameWorld = LerpMatrix(a, b, t);
	}
	//全メッシュのworldを更新（再起関数）
	UpdateWorlds(Meshes[0], world);
	//次のフレームへ
	FrameCount++;

	//コンスタントバッファ更新
	for (auto& mesh : Meshes) {
		mesh.cb0->lightPos = lightPos;
		mesh.cb0->viewProj = viewProj;
		mesh.cb0->world = mesh.world;
	}
}
void HIERARCHY_MESH::UpdateWorlds(MESH& mesh, XMMATRIX& parentWorld)
{
	mesh.world = mesh.currentFrameWorld * mesh.bindWorld * parentWorld;

	for(auto& childIdx : mesh.childIdxs) {
		UpdateWorlds(Meshes[childIdx], mesh.world);
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
	for (auto& mesh : Meshes) {
		drawMesh(mesh.vbv, mesh.ibv, mesh.cbvTbvIdx);
	}
}
