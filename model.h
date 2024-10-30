#pragma once
const float ClearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

unsigned NumVertexElements = 8;
float Vertices[] = {
	//position            normal               texcoord
	-0.5f,  0.5f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f, //左上
	-0.5f, -0.5f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f, //左下
	 0.5f,  0.5f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f, //右上
	 0.5f, -0.5f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f, //右下
};

unsigned short Indices[] = {
	0,1,2,
	2,1,3,
};

float Ambient[] = { 0.3f, 0.3f, 0.3f, 0.0f };
float Diffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };

const char* TextureFilename = "assets\\wall.png";