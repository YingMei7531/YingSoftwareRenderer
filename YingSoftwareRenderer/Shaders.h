#pragma once

#include "YingGL.h"
#include "geometry.h"

//仅贴图的shader
class NormalShader : public Shader {
	mat<2, 3, float> varying_uv;
public:
	~NormalShader() {};
	Vec4f Vertex(int iface, int nthvert, Model* model);
	bool Fragment(Vec3f bar, TGAColor& color, Model* model);
};

//BlinnPhong光照模型的shader
class BlinnPhongShader : public Shader{
	mat<2, 3, float> varying_uv;
	mat<3, 3, float> varying_pos;
public:
	~BlinnPhongShader() {};
	Vec4f Vertex(int iface, int nthvert, Model* model);
	bool Fragment(Vec3f bar, TGAColor& color, Model* model);
};