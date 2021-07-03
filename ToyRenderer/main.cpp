#include"window\window.h"
#include"pipeline\pipeline.h"
#include"camera\camera.h"
#include"time\game_time.h"

const float width = 800;
const float height = 640;

const vec3 light = vec3(1, 1, 1);
const vec3 eye(2, 2,5);
const vec3 center(0, 0, 0);
const vec3 up(0, 1, 0);

const float nearplane = -1;
const float farplane = -10000;
const float fovy = 60;

const char* model_name= "obj/diablo3_pose.obj";
const char* skybox_name = "obj/skybox/box.obj";

RenderWindow my_window(width, height);
GameTime& game_time = GameTime::GetInstance();
Camera camera(eye, center, up);
HDC chdc = my_window.get_chdc();
Pipeline pipeline(camera, model_name, width, height, light, fovy, nearplane, farplane, chdc);

int WINAPI
WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	if (!my_window.InitWindowsApp(hInstance, nShowCmd))
		return 0;
	camera.window = my_window.get_window();
	return my_window.Run(pipeline,camera);
}

LRESULT CALLBACK 
WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {

	case WM_CREATE: 
		return 0;

	case WM_LBUTTONDOWN:
		camera.orbit_pos = camera.get_mouse_pos();
		camera.mouse_buttons[0] = 1;
		return 0;

	case WM_LBUTTONUP:
		camera.mouse_buttons[0] = 0;
		return 0;

	case WM_RBUTTONDOWN:
		camera.fv_pos = camera.get_mouse_pos();
		camera.mouse_buttons[1] = 1;
		return 0;

	case WM_RBUTTONUP:
		camera.mouse_buttons[1] = 0;
		return 0;

	case WM_MOUSEWHEEL:
		camera.wheel_delta = GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
		return 0;

	case WM_KEYDOWN:
		 if (wParam == VK_TAB){
			 if (my_window.model_index == 0)
			 {
				 my_window.model_index = 1;
			 }
			 else
			 {
				 my_window.model_index = 0;
			 }
		 }
		 return 0;
	case WM_DESTROY:
		my_window.destory();
		delete model_name;
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
