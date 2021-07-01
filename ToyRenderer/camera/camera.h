#pragma once
#include"../math/geometry.h"
#include"../time/game_time.h"

class Camera
{
public:
	Camera(vec3 eye, vec3 center, vec3 up,GameTime& game_time);
	~Camera();

	mat<4,4>get_M_View();
	void auto_rotation();

	GameTime& game_time;
private:
       vec3 eye;
	   vec3 center;
	   vec3 up;

	   float nearplane = -0.1;
	   float farplane = -10000.0;
	   float fovy = 60;


};

