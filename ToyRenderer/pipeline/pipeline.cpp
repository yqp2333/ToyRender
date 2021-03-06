#include "pipeline.h"
#include"..\shader\shader.h"
#include"..\my_gl\my_gl.h"

mat<4,4> M_Model = { {
{ 1,       0,      0,      0.},
{ 0,       1,      0,      0.},
{ 0,       0,      1,      0,},
{ 0,       0,      0,      1,}
} };
mat<4, 4> M_Model_eye = { {
{ 1,       0,      0,      0.},
{ 0,       1,      0,      0},
{ 0,       0,      1,      0,},
{ 0,       0,      0,      1,}
} };

mat<4, 4> M_Model_Floor = { {
{ 2,       0,      0,      0.},
{ 0,       2,      0,      0.8},
{ 0,       0,      2,      0,},
{ 0,       0,      0,      1,}
} };

mat<4, 4> M_Model_diablo3 = { {
{ 1.2,       0,      0,      0.},
{ 0,       1.2,      0,      0.},
{ 0,       0,      1.2,      0,},
{ 0,       0,      0,      1,}
} };


void Pipeline::clear_buffer(float* zbuffer, int init_value)
{
	for (int i = 0; i < width * height; i++)
		zbuffer[i] = init_value;
}

void Pipeline::clear_framebuffer(unsigned char* framebuffer,int init_value)
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int index = (i * width + j) * 4;

			framebuffer[index + 2] = init_value;
			framebuffer[index + 1] = init_value;
			framebuffer[index] = init_value;
		}
	}
}

Pipeline::Pipeline(Camera& camera,const char* model_name, float width, float height, vec3 light, float fovy, float nearplane, float farplane, HDC chdc)
	:width(width),
	 height(height),
	 light(light),
	 camera(camera),
	 fovy(fovy),
	 nearplane(nearplane),
	 farplane(farplane),
	 chdc(chdc),
	 model_name(model_name)
{
     model_1_1 = new Model("obj/african_head.obj");
	 model_1_2 = new Model("obj/african_head_eye_inner.obj");
	 model_2 = new Model(model_name);
	 skybox = new Model("obj/skybox/box.obj",1);
	 floor = new Model("obj/floor.obj");

	 zbuffer = new float[width * height];
	 shadowbuffer = new float[width * height];
	 framebuffer = new unsigned char[width * height * 4];
	 lightdir = light.normalize();

}
Pipeline::~Pipeline()
{
	delete model_1_1;
	delete skybox;
	delete floor;
	delete zbuffer;
	delete framebuffer;
	delete shadowbuffer;
}

void Pipeline::pass(Model& model,mat<4,4>& M_Model)
{
	BlinnPhongShader shader(*this,model,M_Model);

	for (int i = 0; i < model.nfaces(); i++)
	{
		vec4 clip_verts[3];
		for (int j = 0; j < 3; j++)
		{
			clip_verts[j] = shader.vertex(i, j);
		}
		rasterize_triangle(clip_verts,shader, *this, zbuffer);
	}
}

void Pipeline::shadow_pass(Model& model,mat<4, 4>& M_Model)
{
	M_ModelLight = lookat(light,vec3(0,0,0),vec3(0,1,0));
	M_Ortho = ortho(-1,1,1,-1, -0.1, -10000);
	ShadowMapping shader(*this, model,M_Model);
	for (int i = 0; i < model.nfaces(); i++)
	{
		vec4 clip_verts[3];
		for (int j = 0; j < 3; j++)
		{
			clip_verts[j] = shader.vertex(i, j);
		}
		rasterize_triangle(clip_verts, shader, *this, shadowbuffer);
	}
}

void Pipeline::skybox_pass(Model& model)
{
	SkyBoxShader shader(*this, model, M_Model);

	for (int i = 0; i < model.nfaces(); i++)
	{
		vec4 clip_verts[3];
		for (int j = 0; j < 3; j++)
		{
			clip_verts[j] = shader.vertex(i, j);
		}
		rasterize_triangle(clip_verts, shader, *this, zbuffer,1);
	}
}

unsigned char* Pipeline::render(HDC chdc,int model_index, bool is_shadow, bool is_skybox){
	M_View = camera.get_M_View();
	M_Perspective = perspective(fovy, width / height, nearplane, farplane);
	M_ViewPort = viewport(width, height);

	clear_framebuffer(framebuffer, 50);
	clear_buffer(zbuffer, -100);
	clear_buffer(shadowbuffer, -100);

	if (model_index == 0)
	{
		if (is_shadow)
		{
			shadow_pass(*model_1_1, M_Model);
		}

		pass(*model_1_1, M_Model);
		pass(*model_1_2, M_Model_eye);

	}
	else if(model_index == 1)
	{
		if (is_shadow)
		{
			shadow_pass(*model_2, M_Model_diablo3);
		}
		pass(*model_2, M_Model_diablo3);

	}

	//if (is_skybox)
		//skybox_pass(*skybox);

	return framebuffer;
}
