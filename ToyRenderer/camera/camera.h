#pragma once
#include"../math/geometry.h"
#include"../time/game_time.h"
class Camera
{
public:
	Camera(vec3 eye, vec3 center, vec3 up);
	~Camera();

	mat<4,4>get_M_View();
	void auto_rotation();
	HWND window;

	GameTime& game_time = GameTime::GetInstance();

	//mouse
	char mouse_buttons[2];
	vec2 orbit_pos;
	vec2 orbit_delta;
	vec2 fv_pos;
	vec2 fv_delta;
	float wheel_delta;

	vec3 eye;
	vec3 center;
	vec3 up;
	vec3 x;
	vec3 y;
	vec3 z;

	vec2 get_mouse_pos();

private:

};

void updata_camera_pos(Camera& camera);
void handle_events(Camera& camera);
