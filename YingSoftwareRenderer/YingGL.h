#pragma once

#include <cmath>
#include "geometry.h"
#include "tgaimage.h"
#include "model.h"

//������Ļ����
void SetScreen(int w, int h, int d = 255);

//����ģ�Ӿ���
void SetModelView(Matrix m);
//�����ӿھ���
void SetViewport(int x, int y, int w, int h, int d = 255);
//����ͶӰ����
void SetProjection(int width, int height, float fov, float near, float far);
//����ͶӰ����
void SetProjection(float dis);
//��ȡģ�Ӿ���
Matrix GetMVMatrix();
//��ȡͶӰ����
Matrix GetProjection();
//��ȡģ��ͶӰ����
Matrix GetMVPMatrix();
//��ȡ�ӿھ���
Matrix GetViewport();

//������Ӱ����
void SetShadowMatrix(Matrix m);
//��ȡ��Ӱ����
Matrix GetShadowMatrix();

//���ù�Դ����
void SetLight(Vec3f v);
//��ȡ��Դ����
Vec3f GetLight();
//�������λ��
void SetViewPos(Vec3f pos);
//��ȡ���λ��
Vec3f GetViewPos();

//��Vec4תΪVec3
Vec3f Vec4ToVec3(Vec4f v);

//ƽ��
Matrix Translate(float x, float y, float z);
//����
Matrix Scale(float x, float y, float z);
//��ת
Matrix Rotate(float x, float y, float z);
//��X����ת
Matrix RotateX(float angle);
//��Y����ת
Matrix RotateY(float angle);
//��Z����ת
Matrix RotateZ(float angle);

//eye�������λ�ã�center�ǽ���λ�ã�up������ϵY�᷽��
Matrix LookAt(Vec3f eye, Vec3f center, Vec3f up);

//�Զ���Shader
//Vertex�Ƕ�����ɫ����Fragment��ƬԪ��ɫ��
class Shader {
public:
	virtual ~Shader() {};
	virtual Vec4f Vertex(int iface, int nthvert, Model* model) = 0;
	virtual bool Fragment(Vec3f bar, TGAColor& color, Model* model) = 0;
};

//����������
void DrawTriangle(Vec3f* pts, float* zbuffer, TGAImage& image, Shader* shader, Model* model);

//������Ӱ����
void SetShadowBuffer(float* shadow);
//��ȡ��Ӱ
float GetShadow(Vec3f v);