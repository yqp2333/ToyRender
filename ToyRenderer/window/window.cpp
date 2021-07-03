#include"window.h"

wchar_t text1[] = L"L_Button : rotate";
wchar_t text2[] = L"R_Button : move";
wchar_t text3[] = L"Wheel     : zoom";
wchar_t text4[] = L"Tab         : change model";
wchar_t text5[] = L"Ctrl         : on/off shadow(has problem)";
wchar_t text6[] = L"Shift       : on/off skybox(not yet installed)";

LRESULT CALLBACK
WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool RenderWindow::InitWindowsApp(HINSTANCE instanceHandle, int show)
{
	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = instanceHandle;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);//画刷句柄，指定工作区的背景颜色
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"RenderClass";

	if (!RegisterClass(&wc)) {
		MessageBox(0, L"RegisterClass", 0, 0);
		return false;
	}

	ghMainWnd = CreateWindow(
		L"RenderClass",
		L"ToyRenderer",//窗口标题
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,//窗口高度 nWidth
		height,//窗口高度 nHeight
		0,
		0,
		instanceHandle, 
		0);


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

	ReleaseDC(ghMainWnd, hdc);

	//creat bmp and bind
	bmp = CreateDIBSection(chdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, &ptr, 0, 0);
	bmp_old = (HBITMAP)SelectObject(chdc, bmp);

	//the show data
	window_fb = (unsigned char*)ptr;

	ShowWindow(ghMainWnd, show);
	UpdateWindow(ghMainWnd);

	return true;
}

void RenderWindow::init_bm_header(BITMAPINFOHEADER& bi, int width, int height)
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


RenderWindow::RenderWindow(float width, float height)
:width(width),height(height)
{
}

int RenderWindow::Run(Pipeline& pipeline, Camera& camera)
{
	Pipeline current_pipeline = pipeline;
	MSG msg = { 0 };
	game_time.Reset();
	while (msg.message != WM_QUIT) {

		handle_events(camera);
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else //执行动画或游戏逻辑部分代码
		{
		    game_time.Tick();

			CalculateFrameState(camera);

			window_draw(current_pipeline.render(chdc,model_index,is_shadow,is_skybox));


			//reset 
			camera.wheel_delta = 0;
			camera.orbit_delta = vec2(0, 0);
			camera.fv_delta = vec2(0, 0);
		}
	}
	return (int)msg.wParam;
}

void RenderWindow::window_display(){
	hdc = GetDC(ghMainWnd);

	SetTextColor(chdc,RGB(255,255,255));
	SetBkMode(chdc,TRANSPARENT);
	TextOut(chdc, 10, 0, text1, wcslen(text1));
	TextOut(chdc, 10, 20, text2, wcslen(text2));
	TextOut(chdc, 10, 40, text3, wcslen(text3));
	TextOut(chdc, 10, 60, text4, wcslen(text4));
	TextOut(chdc, 10, 80, text5, wcslen(text5));
	TextOut(chdc, 10, 100, text6, wcslen(text6));

	BitBlt(hdc, 0, 0, width, height, chdc, 0, 0, SRCCOPY);
	ReleaseDC(ghMainWnd, hdc);
}

void RenderWindow::destory()
{
	DestroyWindow(ghMainWnd);
	delete window_fb;
	DeleteObject(bmp);
	DeleteObject(bmp_old);
	DeleteDC(chdc);
}

void RenderWindow::window_draw(unsigned char* framebuffer) //framebuffer to window_fb
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
}

RenderWindow::~RenderWindow()
{
}

void RenderWindow::CalculateFrameState(Camera& camera)
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f; //流逝时间
	frameCnt++;

	if (game_time.TotalTime() - timeElapsed >= 1.0f)
	{
		float fps = (float)frameCnt;
		float mspf = 1000.0f / fps;

		std::wstring fpsStr = std::to_wstring(fps);
		std::wstring mspfStr = std::to_wstring(mspf);

		std::wstring windowText = L"ToyRenderer  fps:" + fpsStr + L"    mspf" + mspfStr;
		SetWindowText(ghMainWnd, windowText.c_str());

		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

