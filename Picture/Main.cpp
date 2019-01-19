#define WIN32_LEAN_AND_MEAN
#include <wrl.h>
using Microsoft::WRL::ComPtr;

#include <string>
using std::wstring;

#include <d3d11.h>
#include <dcomp.h>
#include <d2d1_3.h>
#include <wincodec.h>
#include <CommCtrl.h>

#pragma comment(lib, "d3d11")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dcomp")
#pragma comment(lib, "windowscodecs")
#pragma comment(lib, "ComCtl32")

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

static HRESULT hr = S_OK;

class Application
{
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	UINT m_width, m_height;

	static const UINT TargetCount = 2U;

	ComPtr<IDCompositionDesktopDevice>	m_device;			//Composition设备
	ComPtr<IDCompositionTarget>			m_target[TargetCount],m_targetForBtn;			//target，针对HWND窗口
	ComPtr<IDCompositionVisual2>		m_visual[TargetCount],m_visualForBtn;			//根Visual
	ComPtr<IDCompositionVirtualSurface>	m_surface[TargetCount],m_surfaceForBtn;			//渲染表面

	ComPtr<ID2D1SolidColorBrush>		m_brush;			//一支纯色画笔
	ComPtr<ID2D1Bitmap1>				m_bmp[TargetCount];				//一幅待显示位图
	D2D1_SIZE_U							m_bmpSize[TargetCount];			//位图尺寸

	void Initialize()					//初始化资源
	{
		CreateDeviceAndResources();
		MakeRelaition();
		Update();
	}

	void CreateDeviceAndResources()		//创建设备和其他资源
	{
		D3D_FEATURE_LEVEL featureLevelSupported;
		ComPtr<ID3D11Device> d3dDevice;
		D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0, D3D11_SDK_VERSION, d3dDevice.GetAddressOf(), &featureLevelSupported, nullptr);

		ComPtr<IDXGIDevice> dxgiDevice;
		d3dDevice.As(&dxgiDevice);

		ComPtr<ID2D1Device> d2dDevice;
		D2D1CreateDevice(dxgiDevice.Get(), D2D1::CreationProperties(D2D1_THREADING_MODE_SINGLE_THREADED, D2D1_DEBUG_LEVEL_NONE, D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS), &d2dDevice);

