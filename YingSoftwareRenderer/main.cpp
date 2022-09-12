#include "YingGL.h" 
#include "Shaders.h"

Model *model = NULL;
const int width  = 800;
const int height = 800;
const int depth = 255;

int main(int argc, char** argv) {
    //命令行控制方式和代码方式构造model
    //构造模型(obj文件路径)
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("obj/african_head.obj");
    }

    //构造image
    TGAImage image(width, height, TGAImage::RGB);
    //构造zbuffer
    float* zbuffer = new float[width * height];
    for (int i = 0; i < width * height; i++) zbuffer[i] = std::numeric_limits<float>::min();
    //构造shadowbuffer
    float* shadowbuffer = new float[width * height];
    for (int i = 0; i < width * height; i++) zbuffer[i] = std::numeric_limits<float>::min();

    //光照方向（指向光源）
    Vec3f lightdir(0, 0, 1);
    //摄像机位置
    Vec3f eye(2, 1, 2);
    //焦点位置
    Vec3f center(0, 0, 0);

    //设置屏幕参数
    SetScreen(width, height, depth);

    //设置矩阵
    SetModelView(LookAt(eye, center, Vec3f(0, 1, 0)));
    SetViewport(0, 0, width, height, depth * 2);
    SetProjection(width, height, 60, 0.3, 10);

    //传入摄像机和光源信息
    SetViewPos(eye);
    SetLight(lightdir);

    //选择shader
    //Shader* shader = new NormalShader();
    Shader* shader = new NormalShadowShader();
    //Shader* shader = new BlinnPhongShader();

    //绘制阴影缓存

    //将视角放到光源处
    SetModelView(LookAt(Vec3f(0,0,0) + lightdir * 2, center, Vec3f(0, 1, 0)));

    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        Vec3f screen_coords[3];
        for (int j = 0; j < 3; j++) {
            screen_coords[j] = Vec4ToVec3(GetViewport() * GetMVPMatrix() * embed<4>(model->vert(i, j)));
        }
        DrawTriangle(screen_coords, shadowbuffer, image, nullptr, model);
    }

    //设置阴影矩阵和阴影缓存
    SetShadowMatrix(GetViewport() * GetMVPMatrix());
    SetShadowBuffer(shadowbuffer);

    //正常渲染

    //将视角放到摄像机
    SetModelView(LookAt(eye, center, Vec3f(0, 1, 0)));

    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        Vec3f screen_coords[3];
        for (int j = 0; j < 3; j++) {
            screen_coords[j] = Vec4ToVec3(shader->Vertex(i, j, model));
        }
        DrawTriangle(screen_coords, zbuffer, image, shader, model);
    }

    //渲染完成
    std::cout << "end" << std::endl;

    delete shader;

    delete[] zbuffer;
    delete[] shadowbuffer;

    //tga默认原点在左上角，现需要指定为左下角，所以进行竖直翻转
    image.flip_vertically();
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}