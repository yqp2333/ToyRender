#include "my_gl.h"

mat<4, 4> lookat(const vec3 camera, const vec3 center, const vec3 up) {
	vec3 z = (camera - center).normalize();
	vec3 x = cross(up, z).normalize();
	vec3 y = cross(z, x).normalize();

	mat<4, 4> MI_Rotate = { {

		{x.x,x.y,x.z,0},
		{y.x,y.y,y.z,0},
		{z.x,z.y,z.z,0},
		{  0,  0,  0,1}
	}
	};

	mat<4, 4> MI_Translation = { {

		{1, 0, 0, -camera.x},
		{0, 1, 0, -camera.y},
		{0, 0, 1, -camera.z},
		{0, 0, 0, 1}
	}
	};

	return MI_Rotate * MI_Translation;

}

mat<4, 4> viewport(float width, float height) {
	mat<4, 4>  M_ViewPort = { {

	{ width/2.,          0 ,     0,   width/ 2.},
	{        0,    height/2.,    0,   height/2.},
	{        0,           0,     1,         0},
	{        0,           0,     0,          1 }

	}};

	return M_ViewPort;
}

mat<4, 4> perspective(float fovy, float aspect, float n, float f) {
	
	fovy = fovy / 180 * 3.1415926;
	float t = std::fabs(n) * tan(fovy * 0.5);
	float r = aspect * t;

	mat<4, 4>  M_Perspective = { {
	{n/r, 0,  0,  0},
	{0, n/t,  0,  0},
	{0,   0, (f+n)/(n-f), 2*f*n/(f-n)},
	{0,   0,  1,  0},
	} };

	return M_Perspective;

}

mat<4, 4> ortho(float left, float right,float top, float bottom, float n, float f)
{
    float x_range = right - left;
	float y_range = top-bottom;
	float z_range = n - f;

	mat<4, 4> M_Ortho = { {
	{2/x_range, 0, 0 , -(left + right)/x_range},
	{0, 2/y_range, 0, -(bottom + top)/y_range},
	{0, 0, 2 / z_range, -(n+f)/ z_range},
	{0, 0, 0 , 1},
	} };
	return M_Ortho;
}


TGAColor lerp(TGAColor a, TGAColor b, float value)
{
	TGAColor color = a;
	color.bgra[0] += (b.bgra[0] - a.bgra[0])* value;
	color.bgra[1] += (b.bgra[1] - a.bgra[1]) * value;
	color.bgra[2] += (b.bgra[2] - a.bgra[2]) * value;

	return color;
}


float smoothstep(float edge0, float edge1, float x)
{
    x = (std::min)((std::max)(0.0f,(x - edge0) / (edge1 - edge0)), 1.0f);
	return x;
}

//barycentric
vec3 barycentric(vec3* viewport_verts, vec3 p) {
	vec3 x(viewport_verts[1][0]  - viewport_verts[0][0] , viewport_verts[2][0]  - viewport_verts[0][0]  , viewport_verts[0][0] - p.x);//0 point A,1 point B, 2 point C.  x(vector AB.x,vector AC.x,vector PA.x)
	vec3 y(viewport_verts[1][1]  - viewport_verts[0][1] , viewport_verts[2][1]  - viewport_verts[0][1] , viewport_verts[0][1] - p.y);
	vec3 u = cross(x, y);

	if (std::abs(u.z) < 1.) {
		return vec3(-1., 1., 1.);
	}
	return vec3(1.f - (u.x + u.y) / float(u.z), u.x / float(u.z), u.y / float(u.z));
}


vec3 perspective_correct_interpolation(vec4* clip_verts, vec3 bar)
{
	float correct_inter_z = 1.0 / (bar.x / (clip_verts[0][3]) + bar.y / (clip_verts[1][3]) + bar.z / (clip_verts[2][3]));
	vec3 correct_bar;
	for (int i = 0; i < 3; i++)
	{
		correct_bar[i] = (bar[i] / clip_verts[i][3]) * correct_inter_z;
	}
	return correct_bar;
}


vec3 clip(vec4* clip_verts)
{
	return vec3();
}