		ComPtr<ID2D1DeviceContext> dc;
		d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &dc);
		dc->CreateSolidColorBrush(D2D1::ColorF(0x2255ff), &m_brush);
		
		DCompositionCreateDevice3(d2dDevice.Get(), IID_PPV_ARGS(&m_device));

		wstring filename[] = { L"QQ.ico",L"github.jpg" };
		BOOL topmost[] = { TRUE,FALSE };
		for (UINT i = 0; i < TargetCount; i++)
		{
			LoadPictures(filename[i].c_str(), dc.Get(), &m_bmp[i]);
			m_bmpSize[i] = m_bmp[i]->GetPixelSize();

			m_device->CreateTargetForHwnd(m_hWnd, topmost[i], &m_target[i]);
			m_device->CreateVisual(&m_visual[i]);
			m_device->CreateVirtualSurface(m_width, m_height, DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ALPHA_MODE_PREMULTIPLIED, &m_surface[i]);
		}
	}

	void LoadPictures(wstring filename, ID2D1DeviceContext* dc, ID2D1Bitmap1** d2Bmp)
	{
		static ComPtr<IWICImagingFactory2>	factory;		//WIC工厂
		ComPtr<IWICBitmapDecoder>			decoder;		//解码器
		ComPtr<IWICBitmapFrameDecode>		frame;			//一帧图像数据
		ComPtr<IWICFormatConverter>			format;			//格式转换器
		ComPtr<IWICBitmap>					bmp;			//位图

		if (!factory)						//第一次调用函数工厂为空，创建之
		{
			CoCreateInstance(
				CLSID_WICImagingFactory2, nullptr,
				CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory));
		}

		factory->CreateDecoderFromFilename(	//从文件中创建解码器
			filename.c_str(),
			nullptr,
			GENERIC_READ,
			WICDecodeMetadataCacheOnDemand,
			&decoder);

		decoder->GetFrame(0, &frame);		//从解码器中获取一帧图像

		factory->CreateFormatConverter(&format);
		format->Initialize(
			frame.Get(),
			GUID_WICPixelFormat32bppPBGRA,	//转换图像格式为RGBA格式
			WICBitmapDitherTypeNone,
			nullptr,
			1.0,
			WICBitmapPaletteTypeCustom);

		factory->CreateBitmapFromSource(	//利用转换完成的图像数据创建位图
			format.Get(),
			WICBitmapCacheOnDemand,
			&bmp);

		auto hr = dc->CreateBitmapFromWicBitmap(bmp.Get(), d2Bmp);
	}

	void MakeRelaition()				//为Target设置根，为Visual设置内容
	{
		for (UINT i = 0; i < TargetCount; i++)
		{
			m_target[i]->SetRoot(m_visual[i].Get());
			m_visual[i]->SetContent(m_surface[i].Get());
		}
	}

	void Update()						//开始呈现
	{
		POINT offset;
		ComPtr<ID2D1DeviceContext> dc;

		for (UINT i = 0; i < TargetCount; i++)
		{
			m_surface[i]->BeginDraw(nullptr, IID_PPV_ARGS(&dc), &offset);
			if (dc)
			{
				dc->Clear(0);
				dc->SetTransform(D2D1::Matrix3x2F::Translation((float)offset.x, (float)offset.y));

				D2D1_RECT_F rcBmp = {
					(m_width - m_bmpSize[i].width) / 2.f,
					(m_height - m_bmpSize[i].height) / 2.f,
					(m_width + m_bmpSize[i].width) / 2.f,
					(m_height + m_bmpSize[i].height) / 2.f,
				};

				dc->DrawBitmap(m_bmp[i].Get(), rcBmp);
				dc->DrawRectangle(rcBmp, m_brush.Get(), 2);
			}
			m_surface[i]->EndDraw();
		}
		m_device->Commit();
	}

	void Resize(UINT width, UINT height)
	{
		m_width = width; m_height = height;
		for(auto &surface:m_surface)
			surface->Resize(width, height);
		MoveChildWindow();
		Update();
	}

	HWND m_hbt;

	void CreateChildWindow()
	{
		m_hbt = CreateWindow(WC_BUTTON, L"Hello Button", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, m_hWnd, 0, m_hInstance, 0);
		m_device->CreateTargetForHwnd(m_hbt, TRUE, &m_targetForBtn);
		m_device->CreateVisual(&m_visualForBtn);
		m_device->CreateVirtualSurface(m_width, m_height, DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ALPHA_MODE_PREMULTIPLIED, &m_surfaceForBtn);
		m_targetForBtn->SetRoot(m_visualForBtn.Get());
		m_visualForBtn->SetContent(m_surfaceForBtn.Get());

		POINT offset;
		ComPtr<ID2D1DeviceContext> dc;
		m_surfaceForBtn->BeginDraw(nullptr, IID_PPV_ARGS(&dc), &offset);
		dc->Clear(D2D1::ColorF(D2D1::ColorF::DarkSlateBlue, 0.5f));
		dc->SetTransform(D2D1::Matrix3x2F::Translation((float)offset.x, (float)offset.y));
		m_surfaceForBtn->EndDraw();
		m_device->Commit();
	}

	void MoveChildWindow()
	{
		UINT btnWidth = 600U, btnHeight = 600U;
		//MoveWindow(m_hbt, (m_width - btnHeight) / 2, (m_height - btnHeight) / 2, (m_width + btnHeight) / 2, (m_height + btnHeight) / 2, TRUE);
		MoveWindow(m_hbt, 0, 0, btnWidth, btnHeight, TRUE);
	}

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		auto app = reinterpret_cast<Application*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		switch (msg)
		{
		case WM_SIZE:
			app->Resize(LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		return 0;
	}
public:
	Application(UINT width, UINT height, HINSTANCE hInstance) :
		m_width(width),
		m_height(height),
		m_hInstance(hInstance) {}

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
		wc.lpszMenuName = nullptr;
		wc.lpszClassName = L"HelloDComp";
		wc.hIconSm = nullptr;

		RegisterClassEx(&wc);

		DWORD style = WS_OVERLAPPEDWINDOW; DWORD styleEx = 0;
		RECT rc = { 0,0,(LONG)m_width,(LONG)m_height};
		AdjustWindowRectEx(&rc, style, false, styleEx);
		auto cx = GetSystemMetrics(SM_CXSCREEN);
		auto cy = GetSystemMetrics(SM_CYFULLSCREEN);
		auto w = rc.right - rc.left;
		auto h = rc.bottom - rc.top;
		auto x = (cx - w) / 2;
		auto y = (cy - h) / 2;

		m_hWnd = CreateWindowEx(WS_EX_NOREDIRECTIONBITMAP, wc.lpszClassName, L"DComposition显示图片", WS_OVERLAPPEDWINDOW,
			x, y, w, h, nullptr, nullptr, m_hInstance, nullptr);
		SetWindowLongPtr(m_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		Initialize();
		CreateChildWindow();

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

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, INT nCmdShow)
{
	CoInitialize(nullptr);
	return Application(1280, 720, hInstance).Run(nCmdShow);
}