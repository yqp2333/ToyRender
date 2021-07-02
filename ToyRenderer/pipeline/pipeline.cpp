#include "pipeline.h"
#include"..\shader\shader.h"
#include"..\my_gl\my_gl.h"


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
	 light(lightdir),
	 camera(camera),
	 fovy(fovy),
	 nearplane(nearplane),
	 farplane(farplane),
	 chdc(chdc),
	 center(vec3(0.0,0.0,0.0)),
	 up(vec3(0.0,1.0,0.0))
{
	 model = new Model(model_name);
	 skybox = new Model("obj/skybox/box.obj",1);
	 zbuffer = new float[width * height];
	 shadowbuffer = new float[width * height];
	 framebuffer = new unsigned char[width * height * 4];
	 lightdir = light.normalize();
	 M_Model = {{
	 { 2,       0,      0,      0.},
	 { 0,       2,      0,      0.},
	 { 0,       0,      2,      0,},
	 { 0,       0,      0,      1,}
     }};
}
Pipeline::~Pipeline()
{
	//delete model;
	//delete zbuffer;
	//delete framebuffer;
	//delete shadowbuffer;
}

void Pipeline::pass()
{
	M_View = camera.get_M_View();
	M_Perspective = perspective(fovy, width / height, nearplane, farplane); 
	M_ViewPort = viewport(width, height);
	BlinnPhongShader shader(*this);

	for (int i = 0; i < model->nfaces(); i++)
	{
		vec4 clip_verts[3];
		for (int j = 0; j < 3; j++)
		{
			clip_verts[j] = shader.vertex(i, j);
		}
		rasterize_triangle(clip_verts,shader, *this, zbuffer);
	}
}

void Pipeline::shadow_pass()
{
    vec3 light_point = vec3(2,2,1);
	M_ModelLight = lookat(light_point, center, up);
	M_Ortho = ortho(-1,1,1,-1, -0.1, -5.0);
	M_Perspective = perspective(fovy, width / height, nearplane, farplane);
	M_ViewPort = viewport(width, height);

	ShadowMapping shader(*this);

	for (int i = 0; i < model->nfaces(); i++)
	{
		vec4 clip_verts[3];
		for (int j = 0; j < 3; j++)
		{
			clip_verts[j] = shader.vertex(i, j);
		}
		rasterize_triangle(clip_verts, shader, *this, shadowbuffer);
	}
}

void Pipeline::skybox_pass()
{
	M_View = camera.get_M_View();
	M_Perspective = perspective(fovy, width / height, nearplane, farplane);
	M_ViewPort = viewport(width, height);
	SkyBoxShader shader(*this);

	for (int i = 0; i < skybox->nfaces(); i++)
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
	clear_framebuffer(framebuffer,100);
    clear_buffer(zbuffer,-100);
	clear_buffer(shadowbuffer,-1);
	pass();
	//skybox_pass();
	//shadow_pass();

	return framebuffer;
}