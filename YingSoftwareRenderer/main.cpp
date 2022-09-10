#include "YingGL.h" 
#include "Shaders.h"

Model *model = NULL;
const int width  = 800;
const int height = 600;
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
    TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);

    //光照方向（指向光源）
    Vec3f light_dir(0, 0, 1);
    //摄像机位置
    Vec3f eye(1.5, 0.5, 1.5);
    //焦点位置
    Vec3f center(0, 0, 0);

    //设置矩阵
    SetModelView(LookAt(eye, center, Vec3f(0, 1, 0)));
    SetViewport(0, 0, width, height, depth * 2);
    //SetProjection((eye-center).norm());
    SetProjection(width, height, 60, 0.3, 10);

    //传入摄像机和光源信息
    SetViewPos(eye);
    SetLight(light_dir);

    //选择shader
    Shader* shader = new NormalShader();
    //Shader* shader = new BlinnPhongShader();

    for (int i=0; i<model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        Vec3f screen_coords[3];
        for (int j=0; j<3; j++) {
            Vec4f v = GetViewport() * shader->Vertex(i, j, model);
            v = v / v[3];
            screen_coords[j] = proj<3>(v);
        }
        DrawTriangle(screen_coords, zbuffer, image, shader, model);
    }

    std::cout << "end" << std::endl;

    delete shader;

    //tga默认原点在左上角，现需要指定为左下角，所以进行竖直翻转
    image.flip_vertically();
    image.write_tga_file("output.tga");
    zbuffer.flip_vertically();
    zbuffer.write_tga_file("zbuffer.tga");
    delete model;
    return 0;
}