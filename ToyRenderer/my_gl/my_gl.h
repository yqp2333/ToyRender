#pragma once
#include "..\math\geometry.h"
#include"..\model\model.h"
#include"..\model\tgaimage.h"
#include"..\shader\shader.h"
#include"..\pipeline\pipeline.h"
#include <windows.h>

mat<4, 4> lookat(const vec3 camera, const vec3 center, const vec3 up);
mat<4, 4> perspective(float fovy, float aspect, float n, float f);
mat<4, 4> ortho(float left, float right, float top, float bottom, float n, float f);
mat<4, 4> viewport(float width, float height);

vec3 barycentric(vec3* viewport_verts, vec3 p);
vec3 perspective_correct_interpolation(vec4* clip_verts, vec3 bar);
vec3 clip(vec4* clip_verts);

void set_color(unsigned char* framebuffer, int x, int y, unsigned char color[], float width, float height);

float smoothstep(float edge0, float edge1, float x);
TGAColor lerp(TGAColor a, TGAColor b, float value);

void rasterize_triangle(vec4* clip_verts, IShader& shader,Pipeline& pipline, float* zbuffe, bool is_skybox = 0 );