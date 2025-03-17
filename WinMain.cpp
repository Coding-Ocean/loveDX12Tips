#include"graphic.h"

//頂点バッファ
ComPtr<ID3D12Resource>   VertexBuffer;
D3D12_VERTEX_BUFFER_VIEW Vbv;
//コンスタントバッファ０
ComPtr<ID3D12Resource> ConstBuffer0;
//コンスタントバッファ０構造体。Header.hlsliと同じ並びにしておく
struct CONST_BUF0 {
	float time;
}* CB0;
UINT CbvIdx;

//Entry point
INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"Shader Toy", 2000, 1000);

	//リソース初期化
	{
		//頂点バッファ
		{
			float vertices[] = {
				//position            texcoord
				-1.0f,  1.0f,  0.0f,  0.0f,  0.0f, //左上
				-1.0f, -1.0f,  0.0f,  0.0f,  1.0f, //左下
				 1.0f,  1.0f,  0.0f,  1.0f,  0.0f, //右上
				 1.0f, -1.0f,  0.0f,  1.0f,  1.0f, //右下
			};
			unsigned numVertexElements = 5;
			//データサイズを求めておく
			UINT sizeInBytes = sizeof(vertices);
			UINT strideInBytes = sizeof(float) * numVertexElements;
			//バッファをつくる
			createBuffer(sizeInBytes, VertexBuffer);
			//バッファにデータを入れる
			updateBuffer(vertices, sizeInBytes, VertexBuffer);
			//バッファビューをつくる
			createVertexBufferView(VertexBuffer, sizeInBytes, strideInBytes, Vbv);
		}
		//コンスタントバッファ０
		{
			//バッファをつくる
			createBuffer(alignedSize(sizeof(CONST_BUF0)), ConstBuffer0);
			//マップしておく
			mapBuffer(ConstBuffer0, (void**)&CB0);
			//１つのディスクリプタのヒープをつくる
			createDescriptorHeap(1);
			//ビューをつくってインデックスをもらっておく
			CbvIdx = createConstantBufferView(ConstBuffer0);
		}
	}

	timeBeginPeriod(1);
	DWORD startTime = timeGetTime();
	while (!quit())
	{
		CB0->time = (timeGetTime() - startTime) / 1000.0f;

		beginRender();
		drawMesh(Vbv, CbvIdx);
		endRender();
	}
	timeEndPeriod(1);
	waitGPU();
	closeEventHandle();
	unmapBuffer(ConstBuffer0);
}
