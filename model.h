#pragma once
const float ClearColor[] = {0.25f, 0.5f, 0.9f, 1.0f};

unsigned NumVertexElements = 5;
float Vertices[] = {
	//position            texcoord
	-0.25f,  0.5f,  0.0f,  0.0f,  0.0f, //左上
	-0.25f, -0.5f,  0.0f,  0.0f,  1.0f, //左下
	 0.25f,  0.5f,  0.0f,  1.0f,  0.0f, //右上
	 0.25f, -0.5f,  0.0f,  1.0f,  1.0f, //右下
};

unsigned short Indices[] = {
	0,1,2,
	2,1,3,
};

float Diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};

const char* TextureFilename = "";//今回はソース内に記述している