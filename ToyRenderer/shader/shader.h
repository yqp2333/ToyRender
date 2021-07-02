#pragma once
#include "..\math\geometry.h"
#include "..\model\tgaimage.h"
#include "..\model\model.h"
#include "..\pipeline\pipeline.h"

class IShader
{
public:
    IShader();
    virtual ~IShader();
    virtual vec4 vertex(int num_face, int num_vert) = 0;
    virtual bool fragment(vec3 bar, TGAColor& color) = 0;
};

class  BlinnPhongShader : public IShader
{
    vec3 model_normal;
    mat<2, 3> uvs;
    mat<3, 3> model_normals;
    mat<3, 3> model_verts;
    mat<3, 3> light_verts;

    Pipeline& pipeline;
    
public:
    BlinnPhongShader();
    BlinnPhongShader(Pipeline& pipeline);

    vec4 vertex(int num_face, int num_vert);
    bool fragment(vec3 bar, TGAColor& color);

};

class ShadowMapping : public IShader
{
    mat<2, 3> uvs;
    mat<3, 3> light_verts;
    Pipeline& pipeline;
public:
    ShadowMapping(Pipeline& pipeline);
    ~ShadowMapping();
    vec4 vertex(int num_face, int num_vert);
    bool fragment(vec3 bar, TGAColor& color);

};

class SkyBoxShader : public IShader
{
    mat<2, 3> uvs;
    mat<3, 3> model_verts;
    Pipeline& pipeline;
public:
    SkyBoxShader(Pipeline& pipeline);
    ~SkyBoxShader();
    vec4 vertex(int num_face, int num_vert);
    bool fragment(vec3 bar, TGAColor& color);
};
