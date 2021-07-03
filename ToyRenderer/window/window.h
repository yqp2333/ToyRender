#pragma once
#include <windows.h>
#include "..\pipeline\pipeline.h"
#include"..\camera\camera.h"
#include <string>

class RenderWindow
{
public:

	RenderWindow(float width, float height);
	~RenderWindow();
	int Run(Pipeline& pipeline,Camera& camera);
	HDC get_chdc(){return chdc;}
	HWND get_window(){return ghMainWnd;}
	bool InitWindowsApp(HINSTANCE instanceHandle, int show);
	void destory();
	int model_index = 0;
private:

	float width;
	float height;

	HWND ghMainWnd = 0;

	HDC hdc;//设备句柄
	HDC chdc;//兼容设备句柄

	HBITMAP bmp;
	HBITMAP bmp_old;

	unsigned char* window_fb;//图片缓存指针
	LPVOID ptr;//用来传入的图片缓存指针

	void window_draw(unsigned char* framebuffer);//传入framebuff准备绘制
	void window_display();//绘制图片
	void CalculateFrameState(Camera& camera);

	void init_bm_header(BITMAPINFOHEADER& bi, int width, int height);

	GameTime& game_time = GameTime::GetInstance();
};

