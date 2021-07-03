#pragma once
#include<windows.h>
#include "..\math\geometry.h"
#include "..\model\model.h"
#include"..\camera\camera.h"

class Pipeline
{
public:
    Pipeline();
    Pipeline(Camera& camera, const char* model_name, float width, float height, vec3 light, float fovy, float nearplane, float farplane, HDC chdc);
    ~Pipeline();


    unsigned char* render(HDC chdc, int model_index, bool is_shadow, bool is_skybox);

    void pass(Model& model,mat<4, 4>& M_Model);
    void shadow_pass(Model& model,mat<4, 4>& M_Model);
    void skybox_pass(Model& model);
    void clear_framebuffer(unsigned char* framebuffer, int init_value);
    void clear_buffer(float* zbuffer, int init_value);

    Model* model_1_1;
    Model* model_1_2;
    Model* model_1_3;
    Model* model_2;
    Model* floor;
    Model* skybox;

    float* zbuffer;
    float* shadowbuffer;
    unsigned char* framebuffer;

    mat<4, 4>  M_View;
    mat<4, 4>  M_Perspective;
    mat<4, 4>  M_Ortho;
    mat<4, 4>  M_ViewPort;
    mat<4, 4>  M_ModelLight;

    float width;
    float height;

    vec3 light;
    vec3 lightdir;

    float fovy;
    float nearplane;
    float farplane;

    const char* model_name;

    HDC chdc;
    Camera& camera;
};

