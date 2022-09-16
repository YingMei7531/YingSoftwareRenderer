#include "YingGL.h"

int width;              //��Ļ���
int height;             //��Ļ�߶�
int depth;              //��Ļ���

float* shadowbuffer;    //��Ӱ����

Matrix modelview;       //ģ�Ӿ���
Matrix viewport;        //�ӿھ���
Matrix projection;      //ͶӰ����

Matrix shadowmat;       //ͶӰ�任����

Vec3f light;            //��Դ����ָ���Դ��
Vec3f viewpos;          //���λ��

void SetScreen(int w, int h, int d) {
    width = w;
    height = h;
    depth = d;
}

void SetModelView(Matrix m) {
    modelview = m;
}

//������x��y����(-1,1)ת������Ļ����
//z����(-1,1)ת����0~255
void SetViewport(int x, int y, int w, int h, int d) {
    viewport = Matrix::identity();
    viewport = Translate(x + w / 2.f, y + h / 2.f, d / 2.f) * Scale(w / 2.f, h / 2.f, d / 2.f);
}

void SetProjection(int width, int height, float fov, float near, float far) {
    Matrix m = Matrix::identity();
    float aspect = (float)width / height;
    m[0][0] = 1 / (tan(fov / 2 * acos(-1) / 180) * aspect);
    m[1][1] = 1 / tan(fov / 2 * acos(-1) / 180);
    m[2][2] = (far + near) / (far - near);
    m[2][3] = 2 * far * near / (far - near);
    m[3][2] = -1;
    projection = m;
}

void SetProjection(float dis) {
    projection = Matrix::identity();
    projection[3][2] = -1.f / dis;
}

Matrix GetMVMatrix() {
    return modelview;
}

Matrix GetProjection() {
    return projection;
}

Matrix GetMVPMatrix() {
    return projection * modelview;
}

Matrix GetViewport() {
    return viewport;
}

void SetShadowMatrix(Matrix m) {
    shadowmat = m;
}

Matrix GetShadowMatrix() {
    return shadowmat;
}

void SetLight(Vec3f v) {
    light = v;
}

Vec3f GetLight() {
    return light;
}

void SetViewPos(Vec3f pos) {
    viewpos = pos;
}

Vec3f GetViewPos() {
    return viewpos;
}

Vec3f Vec4ToVec3(Vec4f v) {
    v = v / v[3];
    return proj<3>(v);
}

Matrix Translate(float x, float y, float z) {
    Matrix m = Matrix::identity();
    m[0][3] = x;
    m[1][3] = y;
    m[2][3] = z;
    return m;
}

Matrix Scale(float x, float y, float z) {
    Matrix m = Matrix::identity();
    m[0][0] = x;
    m[1][1] = y;
    m[2][2] = z;
    return m;
}

Matrix Rotate(float x, float y, float z) {
    return RotateZ(z) * RotateX(x) * RotateY(y);
}

Matrix RotateX(float angle) {
    Matrix m = Matrix::identity();
    m[1][1] = cos(angle * acos(-1) / 180);
    m[1][2] = -sin(angle * acos(-1) / 180);
    m[2][1] = sin(angle * acos(-1) / 180);
    m[1][1] = cos(angle * acos(-1) / 180);
    return m;
}

Matrix RotateY(float angle) {
    Matrix m = Matrix::identity();
    m[0][0] = cos(angle * acos(-1) / 180);
    m[0][2] = sin(angle * acos(-1) / 180);
    m[2][0] = -sin(angle * acos(-1) / 180);
    m[2][2] = cos(angle * acos(-1) / 180);
    return m;
}

Matrix RotateZ(float angle) {
    Matrix m = Matrix::identity();
    m[0][0] = cos(angle * acos(-1) / 180);
    m[0][1] = -sin(angle * acos(-1) / 180);
    m[1][0] = sin(angle * acos(-1) / 180);
    m[1][1] = cos(angle * acos(-1) / 180);
    return m;
}

