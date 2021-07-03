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

	HDC hdc;//�豸���
	HDC chdc;//�����豸���

	HBITMAP bmp;
	HBITMAP bmp_old;

	unsigned char* window_fb;//ͼƬ����ָ��
	LPVOID ptr;//���������ͼƬ����ָ��

	void window_draw(unsigned char* framebuffer);//����framebuff׼������
	void window_display();//����ͼƬ
	void CalculateFrameState(Camera& camera);

	void init_bm_header(BITMAPINFOHEADER& bi, int width, int height);

	GameTime& game_time = GameTime::GetInstance();
};

