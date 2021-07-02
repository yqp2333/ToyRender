#include"window\window.h"
#include"pipeline\pipeline.h"
#include"camera\camera.h"
#include"time\game_time.h"

const float width = 800;
const float height = 640;

const vec3 light = vec3(1, 1, 1);
const vec3 eye(2, 2, 5);
const vec3 center(0, 0, 0);
const vec3 up(0, 1, 0);

const float nearplane = -0.1;
const float farplane = -100000;
const float fovy = 60;

const char* model_name= "obj/african_head.obj";
const char* skybox_name = "obj/skybox/box.obj";

RenderWindow my_window(width, height, width, height);
GameTime game_time;
Camera camera(eye, center, up, game_time);


int WINAPI
WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	if (!my_window.InitWindowsApp(hInstance, nShowCmd))
		return 0;
	camera.window = my_window.ghMainWnd;
    HDC chdc = my_window.get_chdc();
	Pipeline pipeline(camera, model_name, width, height, light, fovy, nearplane, farplane, chdc);
	return my_window.Run(pipeline,camera);
}

LRESULT CALLBACK 
WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {

	case WM_CREATE: 
		break;

	case WM_LBUTTONDOWN:
		camera.orbit_pos = camera.get_mouse_pos();
		camera.mouse_buttons[0] = 1;
		break;

	case WM_LBUTTONUP:
		camera.mouse_buttons[0] = 0;
		break;

	case WM_RBUTTONDOWN:
		camera.fv_pos = camera.get_mouse_pos();
		camera.mouse_buttons[1] = 1;
		break;

	case WM_RBUTTONUP:
		camera.mouse_buttons[1] = 0;
		break;

	case WM_MOUSEWHEEL:
		camera.wheel_delta = GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
		return 0;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		break;

	case WM_DESTROY:
		my_window.destory();
		delete model_name;
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
