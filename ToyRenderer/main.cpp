#include "tgaimage.h"
#include "model.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

const int width = 1000;
const int height = 1000;
const int depth = 255;
Model* model = nullptr;
TGAImage image(width, height, TGAImage::RGB);

const vec3 light(0, 0, -1);
const vec3 camera(0,0,3);
const vec3 center(0,0,0);
const vec3 up(0,1,0);

//transformation Matrix
mat<4,4>  ModelView;
mat<4,4>  ViewPort;
mat<4,4>  Projection;

//void lookat(const vec3 camera, const vec3 center,const vec3 up){
//    //set new coordinate system
//    vec3 z = (camera - center).normalize();
//	vec3 x = cross(up,z).normalize();
//	vec3 y = cross(z,x).normalize();
//
//	mat<4,4> Minv = {{
//
//	    {x.x,x.y,x.z,0},
//	    {y.x,y.y,y.z,0},
//	    {z.x,z.y,z.z,0},
//	    {  0,  0,  0,1}
//	}
//	};
//
//	mat<4, 4> Minv = { {
//
//		{x.x,x.y,x.z,0},
//		{y.x,y.y,y.z,0},
//		{z.x,z.y,z.z,0},
//		{  0,  0,  0,1}
//	}
//	};
//
//}

void viewport(int x, int y, int w, int h){
	 ViewPort = {{
	 {     w/2.,       0 ,       0,      x+w/2.},
	 {       0,      h/2.,       0,      y+h/2.},
	 {       0,       0,         1.,        1.}, 
	 {       0,       0,        0,           1}
	 }};
}

void projection(float coeff){
{
    Projection = {{
	{1, 0, 0 , 0},
	{0, 1, 0 , 0},
	{0, 0, 1 , 0},
	{0, 0, coeff , 1},
	}};
}
}

vec3& world2screen(vec3& p) {
	p.x = int((p.x + 1) * width / 2);
	p.y = int((p.y + 1) * height / 2);
	return p;
}

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
vec3 barycentric(vec3 *pts, vec3 p){

	vec3 x(pts[1].x - pts[0].x, pts[2].x - pts[0].x,pts[0].x-p.x);//0 point A,1 point B, 2 point C.  x(vector AB.x,vector AC.x,vector PA.x)
	vec3 y(pts[1].y - pts[0].y, pts[2].y - pts[0].y, pts[0].y - p.y);
	vec3 u = cross(x,y);

	if(std::abs(u.z) < 1.){
	    return vec3(-1.,1.,1.);
	}
	return vec3(1.f-(u.x+u.y) / float(u.z),u.x/ float(u.z), u.y/ float(u.z));
}

void triangle(vec3 *pts, vec3* uvs, float* zbuffer, TGAImage &image, float lightIntensity){

    //make boundrayBox to scale down the check area
    vec2 boundrayBoxMin(image.get_width()-1.,image.get_height()-1.);
	vec2 boundrayBoxMax(0.,0.);
	vec2 clamp(image.get_width() - 1, image.get_height() - 1);

	for (int i = 0; i < 3; i++) {
		boundrayBoxMin.x = std::max(0., std::min(boundrayBoxMin.x, pts[i].x));
		boundrayBoxMin.y = std::max(0., std::min(boundrayBoxMin.y, pts[i].y));
		boundrayBoxMax.x = std::min(clamp.x, std::max(boundrayBoxMax.x, pts[i].x));
		boundrayBoxMax.y = std::min(clamp.y, std::max(boundrayBoxMax.y, pts[i].y));
	}

	vec3 p;
	// go through the pixel of boundrayBox
	for (p.x = boundrayBoxMin.x;p.x<=boundrayBoxMax.x; p.x++)
	{
		for (p.y = boundrayBoxMin.y; p.y <= boundrayBoxMax.y; p.y++)
		{
		    // vertex barycentric
			vec3 barycentricP = barycentric(pts, p);
			vec2 uv(0,0);

			//check current pixel if in the triangle
			if (barycentricP.x < 0 || barycentricP.y < 0 || barycentricP.z < 0) continue;

			p.z = 0;
			//Using vertex barycentric lerp z_value
			for (int i = 0; i < 3; i++)
			{
			  //lerp depth
			  p.z += pts[i].z * barycentricP[i];
			  //lerp uv
			  uv.x += uvs[i].x * barycentricP[i];
			  uv.y += uvs[i].y * barycentricP[i];
			}

			//check z vaule if before to z_buff
			if (zbuffer[int(p.x+p.y*width)]<p.z)
			{
				zbuffer[int(p.x + p.y * width)] = p.z;
				//find color
				TGAColor color = TGAColor(model->diffuse(uv).r*lightIntensity, model->diffuse(uv).g * lightIntensity, model->diffuse(uv).b * lightIntensity, model->diffuse(uv).a*lightIntensity * lightIntensity);//TGAColor(255*lightIntensity, 255 * lightIntensity, 255 * lightIntensity, 255 * lightIntensity);
				image.set(p.x, p.y, color);
			}
		}
	}
}

