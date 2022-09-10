#pragma once

#include <cmath>
#include "geometry.h"
#include "model.h"

//设置模视矩阵
void SetModelView(Matrix m);
//设置视口矩阵
void SetViewport(int x, int y, int w, int h, int depth);
//设置投影矩阵
void SetProjection(int width, int height, float fov, float near, float far);
//设置投影矩阵
void SetProjection(float dis);
//获取模视矩阵
Matrix GetMVMatrix();
//获取投影矩阵
Matrix GetProjection();
//获取模视投影矩阵
Matrix GetMVPMatrix();
//获取视口矩阵
Matrix GetViewport();

//设置光源方向
void SetLight(Vec3f v);
//获取光源方向
Vec3f GetLight();
//设置相机位置
void SetViewPos(Vec3f pos);
//获取相机位置
Vec3f GetViewPos();

//平移
Matrix Translate(float x, float y, float z);
//缩放
Matrix Scale(float x, float y, float z);
//旋转
Matrix Rotate(float x, float y, float z);
//沿X轴旋转
Matrix RotateX(float angle);
//沿Y轴旋转
Matrix RotateY(float angle);
//沿Z轴旋转
Matrix RotateZ(float angle);

//eye是摄像机位置，center是焦点位置，up是坐标系Y轴方向
Matrix LookAt(Vec3f eye, Vec3f center, Vec3f up);

//自定义Shader
//Vertex是顶点着色器，Fragment是片元着色器
class Shader {
public:
	virtual ~Shader() {};
	virtual Vec4f Vertex(int iface, int nthvert, Model* model) = 0;
	virtual bool Fragment(Vec3f bar, TGAColor& color, Model* model) = 0;
};

//绘制三角形
void DrawTriangle(Vec3f* pts, TGAImage& zbuffer, TGAImage& image, Shader* shader, Model* model);