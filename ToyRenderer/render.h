#pragma once
#include "model.h"
#include "my_gl.h"
#include "render.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

extern mat<4, 4>  ModelView;
extern mat<4, 4>  ViewPort;
extern mat<4, 4>  Projection;

Model* model = nullptr;
TGAImage image;
TGAImage zbuffer;
const int width = 1000;
const int height = 1000;

vec3 light = vec3(1, 1, 1).normalize();
const vec3 camera(1, 1, 3);
const vec3 center(0, 0, 0);
const vec3 up(0, 1, 0);


struct TangentNormalShader : public IShader
{
	mat<3, 3> vert;
	mat<2, 3> vert_uv;
	mat<3, 3> vert_normal;
	mat<4, 4> uniform_M;
	mat<4, 4> uniform_MIT;

	//vertex shader
	virtual vec3 vertex(int iface, int nthvert, vec2& uv) {
		vec4 gl_Vertex = embed<4>(model->vert(iface, nthvert));//get vertex 
		vert_normal.set_col(nthvert, proj<3>(uniform_MIT * embed<4>(model->normal(iface, nthvert).normalize(),.0f)));//get model`s normal
		vert_uv.set_col(nthvert, model->uv(iface, nthvert));//get uv
		gl_Vertex = ViewPort * Projection * ModelView * gl_Vertex;//mvp
		vert.set_col(nthvert,vec3(int(gl_Vertex[0] / gl_Vertex[3]), int(gl_Vertex[1] / gl_Vertex[3]), int(gl_Vertex[2] / gl_Vertex[3])));//set the vertex after mvp tansformation 
		return vec3(int(gl_Vertex[0] / gl_Vertex[3]), int(gl_Vertex[1] / gl_Vertex[3]), int(gl_Vertex[2] / gl_Vertex[3]));
	}

	//fragment shader(pixel shader)
	virtual bool fragment(vec3 bar, TGAColor& color) {
		vec2 uv = vert_uv * bar;
		vec3 normal = (vert_normal * bar).normalize();

	//---------------------tangent space normal mapping--------------------------------------------
		///calculation TBN Mri
		vec3 edge1 = vert.col(1) - vert.col(0);
		vec3 edge2 = vert.col(2) - vert.col(0);

		vec2 delta_uv1 = vert_uv.col(1) - vert_uv.col(0);
		vec2 delta_uv2 = vert_uv.col(2) - vert_uv.col(0);

		float a = 1/(delta_uv1.x*delta_uv2.y - delta_uv2.x * delta_uv1.y);

		mat<2,2> tri_a = { {

		{delta_uv2.y, -delta_uv2.x},
		{-delta_uv1.y, delta_uv1.x},
		}};

		mat<3,2> e1e2;
		e1e2.set_col(0,edge1);
		e1e2.set_col(1,edge2);

		mat<3,2> TB = (e1e2 * tri_a) * a;

		mat<3,3> TBN;

		TBN.set_col(0,TB.transpose()[0].normalize());
		TBN.set_col(1, TB.transpose()[1].normalize());
		TBN.set_col(2,normal);

		//calculation tangent_normal;
		vec3 normal_new = (TBN * model->tangent_normal(uv)).normalize();

	//------------------------------------------------------------------------------

		vec3 light_new = proj<3>(uniform_M * embed<4>(light)).normalize();
		vec3 half_way_vector = (normal_new + light_new).normalize();
		float diffusely_reflection = (std::max)(0., light_new * normal_new);
		float ambient_lighting = 0.5;
		float specular_highlights = pow((std::max)(0., half_way_vector * light_new), model->specular(uv));
		TGAColor c = model->diffuse(uv);
		color = c;
		for (int i = 0; i < 3; i++)
		{
			color[i] = std::min<float>(5 + c[i] * (diffusely_reflection + 0.4 * specular_highlights), 255);
		}
		return false;
	}
};

struct BullinPhongShader : public IShader
{
	mat<2, 3> vert_uv;
	mat<4, 4> uniform_M;
	mat<4, 4> uniform_MIT;
	//vertex shader
	virtual vec3 vertex(int iface, int nthvert, vec2& uv) {
		vec4 gl_Vertex = embed<4>(model->vert(iface, nthvert));
		vert_uv.set_col(nthvert, model->uv(iface, nthvert));
		gl_Vertex = ViewPort * Projection * ModelView *  gl_Vertex;
		return vec3(int(gl_Vertex[0] / gl_Vertex[3]), int(gl_Vertex[1] / gl_Vertex[3]), int(gl_Vertex[2] / gl_Vertex[3]));
	}

