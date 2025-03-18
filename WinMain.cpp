#include"graphic.h"

class vec3 {
public:
	vec3() : e{ 0,0,0 } {}
	vec3(double e0, double e1, double e2) : e{ e0, e1, e2 } {}

	double x() const { return e[0]; }
	double y() const { return e[1]; }
	double z() const { return e[2]; }

	vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
	double operator[](int i) const { return e[i]; }
	double& operator[](int i) { return e[i]; }

	vec3& operator+=(const vec3& v) {
		e[0] += v.e[0];
		e[1] += v.e[1];
		e[2] += v.e[2];
		return *this;
	}

	vec3& operator*=(const double t) {
		e[0] *= t;
		e[1] *= t;
		e[2] *= t;
		return *this;
	}

	vec3& operator/=(const double t) {
		return *this *= 1 / t;
	}

	double length() const {
		return sqrt(length_squared());
	}

	double length_squared() const {
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
	}

public:
	double e[3];
};

// vec3 ユーティリティ関数
inline vec3 operator+(const vec3& u, const vec3& v) {
	return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
	return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
	return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3& v) {
	return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline vec3 operator*(const vec3& v, double t) {
	return t * v;
}

inline vec3 operator/(vec3 v, double t) {
	return (1 / t) * v;
}

inline double dot(const vec3& u, const vec3& v) {
	return u.e[0] * v.e[0]
		+ u.e[1] * v.e[1]
		+ u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v) {
	return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
		u.e[2] * v.e[0] - u.e[0] * v.e[2],
		u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(vec3 v) {
	return v / v.length();
}

// vec3 の型エイリアス
using point3 = vec3;   // 3D 点
using color = vec3;    // RGB 色


//頂点バッファ
ComPtr<ID3D12Resource>   VertexBuffer;
D3D12_VERTEX_BUFFER_VIEW Vbv;
//コンスタントバッファ０
ComPtr<ID3D12Resource> ConstBuffer0;
UINT CbvIdx;
//コンスタントバッファ０構造体。Header.hlsliと同じ並びにしておく
struct CONST_BUF0 {
	float time;
	float rayOrigin[3];
}* CB0;

//Entry point
INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
	window(L"RayTracing", 1920/2, 1080/2, WINDOW);
	//window(L"Shader Toy", 1080, 1080, WINDOW);

	//リソース初期化
	{
		//頂点バッファ
		{
			//texcoordは、xを-aspect ~ aspect,　yを-1~1とする
			float vertices[] = {
				//position            texcoord
				-1.0f,  1.0f,  0.0f,  -aspect(),  -1.0f, //左上
				-1.0f, -1.0f,  0.0f,  -aspect(),  1.0f, //左下
				 1.0f,  1.0f,  0.0f,  aspect(),  -1.0f, //右上
				 1.0f, -1.0f,  0.0f,  aspect(),  1.0f, //右下
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

	//auto aspect_ratio = aspect();
	//auto viewport_height = 2.0;
	//auto viewport_width = aspect_ratio * viewport_height;
	//auto focal_length = 1.0;
	//auto origin = point3(0, 0, 0);
	//auto horizontal = vec3(viewport_width, 0, 0);
	//auto vertical = vec3(0, viewport_height, 0);
	//auto lower_left_corner =
	//	origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);

	timeBeginPeriod(1);
	DWORD startTime = timeGetTime();
	ShowCursor(false);
	while (!quit())
	{
		CB0->time = (timeGetTime() - startTime) / 1000.0f;
		CB0->rayOrigin[0] = 0; CB0->rayOrigin[1] = 0; CB0->rayOrigin[2] = 0;
		beginRender();
		drawMesh(Vbv, CbvIdx);
		endRender();
	}

	waitGPU();
	closeEventHandle();
	unmapBuffer(ConstBuffer0);
	ShowCursor(true);
	timeEndPeriod(1);
}
