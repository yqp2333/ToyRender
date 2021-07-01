#include "camera.h"
#include"../my_gl/my_gl.h"

Camera::Camera(vec3 eye, vec3 center, vec3 up, GameTime& game_time)
: eye(eye),
  center(center),
  up(up),
  game_time(game_time)
{}

Camera::~Camera()
{
}

mat<4, 4> Camera::get_M_View()
{
	return lookat(eye, center, up);
}

void Camera::auto_rotation()
{
    float radius = 10.0f;
	float camX = sin(game_time.TotalTime()) * radius;
	float camZ = cos(game_time.TotalTime()) * radius;
	eye = vec3(camX,0, camZ);
}