	//fragment shader(pixel shader)
	virtual bool fragment(vec3 bar, TGAColor& color) {
		vec2 uv = vert_uv * bar;
		vec3 normal_new = proj<3>(uniform_MIT * embed<4>(model->normal(uv))).normalize();// use Invertible matrix to transformation normal
		vec3 light_new = proj<3>(uniform_M * embed<4>(light)).normalize();
		vec3 half_way_vector = (normal_new + light_new).normalize();
		float diffusely_reflection = (std::max)(0., light_new * normal_new);
		float ambient_lighting = 0.5;
		float specular_highlights = pow((std::max)(0.,half_way_vector*light_new), model->specular(uv));
		TGAColor c = model->diffuse(uv);
		color = c;
		for (int i = 0; i < 3; i++)
		{
			color[i] = std::min<float>(0.5 + c[i]*(diffusely_reflection + 0.7*specular_highlights),255);
		}
		return false;
	}
};

struct NormalMapShader : public IShader
{
	mat<2, 3> vert_uv;
	mat<4,4> uniform_M; 
	mat<4,4> uniform_MIT;
	//vertex shader
	virtual vec3 vertex(int iface, int nthvert, vec2& uv) {
		vec4 gl_Vertex = embed<4>(model->vert(iface, nthvert));
		vert_uv.set_col(nthvert, model->uv(iface, nthvert));
		gl_Vertex = ViewPort * Projection * ModelView * gl_Vertex;
		return vec3(int(gl_Vertex[0] / gl_Vertex[3]), int(gl_Vertex[1] / gl_Vertex[3]), int(gl_Vertex[2] / gl_Vertex[3]));
	}

	//fragment shader(pixel shader)
	virtual bool fragment(vec3 bar, TGAColor& color) {
		vec2 uv = vert_uv * bar;
		vec3 normal_new  = proj<3>(uniform_MIT * embed<4>(model->normal(uv))).normalize();// use Invertible matrix to transformation normal
		vec3 light_new = proj<3>(uniform_M * embed<4>(light)).normalize();
		float intensity = (std::max)(0., light_new * normal_new);
		color = model->diffuse(uv) * intensity;// TGAColor(255, 255, 255) * intensity;
		return false;
	}
};

struct GouraudShader : public IShader
{
	vec3 vert_intensity;
	mat<2, 3> vert_uv;
	//vertex shader
	virtual vec3 vertex(int iface, int nthvert, vec2& uv) {
		vert_intensity[nthvert] = (std::max)(0.f, float(model->normal(iface, nthvert) * light));
		vec4 gl_Vertex = embed<4>(model->vert(iface, nthvert));
		vert_uv.set_col(nthvert, model->uv(iface, nthvert));
		gl_Vertex = ViewPort * Projection * ModelView * gl_Vertex;
		return vec3(int(gl_Vertex[0] / gl_Vertex[3]), int(gl_Vertex[1] / gl_Vertex[3]), int(gl_Vertex[2] / gl_Vertex[3]));
	}

	//fragment shader(pixel shader)
	virtual bool fragment(vec3 bar, TGAColor& color) {
		float intensity = vert_intensity * bar;
		vec2 uv = vert_uv * bar;
		color = model->diffuse(uv) * intensity;// TGAColor(255, 255, 255) * intensity;
		return false;
	}
};


struct PhongShading : public IShader
{
	mat<3, 3> vert_normal;
	mat<2, 3> vert_uv;
	//vertex shader
	virtual vec3 vertex(int iface, int nthvert, vec2& uv) {
		vert_normal.set_col(nthvert, model->normal(iface, nthvert));
		vec4 gl_Vertex = embed<4>(model->vert(iface, nthvert));
		vert_uv.set_col(nthvert, model->uv(iface, nthvert));
		gl_Vertex = ViewPort * Projection * ModelView * gl_Vertex;
		return vec3(int(gl_Vertex[0] / gl_Vertex[3]), int(gl_Vertex[1] / gl_Vertex[3]), int(gl_Vertex[2] / gl_Vertex[3]));
	}

	//fragment shader(pixel shader)
	virtual bool fragment(vec3 bar, TGAColor& color) {
		vec3 normal = vert_normal * bar;
		float intensity = (std::max)(0., normal * light);
		vec2 uv = vert_uv * bar;
		color = model->diffuse(uv) * intensity;// TGAColor(255, 255, 255) * intensity;
		return false;
	}
};

void initRenderInfo(){
	model = new Model("obj/african_head.obj");
	lookat(camera, center, up);
	viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
	projection(-1.f / (camera - center).norm());

	image = TGAImage(width, height, TGAImage::RGB);
	zbuffer = TGAImage(width, height, TGAImage::GRAYSCALE);
}

void render(HDC chdc){

	//‰÷»æ
	TangentNormalShader shader;
	shader.uniform_M = Projection * ModelView;
	shader.uniform_MIT = (Projection * ModelView).invert_transpose();
	for (int i = 0; i < model->nfaces(); i++)
	{
		vec3 screen_coords[3];
		vec2 uvs[3];
		for (int j = 0; j < 3; j++)
		{
			screen_coords[j] = shader.vertex(i, j, uvs[j]);
		}
		triangle(screen_coords, shader, image, zbuffer, chdc);
	}

}

void saveTGAImage(){
	image.flip_vertically();
	zbuffer.flip_vertically();
	image.write_tga_file("output.tga");
	zbuffer.write_tga_file("zbuffer.tga");
}