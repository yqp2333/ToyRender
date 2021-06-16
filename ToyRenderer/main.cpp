#include "tgaimage.h"
#include<windows.h>
#include "model.h"
#include "my_gl.h"
#include "render.h"

HWND ghMainWnd = 0;//主窗口句柄
HDC hdc;//设备句柄
HDC chdc;//兼容设备句柄
HBITMAP bmp;
HBITMAP bmp_old;
LPVOID ptr;
DWORD t_now;
DWORD t_pre;
unsigned char* window_fb;

bool InitWindowsApp(HINSTANCE instanceHandle, int show); //初始化主窗口,成功则返回true

int Run();//封装消息循环代码

LRESULT CALLBACK //主窗口的窗口过程，窗口过程会处理窗口所接收到的消息。回调函数，传进去用于回调
WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void init_bm_header(BITMAPINFOHEADER& bi, int width, int height);
void window_draw(unsigned char* framebuffer);
static void window_display();

//WinMain == Main
int WINAPI
WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
// hInstance 当前程序的实例句柄， hPrevInstance wiwn32不需要此参数， lpCmdLine 运行此程序所用的命令行参数字符串 
// nShowCmd指定程序如何显示 SW_SHOW 按窗口当前的大小与位置显示出来、 SW_SHOWMAXIMIZED 窗口最大化、 SW_SHOWMINIZED 窗口最小化
{
	//创建和初始化应用程序主窗口
	if (!InitWindowsApp(hInstance, nShowCmd)) //初始化成功 返回 true， 失败返回 false。传入应用实例副本和窗口显示命令
		return 0;

	//创建完成后，消息循环开始并持续运转，直到接收消息 WM_QUIT
	return Run();
}

bool InitWindowsApp(HINSTANCE instanceHandle, int show)
{
	//填写 WNDLCASS 结构体，创建窗口

	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;//窗口类的样式。当前样式为，当工作区的高度或宽度发生改变时就重绘
	wc.lpfnWndProc = WndProc;//指向与此窗口类实例相关联的窗口过程函数的指针。基于此实例创建的窗口都会用到这个窗口过程
	wc.cbClsExtra = 0; //用于分配额外的内存空间
	wc.cbWndExtra = 0;//用于分配额外的内存空间
	wc.hInstance = instanceHandle;// 应用实例句柄
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);//窗口图标，内置图标可参考MSDN库
	wc.hCursor = LoadCursor(0, IDC_ARROW);//指针图标
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);//画刷句柄，指定工作区的背景颜色
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"RenderClass";//指定窗口类结构体的名字

	//在windows系统中为上述WNDCLASS 注册一个实例，作为创建窗口的依据。失败则返回0
	if (!RegisterClass(&wc)) {
		MessageBox(0, L"RegisterClass", 0, 0);
		return false;
	}

	//用CreateWindow创建窗口，该函数返回所创建窗口的句柄（HWND类型的数值）。句柄是窗口的引用方式。Windows系统会在内部进行管理，
	//大量win 32 API都需要HWND作为参数

	ghMainWnd = CreateWindow(
		L"RenderClass",//创建的是前面注册的实例
		L"ToyRenderer",//窗口标题
		WS_OVERLAPPEDWINDOW,//窗口样式 dwStyle. 当前是重叠窗口，有标题栏和边框。WS_CAPTION 有一个标题栏的窗口。
							//WS_SYSMENU 由系统菜单。WS_THICKFRAME 可调整大小的边框。WS_MINIMIZEBOX/WS_MAXIMIZEBOX 有最小化/最大化按钮。
		CW_USEDEFAULT,//x坐标，CW_USEDEFAULT，为Windows系统自动选择一个默认值
		CW_USEDEFAULT,//y坐标
		width,//窗口高度 nWidth
		height,//窗口高度 nHeight
		0,//父窗口  所建窗口的父窗口句柄
		0,//菜单句柄  
		instanceHandle, //应用程序实例句柄
		0);//lpParam 只想用户定义数据的指针，可用作WM_CREATE的消息的lpParam参数。发送消息，用于在窗口创建时执行的操作

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

	//展示和更新窗口
	ShowWindow(ghMainWnd, show);//传递句柄和窗口初次显示模式，直接用WinMain传进来的参数
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
		else //执行动画或游戏逻辑部分代码
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

LRESULT CALLBACK //LRESULT 定义为一个整数，表示该函数调用是否成功。回调函数，Windows会在需要的时候自动调用该函数，不需要主动调用。
WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {// HWND接收此消息的窗口的句柄。msg 表示特定消息的预定值

	//处理特定消息


	switch (msg) {

	case WM_CREATE:{


		return 0;
		}
		//按下鼠标左键，弹出消息框
	case WM_LBUTTONDOWN:
	{		

		return 0;

		}
		//按下Esc键，销毁应用程序主窗口
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)// wParam为按下按键的虚拟键代码，用于判断是否是ESC
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

		//收到销毁消息后，发送退出消息，中止消息循环
	case WM_DESTROY:
		PostQuitMessage(0);//当窗口被销毁时，用该函数中止消息循环发出WM_QUIT消息
		return 0;

	}
	return DefWindowProc(hWnd, msg, wParam, lParam);//默认窗口过程，其他为处理的消息都交给它用默认方法处理
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
