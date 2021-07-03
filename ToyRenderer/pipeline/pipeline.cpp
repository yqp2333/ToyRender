#include "pipeline.h"
#include"..\shader\shader.h"
#include"..\my_gl\my_gl.h"

mat<4,4> M_Model = { {
{ 1,       0,      0,      0.},
{ 0,       1,      0,      0.},
{ 0,       0,      1,      0,},
{ 0,       0,      0,      1,}
} };

mat<4, 4> M_Model_Floor = { {
{ 2,       0,      0,      0.},
{ 0,       2,      0,      0.8},
{ 0,       0,      2,      0,},
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
     model_1 = new Model(model_name);
	 model_2 = new Model("obj/african_head.obj");
	 skybox = new Model("obj/skybox/box.obj",1);
	 floor = new Model("obj/floor.obj");

	 zbuffer = new float[width * height];
	 shadowbuffer = new float[width * height];
	 framebuffer = new unsigned char[width * height * 4];
	 lightdir = light.normalize();

}
Pipeline::~Pipeline()
{
	delete model_1;
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
	M_Ortho = ortho(-2,2,2,-2, -0.1, -5.0);
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

unsigned char* Pipeline::render(HDC chdc){
	M_View = camera.get_M_View();
	M_Perspective = perspective(fovy, width / height, nearplane, farplane);
	M_ViewPort = viewport(width, height);

	clear_framebuffer(framebuffer,100.);
    clear_buffer(zbuffer,-1);
	clear_buffer(shadowbuffer, -1);

	//shadow_pass(*model_1);
	//shadow_pass(*floor);

	pass(*model_2, M_Model);
	//pass(*floor, M_Model_Floor);


	//skybox_pass(*skybox);
	return framebuffer;
}

unsigned char* Pipeline::render_2(HDC chdc)
{
	M_View = camera.get_M_View();
	M_Perspective = perspective(fovy, width / height, nearplane, farplane);
	M_ViewPort = viewport(width, height);

	clear_framebuffer(framebuffer, 100.);
	clear_buffer(zbuffer, -1);
	clear_buffer(shadowbuffer, -1);

	//shadow_pass(*model_1);
	//shadow_pass(*floor);

	pass(*model_1, M_Model);
	//pass(*floor, M_Model_Floor);


	//skybox_pass(*skybox);
	return framebuffer;
}
