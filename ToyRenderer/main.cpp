#include "tgaimage.h"
#include<windows.h>
#include "model.h"
#include "my_gl.h"
#include "render.h"

HWND ghMainWnd = 0;//�����ھ��
HDC hdc;//�豸���
HDC chdc;//�����豸���
HBITMAP bmp;
HBITMAP bmp_old;
LPVOID ptr;
DWORD t_now;
DWORD t_pre;
unsigned char* window_fb;

bool InitWindowsApp(HINSTANCE instanceHandle, int show); //��ʼ��������,�ɹ��򷵻�true

int Run();//��װ��Ϣѭ������

LRESULT CALLBACK //�����ڵĴ��ڹ��̣����ڹ��̻ᴦ���������յ�����Ϣ���ص�����������ȥ���ڻص�
WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void init_bm_header(BITMAPINFOHEADER& bi, int width, int height);
void window_draw(unsigned char* framebuffer);
static void window_display();

//WinMain == Main
int WINAPI
WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
// hInstance ��ǰ�����ʵ������� hPrevInstance wiwn32����Ҫ�˲����� lpCmdLine ���д˳������õ������в����ַ��� 
// nShowCmdָ�����������ʾ SW_SHOW �����ڵ�ǰ�Ĵ�С��λ����ʾ������ SW_SHOWMAXIMIZED ������󻯡� SW_SHOWMINIZED ������С��
{
	//�����ͳ�ʼ��Ӧ�ó���������
	if (!InitWindowsApp(hInstance, nShowCmd)) //��ʼ���ɹ� ���� true�� ʧ�ܷ��� false������Ӧ��ʵ�������ʹ�����ʾ����
		return 0;

	//������ɺ���Ϣѭ����ʼ��������ת��ֱ��������Ϣ WM_QUIT
	return Run();
}

bool InitWindowsApp(HINSTANCE instanceHandle, int show)
{
	//��д WNDLCASS �ṹ�壬��������

	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;//���������ʽ����ǰ��ʽΪ�����������ĸ߶Ȼ��ȷ����ı�ʱ���ػ�
	wc.lpfnWndProc = WndProc;//ָ����˴�����ʵ��������Ĵ��ڹ��̺�����ָ�롣���ڴ�ʵ�������Ĵ��ڶ����õ�������ڹ���
	wc.cbClsExtra = 0; //���ڷ��������ڴ�ռ�
	wc.cbWndExtra = 0;//���ڷ��������ڴ�ռ�
	wc.hInstance = instanceHandle;// Ӧ��ʵ�����
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);//����ͼ�꣬����ͼ��ɲο�MSDN��
	wc.hCursor = LoadCursor(0, IDC_ARROW);//ָ��ͼ��
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);//��ˢ�����ָ���������ı�����ɫ
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"RenderClass";//ָ��������ṹ�������

	//��windowsϵͳ��Ϊ����WNDCLASS ע��һ��ʵ������Ϊ�������ڵ����ݡ�ʧ���򷵻�0
	if (!RegisterClass(&wc)) {
		MessageBox(0, L"RegisterClass", 0, 0);
		return false;
	}

	//��CreateWindow�������ڣ��ú����������������ڵľ����HWND���͵���ֵ��������Ǵ��ڵ����÷�ʽ��Windowsϵͳ�����ڲ����й���
	//����win 32 API����ҪHWND��Ϊ����

	ghMainWnd = CreateWindow(
		L"RenderClass",//��������ǰ��ע���ʵ��
		L"ToyRenderer",//���ڱ���
		WS_OVERLAPPEDWINDOW,//������ʽ dwStyle. ��ǰ���ص����ڣ��б������ͱ߿�WS_CAPTION ��һ���������Ĵ��ڡ�
							//WS_SYSMENU ��ϵͳ�˵���WS_THICKFRAME �ɵ�����С�ı߿�WS_MINIMIZEBOX/WS_MAXIMIZEBOX ����С��/��󻯰�ť��
		CW_USEDEFAULT,//x���꣬CW_USEDEFAULT��ΪWindowsϵͳ�Զ�ѡ��һ��Ĭ��ֵ
		CW_USEDEFAULT,//y����
		width,//���ڸ߶� nWidth
		height,//���ڸ߶� nHeight
		0,//������  �������ڵĸ����ھ��
		0,//�˵����  
		instanceHandle, //Ӧ�ó���ʵ�����
		0);//lpParam ֻ���û��������ݵ�ָ�룬������WM_CREATE����Ϣ��lpParam������������Ϣ�������ڴ��ڴ���ʱִ�еĲ���

	if (ghMainWnd == 0) {
		MessageBox(0, L"CreateWindow FAILED", 0, 0);
		return false;
	}


    //init bmp header
	BITMAPINFOHEADER bi;
	init_bm_header(bi, width, height);

	//registe DC
	hdc = GetDC(ghMainWnd);
	chdc = CreateCompatibleDC(hdc);

	ReleaseDC(ghMainWnd,hdc);

	//creat bmp and bind
	bmp = CreateDIBSection(chdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, &ptr, 0, 0);
	bmp_old = (HBITMAP)SelectObject(chdc, bmp);

	//the show data
	window_fb = (unsigned char*)ptr;


	initRenderInfo();

	//չʾ�͸��´���
	ShowWindow(ghMainWnd, show);//���ݾ���ʹ��ڳ�����ʾģʽ��ֱ����WinMain�������Ĳ���
	UpdateWindow(ghMainWnd);

	return true;
}