bool face_culling(vec3* ndc_verts,bool is_front = 0)
{
	//逆时针 正向
    vec3 edge1 = vec3(ndc_verts[1].x - ndc_verts[0].x, ndc_verts[1].y - ndc_verts[0].y, ndc_verts[1].z - ndc_verts[0].z);
	vec3 edge2 = vec3(ndc_verts[2].x - ndc_verts[0].x, ndc_verts[2].y - ndc_verts[0].y, ndc_verts[2].z - ndc_verts[0].z);
	vec3 normal = cross(edge2,edge1).normalize();
	vec3 view =vec3(0,0,-1);
	if (is_front)
	{
	return normal * view > 0;  //front cull
	}
	return normal * view < 0;  //back cull
}


static void set_color(unsigned char* framebuffer, int x, int y, unsigned char color[],float width,float height)
{
	int i;
	int index = ((height - y - 1) * width + x) * 4; // the origin for pixel is bottom-left, but the framebuffer index counts from top-left

	for (i = 0; i < 3; i++)
		framebuffer[index + i] = color[i];
}


void rasterize_triangle(vec4* clip_verts, IShader& shader,Pipeline& pipline,float* zbuffer,bool is_skybox)
{
	vec3 p;
	vec3 ndc_verts[3];
	vec3 view_verts[3];
	TGAColor color;

	//perspective division
	for (int i = 0; i < 3; i++)
	{
		ndc_verts[i] = vec3(clip_verts[i][0]/ clip_verts[i][3], clip_verts[i][1] / clip_verts[i][3], clip_verts[i][2] / clip_verts[i][3]);
	}

	 
	//face culling
	if (!is_skybox)
	{
		if (face_culling(ndc_verts))//back face culling
		{
			 return;
		}
	}
	else
	{
		if (face_culling(ndc_verts,1))
		{
			//return;
		}
	}

	//viewport transformation
 	for (int i = 0; i < 3; i++)
	{
		view_verts[i] = proj<3>(pipline.M_ViewPort * embed<4>(ndc_verts[i]));
		view_verts[i][0] = int(view_verts[i][0]);//x、y一定要取整，要不然生成的图会有缝隙
		view_verts[i][1] = int(view_verts[i][1]);
	}

	//boundrayBox check
	vec2 boundrayBoxMin(pipline.width - 1., pipline.height - 1.);
	vec2 boundrayBoxMax(0., 0.);
	vec2 clamp(pipline.width - 1., pipline.height - 1.);
	for (int i = 0; i < 3; i++) {
		boundrayBoxMin.x = (std::max)(0., (std::min)(boundrayBoxMin.x, view_verts[i][0]));
		boundrayBoxMin.y = (std::max)(0., (std::min)(boundrayBoxMin.y, view_verts[i][1]));
		boundrayBoxMax.x = (std::min)(clamp.x, (std::max)(boundrayBoxMax.x, view_verts[i][0]));
		boundrayBoxMax.y = (std::min)(clamp.y, (std::max)(boundrayBoxMax.y, view_verts[i][1]));
	}

	// go through the pixel of boundrayBox
    for (p.x = boundrayBoxMin.x; p.x <= boundrayBoxMax.x; p.x++){
		for (p.y = boundrayBoxMin.y; p.y <= boundrayBoxMax.y; p.y++){
			// vertex barycentric
  //for(p.x = 0;p.x<= pipline.width;p.x++){
	 //for ( p.y = 0; p.y < pipline.height; p.y++){

			vec3 bar = barycentric(view_verts, p);
			bar = perspective_correct_interpolation(clip_verts,bar);//correct interpolation

			float z = -1;

			if (is_skybox)
			{
				z = -100;//set the z of skybox to max z;
			}
			else
			{
				for (int i = 0; i < 3; i++)
				{
					//Using vertex barycentric lerp z_value
					z += view_verts[i][2] * bar[i];
				}
			}

			float frag_depth = z;//深度一定要是float才能进行深度测试。（如果是整数都重合在一起了）

			if (bar.x < 0 || bar.y < 0 || bar.z < 0 || zbuffer[int(p.x) + int(p.y * pipline.width)] > frag_depth) continue;

			bool discard =  shader.fragment(bar, color);

			//if (!is_skybox)//如果是天空盒，就不写入深度
			//{
				zbuffer[int(p.x + p.y * pipline.width)] = frag_depth;
			//}

			if (!discard)
			{
			   unsigned char c[3];
			   c[0] = color.bgra[2];
			   c[1] = color.bgra[1];
			   c[2] = color.bgra[0];

			   set_color(pipline.framebuffer, p.x, p.y, c, pipline.width, pipline.height);
			}
		}
	}
}

