#pragma once

//１頂点の要素数（position3 + normal3 + texcoord2）
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

float Ambient[] = { 0.2f, 0.2f, 0.2f, 0.0f };
float Diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };

const char* TextureFilename = "assets\\wall.png";