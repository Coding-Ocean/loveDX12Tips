#pragma once

//�P���_�̗v�f��
unsigned NumVertexElements = 8;

float Vertices[] = {
	//position            normal               texcoord
	-0.5f,  0.5f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f, //����
	-0.5f, -0.5f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f, //����
	 0.5f,  0.5f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f, //�E��
	 0.5f, -0.5f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f, //�E��
};

unsigned short Indices[] = {
	0,1,2,
	2,1,3,
};

float Ambient[] = { 0.3f, 0.3f, 0.3f, 0.0f };
float Diffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
float Specular[] = { 0.8f, 0.6f, 0.6f, 25.0f };

const char* TextureFilename = "assets\\wall.png";