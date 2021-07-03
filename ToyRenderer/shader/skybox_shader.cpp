#include"shader.h"
#include "..\my_gl\my_gl.h"

int cubemap_index(vec3 direction, vec2& uv);


SkyBoxShader::SkyBoxShader(Pipeline& pipeline, Model& model,mat<4, 4>& M_Model)
	:pipeline(pipeline),
 	 model(model),
	 M_Model(M_Model)
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
	vec4 clip_vert = pipeline.M_Perspective * M_View * modle_vert;
	model_verts.set_col(num_vert, proj<3>(modle_vert));
	return clip_vert;
}

bool SkyBoxShader::fragment(vec3 bar, TGAColor& color, vec3* viewport_verts)
{
	vec3 model_pos = model_verts *  bar;
	vec2 uv;
	int index = cubemap_index(model_pos.normalize(),uv);
	color = pipeline.skybox->cubemap(index, uv); 
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
		base_value = x;
		if (direction.x>0)//right 
		{
		    index = 0;
			uv_1 = -direction.z;
			uv_2 = -direction.y;
		}
		else//left
		{
			index = 1;
			uv_1 = direction.z;
			uv_2 = -direction.y;
		}
	}
	else if (y>z)
	{
		base_value = y;
		if (direction.y > 0)//top
		{
			index = 3;
			
			uv_1 = +direction.x;
			uv_2 = +direction.z;
		}
		else//bottom
		{
			index = 2;
			uv_1 = +direction.x;
			uv_2 = -direction.z;
		}
	}
	else
	{
		base_value = z;
		if (direction.z > 0)//back
		{
			index = 4;
			uv_1 = +direction.x;
			uv_2 = -direction.y;
		}
		else//front
		{
			index = 5;
			uv_1 = -direction.x;
			uv_2 = -direction.y;
		}
	}
	uv.x = (uv_1/ base_value + 1.0f)/2.f;
	uv.y = (uv_2/ base_value + 1.0f)/2.f;
	return index;
}