int Run()
{
	MSG msg = { 0 };
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else //ִ�ж�������Ϸ�߼����ִ���
		{
			DWORD t_now = GetTickCount();
			if (t_now - t_pre >= 20)//0.05s
			{
				window_draw(render(chdc));
				camera.x += 0.1;
			}

		}

	}

	return (int)msg.wParam;
}

LRESULT CALLBACK //LRESULT ����Ϊһ����������ʾ�ú��������Ƿ�ɹ����ص�������Windows������Ҫ��ʱ���Զ����øú���������Ҫ�������á�
WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {// HWND���մ���Ϣ�Ĵ��ڵľ����msg ��ʾ�ض���Ϣ��Ԥ��ֵ

	//�����ض���Ϣ


	switch (msg) {

	case WM_CREATE:{


		return 0;
		}
		//������������������Ϣ��
	case WM_LBUTTONDOWN:
	{		

		return 0;

		}
		//����Esc��������Ӧ�ó���������
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)// wParamΪ���°�������������룬�����ж��Ƿ���ESC
			DestroyWindow(ghMainWnd);
		    delete model;
			delete zbuffer;
			delete frameBuffer;
			delete shadowbuffer;
			delete window_fb;
			DeleteObject(bmp);
			DeleteObject(bmp_old);
			DeleteDC(chdc);
		return 0;

		//�յ�������Ϣ�󣬷����˳���Ϣ����ֹ��Ϣѭ��
	case WM_DESTROY:
		PostQuitMessage(0);//�����ڱ�����ʱ���øú�����ֹ��Ϣѭ������WM_QUIT��Ϣ
		return 0;

	}
	return DefWindowProc(hWnd, msg, wParam, lParam);//Ĭ�ϴ��ڹ��̣�����Ϊ�������Ϣ����������Ĭ�Ϸ�������
}


static void window_display()//show bmp
{
	hdc = GetDC(ghMainWnd);
	BitBlt(hdc, 0, 0, width, height, chdc, 0, 0, SRCCOPY);
	ReleaseDC(ghMainWnd, hdc);
	DWORD t_pre = GetTickCount();
}

void window_draw(unsigned char* framebuffer) //framebuffer to window_fb
{
	int i, j;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int index = (i * width + j) * 4;
			window_fb[index] = framebuffer[index + 2];
			window_fb[index + 1] = framebuffer[index + 1];
			window_fb[index + 2] = framebuffer[index];
		}
	}
	window_display();

}static void init_bm_header(BITMAPINFOHEADER& bi, int width, int height)// init bmp header
{
	memset(&bi, 0, sizeof(BITMAPINFOHEADER));
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = -height;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = width * height * 4;
}
