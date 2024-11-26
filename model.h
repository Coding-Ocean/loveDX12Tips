#pragma once

unsigned NumVertexElements = 5;//１頂点の要素数
float Vertices[] = {
	//position            texcoord
	-0.5f,  0.5f,  0.0f,  0.0f,  0.0f, //左上
	-0.5f, -0.5f,  0.0f,  0.0f,  1.0f, //左下
	 0.5f,  0.5f,  0.0f,  1.0f,  0.0f, //右上
	 0.5f, -0.5f,  0.0f,  1.0f,  1.0f, //右下
};

unsigned short Indices[] = {
	0,1,2,
	2,1,3,
};

float Diffuse[] = {1.0f, 0.5f, 1.0f, 1.0f};

const char* TextureFilename = "assets\\penguin1.png";