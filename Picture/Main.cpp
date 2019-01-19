#define WIN32_LEAN_AND_MEAN
#include <wrl.h>
using Microsoft::WRL::ComPtr;

#include <string>
using std::wstring;

#include <d3d11.h>
#include <dcomp.h>
#include <d2d1_3.h>
#include <wincodec.h>

#pragma comment(lib, "d3d11")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dcomp")
#pragma comment(lib, "windowscodecs")

class Application
{
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	UINT m_width, m_height;

	ComPtr<IDCompositionDesktopDevice>	m_device;			//Composition�豸
	ComPtr<IDCompositionTarget>			m_target;			//target�����HWND����
	ComPtr<IDCompositionVisual2>		m_visual;			//��Visual
	ComPtr<IDCompositionVirtualSurface>	m_surface;			//��Ⱦ����

	ComPtr<ID2D1SolidColorBrush>		m_brush;			//һ֧��ɫ����
	ComPtr<ID2D1Bitmap1>				m_bmp;				//һ������ʾλͼ
	D2D1_SIZE_U							m_bmpSize;			//λͼ�ߴ�

	void Initialize()					//��ʼ����Դ
	{
		CreateDeviceAndResources();
		MakeRelaition();
		Update();
	}

	void CreateDeviceAndResources()		//�����豸��������Դ
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
		//LoadPictures(L"github.jpg", dc.Get(), &m_bmp);
		LoadPictures(L"QQ.ico", dc.Get(), &m_bmp);
		m_bmpSize = m_bmp->GetPixelSize();

		DCompositionCreateDevice3(d2dDevice.Get(), IID_PPV_ARGS(&m_device));

		m_device->CreateTargetForHwnd(m_hWnd, FALSE, &m_target);
		m_device->CreateVisual(&m_visual);
		m_device->CreateVirtualSurface(m_width, m_height, DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ALPHA_MODE_PREMULTIPLIED, &m_surface);
	}

	void LoadPictures(wstring filename, ID2D1DeviceContext* dc, ID2D1Bitmap1** d2Bmp)
	{
		static ComPtr<IWICImagingFactory2>	factory;		//WIC����
		ComPtr<IWICBitmapDecoder>			decoder;		//������
		ComPtr<IWICBitmapFrameDecode>		frame;			//һ֡ͼ������
		ComPtr<IWICFormatConverter>			format;			//��ʽת����
		ComPtr<IWICBitmap>					bmp;			//λͼ

		if (!factory)						//��һ�ε��ú�������Ϊ�գ�����֮
		{
			CoCreateInstance(
				CLSID_WICImagingFactory2, nullptr,
				CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory));
		}

		factory->CreateDecoderFromFilename(	//���ļ��д���������
			filename.c_str(),
			nullptr,
			GENERIC_READ,
			WICDecodeMetadataCacheOnDemand,
			&decoder);

		decoder->GetFrame(0, &frame);		//�ӽ������л�ȡһ֡ͼ��

		factory->CreateFormatConverter(&format);
		format->Initialize(
			frame.Get(),
			GUID_WICPixelFormat32bppPBGRA,	//ת��ͼ���ʽΪRGBA��ʽ
			WICBitmapDitherTypeNone,
			nullptr,
			1.0,
			WICBitmapPaletteTypeCustom);

		factory->CreateBitmapFromSource(	//����ת����ɵ�ͼ�����ݴ���λͼ
			format.Get(),
			WICBitmapCacheOnDemand,
			&bmp);

		auto hr = dc->CreateBitmapFromWicBitmap(bmp.Get(), d2Bmp);
	}

	void MakeRelaition()				//ΪTarget���ø���ΪVisual��������
	{
		m_target->SetRoot(m_visual.Get());
		m_visual->SetContent(m_surface.Get());
	}

	void Update()						//��ʼ����
	{
		POINT offset;
		ComPtr<ID2D1DeviceContext> dc;
		m_surface->BeginDraw(nullptr, IID_PPV_ARGS(&dc), &offset);
		if (dc)
		{
			dc->Clear(0);
			dc->SetTransform(D2D1::Matrix3x2F::Translation((float)offset.x, (float)offset.y));

			D2D1_RECT_F rcBmp = {
				(m_width - m_bmpSize.width) / 2.f,
				(m_height - m_bmpSize.height) / 2.f,
				(m_width + m_bmpSize.width) / 2.f,
				(m_height + m_bmpSize.height) / 2.f,
			};

			dc->DrawBitmap(m_bmp.Get(), rcBmp);
			dc->DrawRectangle(rcBmp, m_brush.Get(),2);
		}
		m_surface->EndDraw();
		m_device->Commit();
	}

	void Resize(UINT width, UINT height)
	{
		m_width = width; m_height = height;
		m_surface->Resize(width, height);
		Update();
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

		m_hWnd = CreateWindowEx(WS_EX_NOREDIRECTIONBITMAP, wc.lpszClassName, L"DComposition��ʾͼƬ", WS_OVERLAPPEDWINDOW,
			x, y, w, h, nullptr, nullptr, m_hInstance, nullptr);

		SetWindowLongPtr(m_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		Initialize();

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