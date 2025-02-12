#pragma once
#include<DirectXMath.h>
using namespace DirectX;

//メッシュ（今回は１つだけ）-----------------------------------------------------------------
constexpr unsigned NumVertexElements = 12;
float Vertices[NumVertexElements*6] = 
{
	//Pos            Normal      TexCoord    Index1 Index2   Weight1, Weight2
	-0.25f,  0, 0,   0, 0, -1,   0, 0,       0,     1,       1.0f,     0.0f,//左上
	 0.25f,  0, 0,   0, 0, -1,   1, 0,       0,     1,       1.0f,     0.0f,//右上
	-0.25f, -1, 0,   0, 0, -1,   0, 0.5f,    0,     1,       0.5f,     0.5f,//左中
	 0.25f, -1, 0,   0, 0, -1,   1, 0.5f,    0,     1,       0.5f,     0.5f,//右中
	-0.25f, -2, 0,   0, 0, -1,   0, 1,       0,     1,       0.0f,     1.0f,//左下
	 0.25f, -2, 0,   0, 0, -1,   1, 1,       0,     1,       0.0f,     1.0f,//右下
};

unsigned short Indices[3*4] = 
{
	0, 1, 2,
	2, 1, 3,
	2, 3, 4,
	4, 3, 5,
};

float Ambient[4] = 
{
	0.2f, 0.2f, 0.2f, 0.0f,
};
float Diffuse[4] =
{
	0.8f, 0.6f, 0.6f, 1.0f,
};

const char* TextureFilename = "assets\\arm1.png";


//ボーンマトリックス-----------------------------------------------------------------
constexpr unsigned NumBones = 2;

//　親からの相対姿勢行列
XMMATRIX BindWorld[::NumBones] =
{
	{1,0,0,0,	0,1,0,0,	0,0,1,0,   0,0,0,1},//０番ボーンマトリックス
	{1,0,0,0,	0,1,0,0,	0,0,1,0,   0,-1,0,1},//１番ボーンマトリックス
};

//  キーフレーム数
constexpr int NumKeyframes = 3;
//　次のキーまでの間隔
int Interval = 120;
//  ボーン０回転用
float r0 = 40 * 3.1415926f / 180;
float c0 = cos(r0);
float s0 = sin(r0);
//　ボーン１回転用
float r1 = 80 * 3.1415926f / 180;
float c1 = cos(r1);
float s1 = sin(r1);
//  アニメーションキーフレーム行列
XMMATRIX KeyframeWorlds[NumBones][NumKeyframes] =
{ 
	//０番ボーンマトリックス
	{
		{1,0,0,0,	0,1,0,0,	0,0,1,0,	0,0,0,1},//キー０
		{c0,s0,0,0,  -s0,c0,0,0,  0,0,1,0,  0,0,0,1},//キー１
		{1,0,0,0,	0,1,0,0,	0,0,1,0,	0,0,0,1},//キー２
	},
	//１番ボーンマトリックス
	{
		{1,0,0,0,	0,1,0,0,	0,0,1,0,	0,0,0,1},//キー０
		{c1,s1,0,0, -s1,c1,0,0,  0,0,1,0,	0,0,0,1},//キー１
		{1,0,0,0,	0,1,0,0,	0,0,1,0,	0,0,0,1},//キー２
	},
};

//　親のインデックス
int ParentIdx[NumBones] =
{
	-1,//０番ボーンの親はいないので-1
	0, //１番ボーンの親は０
};