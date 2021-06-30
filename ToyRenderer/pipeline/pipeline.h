#pragma once
#include<windows.h>
#include "..\math\geometry.h"
#include "..\model\model.h"

class Pipeline
{
public:
    Pipeline();
    Pipeline(const char* model_name, float width, float height, vec3 light, vec3 camera, float fovy, float nearplane, float farplane, HDC chdc);
    ~Pipeline();
    unsigned char* render(HDC chdc);
    void pass();
    void shadow_pass();
    void clear_framebuffer(unsigned char* framebuffer, int init_value);
    void clear_buffer(float* zbuffer, int init_value);

    Model* model;

    float* zbuffer;
    float* shadowbuffer;
    unsigned char* framebuffer;

    mat<4, 4>  M_ModelView;
    mat<4, 4>  M_Perspective;
    mat<4, 4>  M_Ortho;
    mat<4, 4>  M_ViewPort;
    mat<4, 4>  M_ModelLight;

    float width;
    float height;

    vec3 light;
    vec3 lightdir;
    vec3 camera;
    vec3 center;
    vec3 up;

    float fovy;
    float nearplane;
    float farplane;

    HDC chdc;

};

