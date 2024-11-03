#pragma once
#define _XM_NO_INTRINSICS_
#include<DirectXMath.h>
using namespace DirectX;

constexpr int NumParts = 1;

constexpr unsigned NumVertexElements = 12;
float Vertices[NumParts][NumVertexElements*6] = 
{
	//Pos               Normal             TexCoord    Index1 Index2   Weight1, Weight2
	-0.25,  0.0, 0.0,   0.0, 0.0, -1.0,    0.0, 0.0,   0,     1,       1.0,     0.0,
	 0.25,  0.0, 0.0,   0.0, 0.0, -1.0,    1.0, 0.0,   0,     1,       1.0,     0.0,
	-0.25, -1.0, 0.0,   0.0, 0.0, -1.0,    0.0, 0.5,   0,     1,       0.5,     0.5,
	 0.25, -1.0, 0.0,   0.0, 0.0, -1.0,    1.0, 0.5,   0,     1,       0.5,     0.5,
	-0.25, -2.0, 0.0,   0.0, 0.0, -1.0,    0.0, 1.0,   0,     1,       0.0,     1.0,
	 0.25, -2.0, 0.0,   0.0, 0.0, -1.0,    1.0, 1.0,   0,     1,       0.0,     1.0,
};

unsigned short Indices[NumParts][3*4] = 
{
	0, 1, 2,
	2, 1, 3,
	2, 3, 4,
	4, 3, 5,
};

float Ambient[NumParts][4] = 
{
	0.2f, 0.2f, 0.2f, 0.0f,
};
float Diffuse[NumParts][4] =
{
	0.8f, 0.6f, 0.6f, 1.0f,
};

const char* TextureFilename = "assets\\arm2.png";

//階層ボーンマトリックスデータ
constexpr unsigned NumBones = 2;
//　親のインデックス
int ParentIdx[NumBones] =
{
	-1,//０番ボーンの親はいないので-1
	0, //１番ボーンの親は０
};

//　親からの相対姿勢行列
XMMATRIX BindWorld[NumBones] =
{
	{1.0,0.0,0.0,0.0,	0.0,1.0,0.0,0.0,	0.0,0.0,1.0,0.0,	0.0, 0.0,0.0,1.0},//０番ボーン
	{1.0,0.0,0.0,0.0,	0.0,1.0,0.0,0.0,	0.0,0.0,1.0,0.0,	0.0,-1.0,0.0,1.0},//１番ボーン
};

//アニメーションデータ
//  キーフレーム数
constexpr int NumKeyframes = 5;
//　次のキーまでの間隔
int Interval = 60;
//  キーフレーム行列
XMMATRIX KeyframeWorlds[NumKeyframes][NumBones] =
{
	//キー０
	{
		{1.0,0.0,0.0,0.0,	0.0,1.0,0.0,0.0,	0.0,0.0,1.0,0.0,	0.0,0.0,0.0,1.0},//０番ボーン
		{1.0,0.0,0.0,0.0,	0.0,1.0,0.0,0.0,	0.0,0.0,1.0,0.0,	0.0,0.0,0.0,1.0},//１番ボーン
	},
	//キー１
	{
		{0.7,-0.7,0.0,0.0,	 0.7,0.7,0.0,0.0,	0.0,0.0,1.0,0.0,	0.0,0.0,0.0,1.0},//０番ボーン
		{0.0, 1.0,0.0,0.0,	-1.0,0.0,0.0,0.0,	0.0,0.0,1.0,0.0,	0.0,0.0,0.0,1.0},//１番ボーン
	},
	//キー２
	{
		{1.0,0.0,0.0,0.0,	0.0,1.0,0.0,0.0,	0.0,0.0,1.0,0.0,	0.0,0.0,0.0,1.0},//０番ボーン
		{1.0,0.0,0.0,0.0,	0.0,1.0,0.0,0.0,	0.0,0.0,1.0,0.0,	0.0,0.0,0.0,1.0},//１番ボーン
	},
	//キー３
	{
		{0.7,-0.7,0.0,0.0,	0.7,0.7,0.0,0.0,	0.0,0.0,1.0,0.0,	0.0,0.0,0.0,1.0},//０番ボーン
		{0.0,-1.0,0.0,0.0,  1.0,0.0,0.0,0.0,	0.0,0.0,1.0,0.0,	0.0,0.0,0.0,1.0},//１番ボーン
	},
	//キー４
	{
		{1.0,0.0,0.0,0.0,	0.0,1.0,0.0,0.0,	0.0,0.0,1.0,0.0,	0.0,0.0,0.0,1.0},//０番ボーン
		{1.0,0.0,0.0,0.0,	0.0,1.0,0.0,0.0,	0.0,0.0,1.0,0.0,	0.0,0.0,0.0,1.0},//１番ボーン
	},
};