Matrix LookAt(Vec3f eye, Vec3f center, Vec3f up) {
    Vec3f z = (eye - center).normalize();
    Vec3f x = cross(up, z).normalize();
    Vec3f y = cross(z, x).normalize();
    Matrix rotation = Matrix::identity();
    for (int i = 0; i < 3; i++) {
        rotation[0][i] = x[i];
        rotation[1][i] = y[i];
        rotation[2][i] = z[i];
    }
    return rotation * Translate(-eye.x, -eye.y, -eye.z);
}

//������
//P = (1-u-v)A + uB + vC
//���� (1-u-v,u,v)
Vec3f Barycentric(Vec2f a, Vec2f b, Vec2f c, Vec2f p) {
    Vec3f vx(b.x - a.x, c.x - a.x, a.x - p.x);
    Vec3f vy(b.y - a.y, c.y - a.y, a.y - p.y);
    Vec3f u = cross(vx, vy);
    if (std::abs(u.z) >= 1e-2) return Vec3f(1 - (u.x + u.y) / u.z, u.x / u.z, u.y / u.z);
    return Vec3f(-1, -1, -1);
}

void DrawTriangle(Vec4f* pts, float* zbuffer, TGAImage& image, Shader* shader, Model* model) {
    int width = image.get_width();
    int height = image.get_height();
    Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    //ȷ�������εı߿�
    for (int i = 0; i < 3; i++) {
        bboxmin.x = std::min(bboxmin.x, pts[i][0] / pts[i][3]);
        bboxmin.y = std::min(bboxmin.y, pts[i][1] / pts[i][3]);
        bboxmax.x = std::max(bboxmax.x, pts[i][0] / pts[i][3]);
        bboxmax.y = std::max(bboxmax.y, pts[i][1] / pts[i][3]);
    }
    bboxmin.x = std::max(bboxmin.x, 0.0f);
    bboxmin.y = std::max(bboxmin.y, 0.0f);
    bboxmax.x = std::min(bboxmax.x, (float)width);
    bboxmax.y = std::min(bboxmax.y, (float)height);
    Vec3i p;
    //�����߿��е�ÿһ����
    for (p.x = bboxmin.x; p.x <= bboxmax.x; p.x++) {
        for (p.y = bboxmin.y; p.y <= bboxmax.y; p.y++) {
            //bc����������
            Vec3f bc = Barycentric(proj<2>(pts[0] / pts[0][3]), proj<2>(pts[1] / pts[1][3]), 
                proj<2>(pts[2] / pts[2][3]), Vec2f(p.x,p.y));
            //���������и�ֵ��˵��������������
            if (bc.x < 0 || bc.y < 0 || bc.z < 0) continue;
            //���������������
            Vec3f bc_revised(bc[0] / pts[0][3], bc[1] / pts[1][3], bc[2] / pts[2][3]);
            float zn = 1.0 / (bc_revised[0] + bc_revised[1] + bc_revised[2]);
            for (int i = 0; i < 3; i++) bc_revised[i] *= zn;
            //����zbuffer������ÿ�������zֵ���϶�Ӧ�������������
            float z = (pts[0][2] / pts[0][3]) * bc_revised[0] + (pts[1][2] / pts[1][3]) * bc_revised[1]
                + (pts[2][2] / pts[2][3]) * bc_revised[2];
            if (z >= 255.5 || z < 0) continue;
            if (zbuffer[p.x + p.y * width] < z) {
                //���û��shader����������Ȼ���
                if (shader == nullptr) {
                    zbuffer[p.x + p.y * width] = z;
                    continue;
                }
                //��ƬԪ��ɫ�����㵱ǰ������ɫ
                TGAColor color;
                bool discard = shader->Fragment(bc_revised, color, model);
                if (!discard) {
                    zbuffer[p.x + p.y * width] = z;
                    image.set(p.x, p.y, color);
                }
            }
        }
    }
}

void SetShadowBuffer(float* shadow) {
    shadowbuffer = shadow;
}

float GetShadow(Vec3f v) {
    float z = shadowbuffer[(int)v.x + (int)v.y * width];
    return 0.3 + 0.7 * (z < v.z + 23.34);
}