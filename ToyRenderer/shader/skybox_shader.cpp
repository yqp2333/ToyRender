#include"shader.h"
#include "..\my_gl\my_gl.h"

int cubemap_index(vec3 direction, vec2& uv);

SkyBoxShader::SkyBoxShader(Pipeline& pipeline)
	:pipeline(pipeline)
{}

SkyBoxShader::~SkyBoxShader()
{
}

vec4 SkyBoxShader::vertex(int num_face, int num_vert)
{   mat<4,4>M_View = pipeline.M_View;
    M_View.rows[0][3] = 0;
	M_View.rows[1][3] = 0;
	M_View.rows[2][3] = 0;

	vec4 modle_vert = embed<4>(pipeline.skybox->vert(num_face,num_vert));
	vec4 clip_vert = pipeline.M_Perspective * M_View* modle_vert; //pipeline.M_Perspective* 

	//vec3 ndc_vert = proj<3>(clip_vert/ clip_vert[3]);
	model_verts.set_col(num_vert, proj<3>(modle_vert));
	return clip_vert;
}

bool SkyBoxShader::fragment(vec3 bar, TGAColor& color)
{
	vec3 model_pos = model_verts *  bar;
	vec2 uv;
	int index = cubemap_index(model_pos.normalize(),uv);
	color = pipeline.skybox->cubemap(index, uv); //TGAColor(255, 255, 255); //pipeline.skybox->cubemap(index,uv); ////
	return false;
}

int cubemap_index(vec3 direction, vec2& uv){
    int index = -1;
	float uv_1 = 0;
	float uv_2 = 0;
	float base_value = 0;

    float x = std::fabs(direction.x);
	float y = std::fabs(direction.y);
	float z = std::fabs(direction.z);
	if (x>y && x>z)
	{
		if (direction.x>0)//right 
		{
		    index = 4;
			base_value = direction.x;
			uv_1+= direction.z;
			uv_2+= direction.y;
		}
		else//left
		{
			index = 5;
			base_value = direction.x;
			uv_1 += direction.z;
			uv_2 -= direction.y;
		}
	}
	else if (y>z)
	{
		if (direction.y > 0)//top
		{
			index = 2;
			base_value = direction.y;
			uv_1 -= direction.x;
			uv_2 -= direction.z;
		}
		else//bottom
		{
			index = 3;
			base_value = direction.y;
			uv_1 += direction.x;
			uv_2 -= direction.z;
		}
	}
	else
	{
		if (direction.z > 0)//front
		{
			index = 0;
			base_value = direction.z;
			uv_1 -= direction.x;
			uv_2 += direction.y;
		}
		else//back
		{
			index = 1;
			base_value = direction.z;
			uv_1 -= direction.x;
			uv_2 -= direction.y;
		}
	}
	uv.x = (uv_1/base_value + 1.0f)/2.0f;
	uv.y = (uv_2/base_value + 1.0f)/2.0f;
	return index;
}