#include "tgaimage.h"
#include "model.h"
#include "my_gl.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

extern mat<4, 4>  ModelView;
extern mat<4, 4>  ViewPort;
extern mat<4, 4>  Projection;

Model* model = nullptr;
const int width = 1000;
const int height = 1000;

vec3 light = vec3(0, 0, 1).normalize();
const vec3 camera(1, 1, 3);
const vec3 center(0,0,0);
const vec3 up(0,1,0);


struct GouraudShader : public IShader
{
    vec3 vert_intensity;
	mat<2,3> vert_uv;
	//vertex shader
	virtual vec3 vertex(int iface, int nthvert,vec2& uv){
	   vert_intensity[nthvert] = std::max(0.f, float(model->normal(iface,nthvert)*light));
	   vec4 gl_Vertex = embed<4>(model->vert(iface,nthvert));
	   vert_uv.set_col(nthvert,model->uv(iface, nthvert));
	   gl_Vertex = ViewPort * Projection * ModelView * gl_Vertex;
	   return vec3(int(gl_Vertex[0]/gl_Vertex[3]), int(gl_Vertex[1]/gl_Vertex[3]),int(gl_Vertex[2]/gl_Vertex[3]));
	}

	//fragment shader(pixel shader)
	virtual bool fragment(vec3 bar, TGAColor& color){
	    float intensity = vert_intensity * bar;
		vec2 uv = vert_uv * bar;
		color = TGAColor(255, 255, 255) * intensity;  //model->diffuse(uv)* intensity;
		return false;
	}
};

struct PhongShading : public IShader
{
	mat<3, 3> vert_normal;
	mat<2, 3> vert_uv;
	//vertex shader
	virtual vec3 vertex(int iface, int nthvert, vec2& uv) {
		vert_normal.set_col(nthvert,model->normal(iface, nthvert));
		vec4 gl_Vertex = embed<4>(model->vert(iface, nthvert));
		vert_uv.set_col(nthvert, model->uv(iface, nthvert));
		gl_Vertex = ViewPort * Projection * ModelView * gl_Vertex;
		return vec3(int(gl_Vertex[0] / gl_Vertex[3]), int(gl_Vertex[1] / gl_Vertex[3]), int(gl_Vertex[2] / gl_Vertex[3]));
	}

	//fragment shader(pixel shader)
	virtual bool fragment(vec3 bar, TGAColor& color) {
		vec3 normal = vert_normal * bar;
		float intensity = std::max(0.,normal*light);
		vec2 uv = vert_uv * bar;
		color = TGAColor(255, 255, 255) * intensity; //model->diffuse(uv)* intensity;
		return false;
	}
};


int main(int argc, char** argv) {
	model = new Model("obj/african_head.obj");

	lookat(camera,center,up);
	viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
	projection(-1.f / (camera - center).norm());

	TGAImage image(width, height, TGAImage::RGB);
	TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);

	PhongShading shader;

	for (int i = 0; i < model->nfaces(); i++)
	{
		vec3 screen_coords[3];
		vec2 uvs[3];
		for (int j = 0; j < 3; j++)
		{
		    screen_coords[j] = shader.vertex(i,j,uvs[j]);
		}
		triangle(screen_coords,shader, image, zbuffer);
	}

	image.flip_vertically(); 
	zbuffer.flip_vertically();
	image.write_tga_file("output.tga");
	zbuffer.write_tga_file("zbuffer.tga");

	delete model;

	return 0;
}

