#include "camera.h"
#include"../my_gl/my_gl.h"

Camera::Camera(vec3 eye, vec3 center, vec3 up)
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

void updata_camera_pos(Camera& camera)
{
	vec3 from_target = camera.eye - camera.center;			
	float radius = from_target.norm();

	float phi = (float)atan2(from_target[0], from_target[2]); 
	float theta = (float)acos(from_target[1] / radius);		  
	float x_delta = camera.orbit_delta[0] / 800.;
	float y_delta = camera.orbit_delta[1] / 640.;

	// for mouse wheel
	radius *= (float)pow(0.95, camera.wheel_delta);

	float factor = 1.5 * 3.1415926;
	// for mouse left button
	phi += x_delta * factor;
	theta += y_delta * factor;
	if (theta > 3.1415926) theta = 3.1415926 - (1e-5f) * 100;
	if (theta < 0)  theta = (1e-5f) * 100;

	camera.eye[0] = camera.center[0] + radius * sin(phi) * sin(theta);
	camera.eye[1] = camera.center[1] + radius * cos(theta);
	camera.eye[2] = camera.center[2] + radius * sin(theta) * cos(phi);

	// for mouse right button
	factor = radius * (float)tan(60.0 / 360 * 3.1415926) * 2.2;
	x_delta = camera.fv_delta[0] / 800.;
	y_delta = camera.fv_delta[1] / 640.;
	vec3 left = x_delta * factor * camera.x;
	vec3 up = y_delta * factor * camera.y;

	camera.eye = camera.eye + (left - up);
	camera.center = camera.center + (left - up);
}

void handle_mouse_events(Camera& camera)
{
	if (camera.mouse_buttons[0])
	{
		vec2 cur_pos = camera.get_mouse_pos();
		camera.orbit_delta = camera.orbit_pos - cur_pos;
		camera.orbit_pos = cur_pos;
	}

	if (camera.mouse_buttons[1])
	{
		vec2 cur_pos = camera.get_mouse_pos();
		camera.fv_delta = camera.fv_pos - cur_pos;
		camera.fv_pos = cur_pos;
	}

	updata_camera_pos(camera);
}


void handle_events(Camera& camera)
{
	//calculate camera axis
	camera.z = (camera.eye - camera.center).normalize();
	camera.x = cross(camera.up, camera.z).normalize();
	camera.y = cross(camera.z, camera.x).normalize();

	//mouse and keyboard events
	handle_mouse_events(camera);
}

vec2 Camera::get_mouse_pos()
{
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(window, &point);
	return vec2((float)point.x, (float)point.y);
}