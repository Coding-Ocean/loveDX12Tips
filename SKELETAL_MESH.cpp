#include "SKELETAL_MESH.h"
#include "model.h"

SKELETAL_MESH::SKELETAL_MESH()
	:Interval(::Interval)
{
}

SKELETAL_MESH::~SKELETAL_MESH()
{
	ConstBuffer0->Unmap(0, nullptr);
	for (auto& parts : Parts) {
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

	//コンスタントバッファ０
	{
		//バッファをつくる
		Hr = createBuffer(512, ConstBuffer0);
		assert(SUCCEEDED(Hr));
		//マップしておく
		Hr = mapBuffer(ConstBuffer0, (void**)&Cb0);
		assert(SUCCEEDED(Hr));
	}

	//ディスクリプタヒープ
	{
		//ディスクリプタ(ビュー)３つ分のヒープをつくる
		Hr = createDescriptorHeap(1 + 2 * NumParts, CbvTbvHeap);
		assert(SUCCEEDED(Hr));
		//１つめのディスクリプタ(ビュー)をヒープにつくる
		auto hCbvTbvHeap = CbvTbvHeap->GetCPUDescriptorHandleForHeapStart();
		createConstantBufferView(ConstBuffer0, hCbvTbvHeap);
		for (auto& parts : Parts) {
			//２つめのディスクリプタ(ビュー)をヒープにつくる
			hCbvTbvHeap.ptr += CbvTbvIncSize;
			createConstantBufferView(parts.constBuffer1, hCbvTbvHeap);
			//３つめのディスクリプタ(ビュー)をヒープにつくる
			hCbvTbvHeap.ptr += CbvTbvIncSize;
			createTextureBufferView(parts.textureBuffer, hCbvTbvHeap);
		}
	}
}

//2つの行列の間の線形補間を計算し、結果の行列を返す 
XMMATRIX SKELETAL_MESH::LerpMatrix(XMMATRIX& a, XMMATRIX& b, float t)
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
	
	//回転行列 → クォータニオン
	XMVECTOR qA = XMQuaternionRotationMatrix(a);
	XMVECTOR qB = XMQuaternionRotationMatrix(b);
	//クォータニオン線形補完
	XMVECTOR qR = XMQuaternionSlerp(qA, qB, t);
	//クォータニオン → 回転行列
	XMMATRIX ret = XMMatrixRotationQuaternion(qR);

	//移動成分を線形補間
	XMVECTOR vvA = XMLoadFloat3(&vA);
	XMVECTOR vvB = XMLoadFloat3(&vB);
	XMVECTOR vR = (1.0f - t) * vvA + t * vvB;
	//移動成分を行列に戻す
	ret._41 = XMVectorGetX(vR);
	ret._42 = XMVectorGetY(vR);
	ret._43 = XMVectorGetZ(vR);
	ret._44 = 1.0;

	return ret;
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
	//全のworlボーンのworldを更新（再起関数）
	UpdateWorld(Bones[0], XMMatrixIdentity());
	//次のフレームへ
	FrameCount++;

	//コンスタントバッファ0更新
	Cb0->worldViewProj = world * view * proj;
	Cb0->world = world;
	Cb0->lightPos = lightPos;
	for (int i = 0; i < NumBones; ++i) {
		Cb0->boneWorlds[i] = Bones[i].world;
	}
}
void SKELETAL_MESH::UpdateWorld(BONE& bone, const XMMATRIX& parentWorld)
{
	XMMATRIX invBindWorld = XMMatrixInverse(nullptr, bone.bindWorld);
	bone.world = invBindWorld * bone.currentFrameWorld * bone.bindWorld * parentWorld;

	for(auto& childIdx : bone.childIdxs) {
		UpdateWorld(Bones[childIdx], bone.world);
	}
}

void SKELETAL_MESH::draw()
{
	//ディスクリプタヒープをＧＰＵにセット
	CommandList->SetDescriptorHeaps(1, CbvTbvHeap.GetAddressOf());
	//Cb0vをディスクリプタテーブル0にセット
	auto hCbvTbvHeap = CbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
	CommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);

	for (auto& parts : Parts) {
		//頂点をセット
		CommandList->IASetVertexBuffers(0, 1, &parts.vertexBufferView);
		CommandList->IASetIndexBuffer(&parts.indexBufferView);
		//cb1vとtbvをディスクリプタテーブル1にセット
		hCbvTbvHeap.ptr += CbvTbvIncSize;
		CommandList->SetGraphicsRootDescriptorTable(1, hCbvTbvHeap);
		//描画
		CommandList->DrawIndexedInstanced(parts.numIndices, 1, 0, 0, 0);
	}
}
