#pragma once
#include <windows.h>
#include "..\pipeline\pipeline.h"
#include"..\camera\camera.h"
#include <string>

class RenderWindow
{
public:
	RenderWindow(float w_width, float w_height, float i_width, float i_hight);
	~RenderWindow();
	int Run(Pipeline pipeline,Camera& camera);
	void destory();
	HDC get_chdc();
	bool InitWindowsApp(HINSTANCE instanceHandle, int show);
private:
    //主窗口句柄
	HWND ghMainWnd = 0;
	
	HDC hdc;//设备句柄
	HDC chdc;//兼容设备句柄

	HBITMAP bmp;
	HBITMAP bmp_old;

	unsigned char* window_fb;//图片缓存指针
	LPVOID ptr;//用来传入的图片缓存指针

	float window_height;
	float window_width;

	float img_width;
	float img_height;

	void window_draw(unsigned char* framebuffer);//传入framebuff准备绘制
	void window_display();//绘制图片
	void CalculateFrameState(Camera& camera);

	void init_bm_header(BITMAPINFOHEADER& bi, int width, int height);
};

