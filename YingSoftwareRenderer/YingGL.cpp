#include "YingGL.h"

Matrix modelview;       //模视矩阵
Matrix viewport;        //视口矩阵
Matrix projection;      //投影矩阵

Vec3f light;            //光源方向（指向光源）
Vec3f viewpos;          //相机位置

void SetModelView(Matrix m) {
    modelview = m;
}

//将物体x，y坐标(-1,1)转换到屏幕坐标
//zbuffer(-1,1)转换到0~255
void SetViewport(int x, int y, int w, int h, int depth) {
    viewport = Matrix::identity();
    viewport = Translate(x + w / 2.f, y + h / 2.f, depth / 2.f) * Scale(w / 2.f, h / 2.f, depth / 2.f);
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

//求质心
//P = (1-u-v)A + uB + vC
//返回 (1-u-v,u,v)
Vec3f Barycentric(Vec2f a, Vec2f b, Vec2f c, Vec2f p) {
    Vec3f vx(b.x - a.x, c.x - a.x, a.x - p.x);
    Vec3f vy(b.y - a.y, c.y - a.y, a.y - p.y);
    Vec3f u = cross(vx, vy);
    if (std::abs(u.z) >= 1e-2) return Vec3f(1 - (u.x + u.y) / u.z, u.x / u.z, u.y / u.z);
    return Vec3f(-1, -1, -1);
}

void DrawTriangle(Vec3f* pts, TGAImage& zbuffer, TGAImage& image, Shader* shader, Model* model) {
    int width = image.get_width();
    int height = image.get_height();
    Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    //确定三角形的边框
    for (int i = 0; i < 3; i++) {
        bboxmin.x = std::min(bboxmin.x, pts[i].x);
        bboxmin.y = std::min(bboxmin.y, pts[i].y);
        bboxmax.x = std::max(bboxmax.x, pts[i].x);
        bboxmax.y = std::max(bboxmax.y, pts[i].y);
    }
    bboxmin.x = std::max(bboxmin.x, 0.0f);
    bboxmin.y = std::max(bboxmin.y, 0.0f);
    bboxmax.x = std::min(bboxmax.x, (float)width);
    bboxmax.y = std::min(bboxmax.y, (float)height);
    Vec3i p;
    //遍历边框中的每一个点
    for (p.x = bboxmin.x; p.x <= bboxmax.x; p.x++) {
        for (p.y = bboxmin.y; p.y <= bboxmax.y; p.y++) {
            //bc是质心坐标
            Vec3f bc = Barycentric(proj<2>(pts[0]), proj<2>(pts[1]), proj<2>(pts[2]), Vec2f(p.x,p.y));
            //质心坐标有负值，说明点在三角形外
            if (bc.x < 0 || bc.y < 0 || bc.z < 0) continue;
            //计算zbuffer，并且每个顶点的z值乘上对应的质心坐标分量
            p.z = pts[0][2] * bc[0] + pts[1][2] * bc[1] + pts[2][2] * bc[2];
            int z = std::max(0, std::min(255, int(p.z + 0.5)));
            if (zbuffer.get(p.x, p.y)[0] < z) {
                TGAColor color;
                //用片元着色器计算当前像素颜色
                bool discard = shader->Fragment(bc, color, model);
                if (!discard) {
                    zbuffer.set(p.x, p.y, TGAColor(z));
                    image.set(p.x, p.y, color);
                }
            }
        }
    }
}