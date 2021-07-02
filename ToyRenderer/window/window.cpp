#include"window.h"

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
		window_width,//窗口高度 nWidth
		window_height,//窗口高度 nHeight
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
	init_bm_header(bi, img_width, img_height);

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

int RenderWindow::Run(Pipeline pipeline, Camera& camera)
{
	Pipeline current_pipeline = pipeline;
	MSG msg = { 0 };
	camera.game_time.Reset();
	while (msg.message != WM_QUIT) {

		handle_events(camera);
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else //执行动画或游戏逻辑部分代码
		{
		    camera.game_time.Tick();
			CalculateFrameState(camera);
			//camera.auto_rotation();
		    window_draw(current_pipeline.render(chdc));
			camera.wheel_delta = 0;
			camera.orbit_delta = vec2(0, 0);
			camera.fv_delta = vec2(0, 0);
		}
	
	}
	return (int)msg.wParam;
}

void RenderWindow::window_display(){
	hdc = GetDC(ghMainWnd);
	BitBlt(hdc, 0, 0, img_width, img_height, chdc, 0, 0, SRCCOPY);
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

HDC RenderWindow::get_chdc()
{
	return chdc;
}

void RenderWindow::window_draw(unsigned char* framebuffer) //framebuffer to window_fb
{
	int i, j;
	for (int i = 0; i < img_height; i++)
	{
		for (int j = 0; j < img_width; j++)
		{
			int index = (i * img_width + j) * 4;
			window_fb[index] = framebuffer[index + 2];
			window_fb[index + 1] = framebuffer[index + 1];
			window_fb[index + 2] = framebuffer[index];
		}
	}
	window_display();
}

RenderWindow::RenderWindow(float w_width, float w_height, float i_width, float i_hight)
	:window_width(w_width),
	window_height(w_height),
	img_width(i_width),
	img_height(i_hight)
	{}

RenderWindow::~RenderWindow()
{
}

void RenderWindow::CalculateFrameState(Camera& camera)
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f; //流逝时间
	frameCnt++;

	if (camera.game_time.TotalTime() - timeElapsed >= 1.0f)
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

