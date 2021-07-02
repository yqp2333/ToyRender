#include"shader.h"
#include "..\my_gl\my_gl.h"

BlinnPhongShader::BlinnPhongShader(Pipeline& pipeline)
    :pipeline(pipeline)
	 {}

vec4 BlinnPhongShader::vertex(int num_face, int num_vert)
{
	vec4 modle_vert = embed<4>(pipeline.model->vert(num_face, num_vert));
	vec4 clip_vert = pipeline.M_Perspective * pipeline.M_View * pipeline.M_Model* modle_vert;

	model_verts.set_col(num_vert, pipeline.model->vert(num_face, num_vert));
	model_normals.set_col(num_vert, pipeline.model->normal(num_face, num_vert));
	uvs.set_col(num_vert, pipeline.model->uv(num_face, num_vert));

	//change the viet to light`s view space
	vec4 light_vert = pipeline.M_Ortho * pipeline.M_ModelLight * pipeline.M_Model * embed<4>(pipeline.model->vert(num_face, num_vert));
	light_vert = light_vert / light_vert[3];
	light_vert = pipeline.M_ViewPort * embed<4>(light_vert);
	light_verts.set_col(num_vert,proj<3>(light_vert) );

	return clip_vert;
}

bool BlinnPhongShader::fragment(vec3 bar, TGAColor& color)
{
	TGAColor light_color = TGAColor(255, 255, 255);
	vec2 uv = uvs * bar;
	vec3 normal = model_normals * bar;//用于计算切线空间

    //---------------------tangent space normal mapping--------------------------------------------
	vec3 edge1 = model_verts.col(1) - model_verts.col(0);
	vec3 edge2 = model_verts.col(2) - model_verts.col(0);

	vec2 delta_uv1 = uvs.col(1) - uvs.col(0);
	vec2 delta_uv2 = uvs.col(2) - uvs.col(0);

	float a = 1 / (delta_uv1.x * delta_uv2.y - delta_uv2.x * delta_uv1.y);

	mat<2, 2> M_a = { {

	{delta_uv2.y, -delta_uv2.x},
	{-delta_uv1.y, delta_uv1.x},
	} };

	mat<3, 2> e1e2;
	e1e2.set_col(0, edge1);
	e1e2.set_col(1, edge2);

	mat<3, 2> M_TB = (e1e2 * M_a) * a;

	mat<3, 3> M_Model_TBN;

	M_Model_TBN.set_col(0, M_TB.transpose()[0].normalize());
	M_Model_TBN.set_col(1, M_TB.transpose()[1].normalize());
	M_Model_TBN.set_col(2, normal);

	//calculatie tangent_normal and transform to view space;
	vec3 model_tan_normal = M_Model_TBN * pipeline.model->tangent_normal(uv).normalize();
	vec3 view_normal = proj<3>((pipeline.M_View * pipeline.M_Model).invert_transpose() * embed<4>(model_tan_normal,0)).normalize();

	//vec3 view_normal = proj<3>(M_ModelView.invert_transpose() * embed<4>(model->normal(uv),0)).normalize();//法线和光变化到同一空间中进行光照计算,从世界空间法线贴图读取。
	vec3 view_lightdir = proj<3>((pipeline.M_View * pipeline.M_Model) * embed<4>(pipeline.lightdir,0)).normalize();//灯光如果是平行光，则不应该有平移变换。法线同理，所以变为齐次坐标时，w=0；
	vec3 half_vector = (view_normal + view_lightdir).normalize();

	////shadow
	//vec3 light_vert = light_verts * bar;
	//int index = int(light_vert[0])+ int(light_vert[1] * pipeline.width);
	//float shadow_intensity;

	//shadow_intensity = 0.3 + 0.7 * (pipeline.shadowbuffer[index] < light_vert[2]);

	TGAColor ambient;
	float ka = 0.07;
	ambient = light_color * ka;

	TGAColor diffuse ;//lambert model
	TGAColor kd = pipeline.model->diffuse(uv) * 0.8;
	diffuse = kd * (std::max)(0., view_lightdir * view_normal);

	TGAColor specular = TGAColor(100, 100, 100);
	float ks = 0.3;
	float shininess = pipeline.model->specular(uv);
	specular = specular * pow((std::max)(0., half_vector * view_normal), shininess) * ks;

	color = (ambient + diffuse + specular) * light_color;//* shadow_intensity; //TGAColor(255, 255, 255) * (light_vert[2]*0.5+0.5);//
	return false;
}

IShader::IShader()
{
}

IShader::~IShader()
{
}


ShadowMapping::ShadowMapping(Pipeline& pipeline)
	:pipeline(pipeline)
{}


ShadowMapping::~ShadowMapping()
{
}

vec4 ShadowMapping::vertex(int num_face, int num_vert)
{   
	vec4 modle_vert = embed<4>(pipeline.model->vert(num_face, num_vert));
	vec4 clip_vert = pipeline.M_Ortho * pipeline.M_ModelLight * modle_vert;

	vec4 light_vert = clip_vert / clip_vert[3];
	//light_vert = pipeline.M_ViewPort * embed<4>(light_vert);
	light_verts.set_col(num_vert, proj<3>(light_vert));

	uvs.set_col(num_vert, pipeline.model->uv(num_face, num_vert));
	return clip_vert;
}

bool ShadowMapping::fragment(vec3 bar, TGAColor& color)
{
	vec2 uv = uvs * bar; 
	vec3 light_vert = light_verts * bar;
	float z = (light_vert.z)* 0.5 + 0.5;

	color = TGAColor(255, 255, 255) * z;
	return true;
}

