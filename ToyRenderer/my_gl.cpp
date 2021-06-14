#include "my_gl.h"

mat<4, 4>  ModelView;
mat<4, 4>  ViewPort;
mat<4, 4>  Projection;
float depth = 2000.f;

void lookat(const vec3 camera, const vec3 center, const vec3 up) {
	//set new coordinate system
	vec3 z = (camera - center).normalize();
	vec3 x = cross(up, z).normalize();
	vec3 y = cross(z, x).normalize();

	mat<4, 4> Minv = { {

		{x.x,x.y,x.z,0},
		{y.x,y.y,y.z,0},
		{z.x,z.y,z.z,0},
		{  0,  0,  0,1}
	}
	};

	mat<4, 4> Tr = { {

		{1, 0, 0, -center.x},
		{0, 1, 0, -center.y},
		{0, 0, 1, -center.z},
		{0, 0, 0, 1}
	}
	};

	ModelView = Minv * Tr;

}

void viewport(int x, int y, int w, int h) {
	ViewPort = { {
	{     w / 2.,       0 ,       0,      x + w / 2.},
	{       0,      h / 2.,       0,      y + h / 2.},
	{       0,       0,         depth / 2.,        depth / 2.},
	{       0,       0,        0,           1}
	} };
}

void projection(double coeff) {
	{
		Projection = { {
		{1, 0, 0 , 0},
		{0, 1, 0 , 0},
		{0, 0, 1 , 0},
		{0, 0, coeff , 1},
		} };
	}
}

//barycentric
vec3 barycentric(vec3* pts, vec3 p) {
	vec3 x(pts[1][0]  - pts[0][0] , pts[2][0]  -  pts[0][0]  , pts[0][0] - p.x);//0 point A,1 point B, 2 point C.  x(vector AB.x,vector AC.x,vector PA.x)
	vec3 y(pts[1][1]  - pts[0][1] , pts[2][1]  -  pts[0][1] , pts[0][1] - p.y);
	vec3 u = cross(x, y);

	if (std::abs(u.z) < 1.) {
		return vec3(-1., 1., 1.);
	}
	return vec3(1.f - (u.x + u.y) / float(u.z), u.x / float(u.z), u.y / float(u.z));
}


void triangle(vec3* pts, IShader& shader, TGAImage& image, float* zbuffer, HDC hdc , bool isPaint)
{
	vec3 p;
	TGAColor color;

	//make boundrayBox to scale down the check area
	vec2 boundrayBoxMin(image.get_width() - 1., image.get_height() - 1.);
	vec2 boundrayBoxMax(0., 0.);
	vec2 clamp(image.get_width() - 1, image.get_height() - 1);
	for (int i = 0; i < 3; i++) {
		boundrayBoxMin.x = (std::max)(0., (std::min)(boundrayBoxMin.x, pts[i][0]));
		boundrayBoxMin.y = (std::max)(0., (std::min)(boundrayBoxMin.y, pts[i][1]));
		boundrayBoxMax.x = (std::min)(clamp.x, (std::max)(boundrayBoxMax.x, pts[i][0]));
		boundrayBoxMax.y = (std::min)(clamp.y, (std::max)(boundrayBoxMax.y, pts[i][1]));
	}
	// go through the pixel of boundrayBox
	for (p.x = boundrayBoxMin.x; p.x <= boundrayBoxMax.x; p.x++){
		for (p.y = boundrayBoxMin.y; p.y <= boundrayBoxMax.y; p.y++){
			// vertex barycentric
			vec3 barycentricP = barycentric(pts, p);
			vec2 uv(0,0);
			float z = 0;
			for (int i = 0; i < 3; i++)
			{
				//Using vertex barycentric lerp z_value
				z += pts[i][2] * barycentricP[i];
			}

			int frag_depth = z;

			if (barycentricP.x < 0 || barycentricP.y < 0 || barycentricP.z < 0 || zbuffer[int(p.x) + int(p.y)*image.get_width()] > frag_depth) continue;
			bool discard =  shader.fragment(barycentricP,color);

			if (!discard)
			{
			   zbuffer[int(p.x + p.y * image.get_width())] = frag_depth;
			   //image.set(p.x,p.y,color);
			   COLORREF caintColor = RGB(color.bgra[2],color.bgra[1],color.bgra[0]);
			   if (isPaint)
			   {
				   SetPixel(hdc, p.x, 1000 - p.y, caintColor);
			   }
			}

		}
	}
}

//Draw Line
void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color){

	bool steep = false;

	if (std::abs(x1 - x0) < std::abs(y1 - y0))
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	int dx = x1 - x0;
	int dy = y1 - y0;

	float gradient = std::abs(dy) * 2;
	float error = 0;
	int y = y0;

	for (int x = x0; x <= x1; x++)
	{
		if (steep) {
			image.set(y, x, color);
		}
		else
		{
			image.set(x, y, color);
		}
		error += gradient;
		if (error > dx)
		{
			y += (y1 > y0 ? 1 : -1);
			error -= dx * 2;
		}
	}
}

IShader::~IShader()
{
}