float lightIntensity(vec3* world_coords){
	//light = light.normalize();
	vec3 norm = cross(world_coords[2] - world_coords[0], world_coords[1] - world_coords[0]);
	norm.normalize();
	return norm * light;
}

void drawModelLine(){
	//TGAImage image(width, height, TGAImage::RGB);
	//model = new Model("obj/african_head.obj");
	for (int i = 0; i < model->nfaces(); i++)
	{
		std::vector<int> face = model->face(i);
		vec3 screen_coords[3];
		vec3 world_coords[3];
		std::cout << i << std::endl;

		for (int j = 0; j < 3; j++)
		{
			// draw model line
			vec3 v0 = model->vert(face[j]);
			vec3 v1 = model->vert(face[(j+1)%3]);
			//Viewport Conversion
			int x0 = (v0.x + 1.) * width / 2.;//scale and translation
			int y0 = (v0.y + 1.) * height / 2.;
			int x1 = (v1.x + 1.) * width / 2.;
			int y1 = (v1.y + 1.) * height / 2.;
			line(x0, y0, x1, y1, image, white);
		}
	}
}

void drawModelTriangle() {

	//Create z_buffer and init using min
	float* zbuffer = new float[width * height];
	for (int i = 0; i < width * height; i++) {
		zbuffer[i] = -std::numeric_limits<float>::max();
	}

	for (int i = 0; i < model->nfaces(); i++)
	{
		std::vector<int> face = model->face(i);
		vec3 screen_coords[3];
		vec3 world_coords[3];
		vec3 uvs[3];
		std::cout << i << std::endl;

		for (int j = 0; j < 3; j++)
		{   //get triangle vertex
	        vec3 v = model->vert(face[2 * j]);
			vec4 gl_Vertex = embed<4>(v);
			world_coords[j] = v;
			gl_Vertex = ViewPort * Projection * gl_Vertex;
			screen_coords[j] = vec3(int(gl_Vertex[0]/ gl_Vertex[3]), int(gl_Vertex[1]/gl_Vertex[3]), int(gl_Vertex[2]/gl_Vertex[3]));
			//get uv
			vec2 uv = model->uv(face[2 * j + 1]);
			uvs[j] = vec3(uv.x,uv.y,0);
		}

		float intensity = lightIntensity(world_coords);

		if (intensity > 0)
		{
			triangle(screen_coords, uvs, zbuffer, image, intensity);
		}

	}
	delete zbuffer;
}

int main(int argc, char** argv) {
	model = new Model("obj/african_head.obj");
	//image.set(52, 41, red);
	//line(0,0,500,500,image,red);
	//line(0,500,500,0, image, red);
	//line(0, 250, 500, 250, image, red);
	//line(250, 0, 250, 500, image, red);

	//vec2 pts[3] = {vec2(10,10), vec2(100, 30), vec2(190, 160) };
	//triangle(pts, image, TGAColor(255, 0, 0, 255));
	viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
	projection(-1/camera.z);
	drawModelTriangle();

	image.flip_vertically(); //want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	delete model;
	return 0;
}

