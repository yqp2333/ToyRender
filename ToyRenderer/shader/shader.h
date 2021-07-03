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
    virtual bool fragment(vec3 bar, TGAColor& color, vec3*  viewport_verts) = 0;
};

class  BlinnPhongShader : public IShader
{
    vec3 model_normal;
    mat<2, 3> uvs;
    mat<3, 3> model_normals;
    mat<3, 3> model_verts;
    mat<3, 3> viewport_verts;
    

    Pipeline& pipeline;
    Model& model;
    mat<4, 4>& M_Model;

public:
    BlinnPhongShader();
    BlinnPhongShader(Pipeline& pipeline,Model& model,mat<4,4>& M_Model);

    vec4 vertex(int num_face, int num_vert);
    bool fragment(vec3 bar, TGAColor& color, vec3* viewport_verts);
};

class ShadowMapping : public IShader
{
    mat<2, 3> uvs;
    mat<3, 3> light_verts;
    Pipeline& pipeline;
    Model& model;
    mat<4, 4>& M_Model;
public:
    ShadowMapping(Pipeline& pipeline, Model& model, mat<4, 4>& M_Model);
    ~ShadowMapping();
    vec4 vertex(int num_face, int num_vert);
    bool fragment(vec3 bar, TGAColor& color, vec3* viewport_verts);
};

class SkyBoxShader : public IShader
{
    mat<2, 3> uvs;
    mat<3, 3> model_verts;
    Pipeline& pipeline;
    Model& model;
    mat<4, 4>& M_Model;
public:
    SkyBoxShader(Pipeline& pipeline, Model& model, mat<4, 4>& M_Model);
    ~SkyBoxShader();
    vec4 vertex(int num_face, int num_vert);
    bool fragment(vec3 bar, TGAColor& color, vec3* viewport_verts);
};
