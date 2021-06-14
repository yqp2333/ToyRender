#pragma once
#include"tgaimage.h"
#include"geometry.h"
#include"model.h"
#include <windows.h>
//transformation Matrix

void lookat(const vec3 camera, const vec3 center, const vec3 up);
void viewport(int x, int y, int w, int h);
void projection(double coeff);

struct IShader
{
	virtual ~IShader();
	bool isDepthShader;
	virtual vec3 vertex(int iface, int nthvert, vec2& uv) = 0;
	virtual bool fragment(vec3 bar, TGAColor &color) = 0;
};

void triangle(vec3* pts,IShader &shader, TGAImage& image, float* zbuffer,HDC hdc, bool isPaint =1);
void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color);