#pragma once

#include "YingGL.h"

//��������ͼ��shader
class NormalShader : public Shader {
private:
	mat<2, 3, float> varying_uv;
public:
	~NormalShader() {};
	Vec4f Vertex(int iface, int nthvert, Model* model);
	bool Fragment(Vec3f bar, TGAColor& color, Model* model);
};

//��������ͼ����Ӱ��shader
class NormalShadowShader : public Shader {
private:
	mat<2, 3, float> varying_uv;
	mat<3, 3, float> varying_pos;
public:
	~NormalShadowShader() {};
	Vec4f Vertex(int iface, int nthvert, Model* model);
	bool Fragment(Vec3f bar, TGAColor& color, Model* model);
};

//BlinnPhong����ģ�͵�shader
class BlinnPhongShader : public Shader{
private:
	mat<2, 3, float> varying_uv;
	mat<3, 3, float> varying_pos;
public:
	~BlinnPhongShader() {};
	Vec4f Vertex(int iface, int nthvert, Model* model);
	bool Fragment(Vec3f bar, TGAColor& color, Model* model);
};