#pragma once

struct Bitmap
{
	ComPtr<ID2D1Bitmap1>	data;
	D2D1_SIZE_F				sizef;
	D2D1_SIZE_U				sizeu;
};

class Application
{
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	UINT m_width, m_height;

	D2D1_POINT_2F m_offset;

	ComPtr<IDCompositionDesktopDevice>	m_device;			//Composition设备
	ComPtr<IDCompositionTarget>			m_target;			//target，针对HWND窗口
	ComPtr<IDCompositionVisual2>		m_visual;			//根Visual
	ComPtr<IDCompositionVirtualSurface>	m_surface;			//渲染表面

	ComPtr<ID2D1DeviceContext>			m_dc;				//DC，用来创建资源
	ComPtr<ID2D1SolidColorBrush>		m_brush;			//一支纯色画笔
	Bitmap								m_bmp;

	ComPtr<IWICImagingFactory2>			m_factory;			//WIC工厂

	void CreateDeviceAndResources();
	void MoveVisual(float x, float y);
	void Update();
	void LoadPicture(wstring filename);
	void LoadPicture(int resourceId, LPCTSTR type);

	wstring OpenFile();
	
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	LRESULT Handle_WM_PAINT(WPARAM wParam, LPARAM lParam);
	LRESULT Handle_WM_SIZE(WPARAM wParam, LPARAM lParam);
	LRESULT Handle_WM_LBUTTONDOWN(WPARAM wParam, LPARAM lParam);
	LRESULT Handle_WM_LBUTTONUP(WPARAM wParam, LPARAM lParam);
	LRESULT Handle_WM_MOUSEMOVE(WPARAM wParam, LPARAM lParam);
	LRESULT Handle_WM_MOUSEWHEEL(WPARAM wParam, LPARAM lParam);
	LRESULT Handle_WM_KEYDOWN(WPARAM wParam, LPARAM lParam);
	LRESULT Handle_WM_DESTROY(WPARAM wParam, LPARAM lParam);
	LRESULT Handle_WM_DROPFILES(WPARAM wParam, LPARAM lParam);
	LRESULT Handle_WM_COMMAND(WPARAM wParam, LPARAM lParam);

public:
	Application(UINT width, UINT height, HINSTANCE hInstance) :
		m_width(width),
		m_height(height),
		m_hInstance(hInstance),
		m_offset{}
	{}

	int Run(int nCmdShow)
	{
		WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
		wc.style = CS_VREDRAW | CS_HREDRAW;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = m_hInstance;
		wc.hIcon = LoadIcon(m_hInstance, IDI_APPLICATION);
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
		wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
		wc.lpszClassName = L"HelloDComp";
		wc.hIconSm = nullptr;

		RegisterClassEx(&wc);

		DWORD style = WS_OVERLAPPEDWINDOW; DWORD styleEx = 0;
		RECT rc = { 0,0,(LONG)m_width,(LONG)m_height };
		AdjustWindowRectEx(&rc, style, false, styleEx);
		auto cx = GetSystemMetrics(SM_CXSCREEN);
		auto cy = GetSystemMetrics(SM_CYFULLSCREEN);
		auto w = rc.right - rc.left;
		auto h = rc.bottom - rc.top;
		auto x = (cx - w) / 2;
		auto y = (cy - h) / 2;
		
		m_hWnd = CreateWindowEx(WS_EX_ACCEPTFILES, wc.lpszClassName, L"DComposition显示图片", WS_OVERLAPPEDWINDOW,
			x, y, w, h, nullptr, nullptr, m_hInstance, nullptr);

		CreateDeviceAndResources();
		SetWindowLongPtr(m_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		ShowWindow(m_hWnd, nCmdShow);

		MSG msg = {};
		while (GetMessage(&msg, nullptr, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return static_cast<int>(msg.wParam);
	}
};

