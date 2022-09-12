#include "Shaders.h"

Vec4f NormalShader::Vertex(int iface, int nthvert, Model* model) {
	varying_uv.set_col(nthvert, model->uv(iface, nthvert));
	Vec4f vertex = embed<4>(model->vert(iface, nthvert));
	return GetViewport() * GetMVPMatrix() * vertex;
}

bool NormalShader::Fragment(Vec3f bar, TGAColor& color, Model* model) {
	Vec2f uv = varying_uv * bar;
	color = model->diffuse(uv);
	return false;
}

Vec4f NormalShadowShader::Vertex(int iface, int nthvert, Model* model) {
	varying_uv.set_col(nthvert, model->uv(iface, nthvert));
	Vec4f vertex = embed<4>(model->vert(iface, nthvert));
	varying_pos.set_col(nthvert, proj<3>(vertex));
	return GetViewport() * GetMVPMatrix() * vertex;
}

bool NormalShadowShader::Fragment(Vec3f bar, TGAColor& color, Model* model) {
	Vec2f uv = varying_uv * bar;
	color = model->diffuse(uv);
	Vec3f pos = varying_pos * bar;
	pos = Vec4ToVec3(GetShadowMatrix() * embed<4>(pos));
	float shadow = GetShadow(pos);
	for (int i = 0; i < 3; i++) color[i] = std::min<float>((float)color[i] * shadow, 255);
	return false;
}

Vec4f BlinnPhongShader::Vertex(int iface, int nthvert, Model* model) {
	varying_uv.set_col(nthvert, model->uv(iface, nthvert));
	Vec4f vertex = embed<4>(model->vert(iface, nthvert));
	varying_pos.set_col(nthvert, proj<3>(vertex));
	return GetViewport() * GetMVPMatrix() * vertex;
}

bool BlinnPhongShader::Fragment(Vec3f bar, TGAColor& color, Model* model) {
	Vec2f uv = varying_uv * bar;
	Vec3f pos = varying_pos * bar;

	Vec3f n = proj<3>(GetMVMatrix() * embed<4>(model->normal(uv))).normalize();

	Vec3f lightdir = GetLight();
	Vec3f l = proj<3>(GetMVMatrix() * embed<4>(lightdir)).normalize();

	Vec3f viewdir = GetViewPos() - pos;
	Vec3f v = proj<3>(GetMVMatrix() * embed<4>(viewdir)).normalize();

	Vec3f h = (v + l).normalize();

	float diff = std::max(0.0f, n * l);
	float spec = pow(std::max(n * h, 0.0f), model->specular(uv));

	float shadow = GetShadow(pos);

	TGAColor c = model->diffuse(uv);
	for (int i = 0; i < 3; i++) color[i] = std::min<float>((float)c[i] * (diff + spec) * shadow, 255);
	return false;
}

Vec4f ToonShader::Vertex(int iface, int nthvert, Model* model) {
	varying_uv.set_col(nthvert, model->uv(iface, nthvert));
	Vec4f vertex = embed<4>(model->vert(iface, nthvert));
	varying_pos.set_col(nthvert, proj<3>(vertex));
	return GetViewport() * GetMVPMatrix() * vertex;
}

bool ToonShader::Fragment(Vec3f bar, TGAColor& color, Model* model) {
	Vec2f uv = varying_uv * bar;
	color = model->diffuse(uv);

	Vec3f pos = varying_pos * bar;
	pos = Vec4ToVec3(GetShadowMatrix() * embed<4>(pos));

	Vec3f n = proj<3>(GetMVMatrix() * embed<4>(model->normal(uv))).normalize();

	Vec3f lightdir = GetLight();
	Vec3f l = proj<3>(GetMVMatrix() * embed<4>(lightdir)).normalize();

	float diff = std::max(0.0f, n * l);
	diff = std::max<float>((int)(diff / 0.05) * 0.1 - 1, 0);

	float shadow = GetShadow(pos);

	for (int i = 0; i < 3; i++) color[i] = std::min<float>((float)255 * diff * shadow, 255);
	return false;
}
