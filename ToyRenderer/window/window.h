#pragma once
#include <windows.h>
#include "..\pipeline\pipeline.h"

class RenderWindow
{
public:
	RenderWindow(float w_width, float w_height, float i_width, float i_hight);
	~RenderWindow();
	int Run(Pipeline pipeline);
	void destory();
	HDC get_chdc();
	bool InitWindowsApp(HINSTANCE instanceHandle, int show);
private:
    //�����ھ��
	HWND ghMainWnd = 0;
	
	HDC hdc;//�豸���
	HDC chdc;//�����豸���

	HBITMAP bmp;
	HBITMAP bmp_old;

	unsigned char* window_fb;//ͼƬ����ָ��
	LPVOID ptr;//���������ͼƬ����ָ��

	DWORD t_now;//time ��ǰ֡
	DWORD t_pre;//time ��һ֡

	float window_height;
	float window_width;

	float img_width;
	float img_height;

	void window_draw(unsigned char* framebuffer);//����framebuff׼������
	void window_display();//����ͼƬ

	void init_bm_header(BITMAPINFOHEADER& bi, int width, int height);
};
