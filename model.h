#pragma once

unsigned NumVertexElements = 8;
float Vertices[] = {
	//position            normal               texcoord
	-0.5f,  0.5f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f, //ç∂è„
	-0.5f, -0.5f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f, //ç∂â∫
	 0.5f,  0.5f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f, //âEè„
	 0.5f, -0.5f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f, //âEâ∫
};

unsigned short Indices[] = {
	0,1,2,
	2,1,3,
};

float Ambient[] = { 0.3f, 0.3f, 0.3f, 0.0f };
float Diffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
float Specular[] = { 0.0f, 0.0f, 0.0f, 0.0f };

const char* TextureFilename = "assets\\penguin1.png";