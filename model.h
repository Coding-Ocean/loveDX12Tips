#pragma once

unsigned NumVertexElements = 8;
constexpr float N = 0.577350259f;//Šp‚©‚çÎ‚ß‚S‚T“x‚ÉL‚Ñ‚é–@ü‚ğİ’è‚·‚é‚½‚ß‚Ì’l
float Vertices[] = {
	//Pos					Normal      TexCoord
	-0.50f, -0.12f, -0.60f,	-N,-N,-N,	0.0f, 0.0f,
	-0.50f, -0.12f,  0.60f, -N,-N, N,	0.0f, 0.0f,
	-0.50f,  0.12f,  0.60f, -N, N, N,	0.0f, 0.0f,
	-0.50f,  0.12f, -0.60f, -N, N,-N,	0.0f, 0.0f,
	 0.50f, -0.12f, -0.60f,  N,-N,-N,	0.0f, 0.0f,
	 0.50f, -0.12f,  0.60f,  N,-N, N,	0.0f, 0.0f,
	 0.50f,  0.12f,  0.60f,  N, N, N,	0.0f, 0.0f,
	 0.50f,  0.12f, -0.60f,  N, N,-N,	0.0f, 0.0f,
};

unsigned short Indices[] = {
	2,1,0,
	2,0,3,
	7,4,5,
	7,5,6,
	3,0,4,
	3,4,7,
	6,5,1,
	6,1,2,
	2,3,7,
	2,7,6,
	0,1,5,
	0,5,4,
};

float Ambient[] = { 0.2f, 0.2f, 0.0f, 0.0f };
float Diffuse[] = { 0.0f, 0.5f, 0.0f, 1.0f };

const char* TextureFilename = "assets\\white.png";