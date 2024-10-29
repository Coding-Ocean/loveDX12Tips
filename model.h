#pragma once

unsigned NumVertexElements = 5;
float Vertices[] = {
	-0.5f, -0.5f,  0.0f,  0.0f,  1.0f, //ç∂â∫
	-0.5f,  0.5f,  0.0f,  0.0f,  0.0f, //ç∂è„
	 0.5f, -0.5f,  0.0f,  1.0f,  1.0f, //âEâ∫
	 0.5f,  0.5f,  0.0f,  1.0f,  0.0f, //âEè„
};

unsigned short Indices[] = {
	0,1,2,
	2,1,3,
};

float Diffuse[] = {1.0f, 1.0f, 0.0f, 1.0f};

const char* TextureFilename = "assets\\penguin1.png";