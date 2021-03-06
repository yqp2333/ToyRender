#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "..\math\geometry.h"
#include "tgaimage.h"

class Model {
private:
	std::vector<vec3> verts_;
	std::vector<vec2> uvs_;
	std::vector<vec3> normals_;
	std::vector<std::vector<vec3> > faces_;

	TGAImage diffusemap_;
	TGAImage normalmap_;
	TGAImage tangentNormalmap_;
	TGAImage specularmap_;
	TGAImage cubemap_[6];

	void load_texture(std::string filename, const char* suffix, TGAImage& img);

public:
	Model(const char *filename, bool skybox = 0);
	~Model();

	int nverts();
	int nfaces();

	vec3 vert(int iface, int nvert);
	vec2 uv(int iface, int nvert);
	vec3 normal(int iface, int nvert);
	std::vector<int> face(int idx);
	TGAColor diffuse(vec2 uv);
	vec3 normal(vec2 uv);
	vec3 tangent_normal(vec2 uv);
	double specular(vec2 uv);
	TGAColor cubemap(int index,vec2 uv);
};

#endif //__MODEL_H__
