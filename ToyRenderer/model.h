#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"
#include "tgaimage.h"

class Model {
private:
	std::vector<vec3> verts_;
	std::vector<vec2> uvs_;
	std::vector<std::vector<int> > faces_;
	TGAImage diffusemap_;
	void load_texture(std::string filename, const char* suffix, TGAImage& img);

public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	vec3 vert(int i);
	vec2 uv(int i);
	std::vector<int> face(int idx);
	TGAColor diffuse(vec2 uv);

};

#endif //__MODEL_H__
