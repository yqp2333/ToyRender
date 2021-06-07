#include "tgaimage.h"
#include "model.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const int width = 800;
const int height = 800;

//Draw Line
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color){
    
	bool steep = false;

	if (std::abs(x1-x0)< std::abs(y1-y0))
	{
	    std::swap(x0,y0);
		std::swap(x1,y1);
		steep  = true;
	}
	if(x0>x1){
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	int dx = x1 - x0;
	int dy = y1 - y0;

	float gradient = std::abs(dy)*2;
	float error = 0;
	int y = y0;

	for (int x = x0; x<=x1; x++)
	{
	    if(steep){
		image.set(y,x,color);
		}
		else
		{
		image.set(x,y,color);
		}
		error+=gradient;
		if (error>dx)
		{
		    y+=(y1>y0?1:-1);
			error -= dx*2;
		}
	}
}
	//barycentric
vec3 barycentric(vec2 *pts, vec2 p){
	vec3 x(pts[1].x - pts[0].x, pts[2].x - pts[0].x,pts[0].x-p.x);//0 point A,1 point B, 2 point C.  x(vector AB.x,vector AC.x,vector PA.x)
	vec3 y(pts[1].y - pts[0].y, pts[2].y - pts[0].y, pts[0].y - p.y);
	vec3 u = cross(x,y);

	if(std::abs(u.z) < 1.){
	    return vec3(-1.,1.,0.);
	}
	return (vec3(1.-(u.x / u.z + u.y / u.z),u.x/u.z,u.y/u.z));
}

void triangle(vec2 *pts, TGAImage &image, TGAColor color){
    vec2 boundrayBoxMin(image.get_width()-1.,image.get_height()-1.);
	vec2 boundrayBoxMax(0.,0.);
	for (int i = 0; i < 3; i++)
	{
	    boundrayBoxMin.x = std::min(boundrayBoxMin.x, pts[i].x);
		boundrayBoxMin.y = std::min(boundrayBoxMin.y, pts[i].y);
		boundrayBoxMax.x = std::max(boundrayBoxMax.x, pts[i].x);
		boundrayBoxMax.y = std::max(boundrayBoxMax.y, pts[i].y);
	}

	vec2 p;
	for (p.x = boundrayBoxMin.x;p.x<=boundrayBoxMax.x; p.x++)
	{
		for (p.y = boundrayBoxMin.y; p.y <= boundrayBoxMax.y; p.y++)
		{
			vec3 barycentricP = barycentric(pts,p);
			if (barycentricP.x>=0 && barycentricP.y>=0 && barycentricP.z >= 0)
			{
			    image.set(p.x,p.y,color);
			}
		}
	}
}
vec2 world2screen(vec2 p){
    p.x = (p.x + 1)*width/2;
	p.y = (p.y + 1)*height/2;
	return p;
}

int main(int argc, char** argv) {
	Model* model = nullptr;
	TGAImage image(width, height, TGAImage::RGB);
	//image.set(52, 41, red);
	//line(0,0,500,500,image,red);
	//line(0,500,500,0, image, red);
	//line(0, 250, 500, 250, image, red);
	//line(250, 0, 250, 500, image, red);

	//vec2 pts[3] = {vec2(10,10), vec2(100, 30), vec2(190, 160) };
	//triangle(pts, image, TGAColor(255, 0, 0, 255));

	model = new Model("obj/african_head.obj");


	for (int i = 0; i < model->nfaces(); i++)
	{
	    std::vector<int> face = model->face(i);
		vec2 pts[3];
		std::cout<<i<<std::endl;

		for (int j=0; j	<3; j++)
		{
			vec3 v0 = model->vert(face[j]);
			vec2 p(v0.x, v0.y);
			pts[j]=world2screen(p);

		   //// draw model line
		   //vec3 v0 = model->vert(face[j]);
		   //vec3 v1 = model->vert(face[(j+1)%3]);
		   ////Viewport Conversion
		   //int x0 = (v0.x + 1.) * width / 2.;//scale and translation
		   //int y0 = (v0.y + 1.) * height / 2.;
		   //int x1 = (v1.x + 1.) * width / 2.;
		   //int y1 = (v1.y + 1.) * height / 2.;
		   //line(x0, y0, x1, y1, image, white);
		}
		triangle(pts, image, TGAColor(rand() % 255, rand() % 255, rand() % 255, 255));
	}

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	delete model;
	return 0;
}

