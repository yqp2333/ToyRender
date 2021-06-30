#include"window\window.h"
#include"pipeline\pipeline.h"

const float width = 800;
const float height = 640;

const vec3 light = vec3(1, 1, 1);
const vec3 camera(1, 1, 3);
const vec3 center(0, 0, 0);
const vec3 up(0, 1, 0);

const float nearplane = -0.1;
const float farplane = -10000.0;
const float fovy = 60;

const char* model_name= "obj/african_head.obj";

RenderWindow my_window(width, height, width, height);

int WINAPI
WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	if (!my_window.InitWindowsApp(hInstance, nShowCmd))
		return 0;
    HDC chdc = my_window.get_chdc();
	Pipeline pipeline(model_name, width, height, light, camera, fovy, nearplane, farplane, chdc);
	return my_window.Run(pipeline);
}

LRESULT CALLBACK 
WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {

	case WM_CREATE: {

		return 0;
	}
				  //按下鼠标左键，弹出消息框
	case WM_LBUTTONDOWN:
	{
		return 0;
	}
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			return 0;

	case WM_DESTROY:
		my_window.destory();
		delete model_name;
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
