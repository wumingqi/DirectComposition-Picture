#include "Pch.h"
#include "Application.h"


void Application::CreateDeviceAndResources()
{
	D3D_FEATURE_LEVEL featureLevelSupported;
	ComPtr<ID3D11Device> d3dDevice;
	D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0, D3D11_SDK_VERSION, d3dDevice.GetAddressOf(), &featureLevelSupported, nullptr);

	ComPtr<IDXGIDevice> dxgiDevice;
	d3dDevice.As(&dxgiDevice);

	ComPtr<ID2D1Device> d2dDevice;
	D2D1CreateDevice(dxgiDevice.Get(), D2D1::CreationProperties(D2D1_THREADING_MODE_SINGLE_THREADED, D2D1_DEBUG_LEVEL_NONE, D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS), &d2dDevice);

	d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &m_dc);
	m_dc->CreateSolidColorBrush(D2D1::ColorF(0x2255ff), &m_brush);

	//创建WIC工厂
	CoCreateInstance(
		CLSID_WICImagingFactory2, nullptr,
		CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_factory));

	

	DCompositionCreateDevice3(d2dDevice.Get(), IID_PPV_ARGS(&m_device));

	m_device->CreateTargetForHwnd(m_hWnd, FALSE, &m_target);
	m_device->CreateVisual(&m_visual);
	m_device->CreateVirtualSurface(m_width, m_height, DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ALPHA_MODE_PREMULTIPLIED, &m_surface);

	m_target->SetRoot(m_visual.Get());
	m_visual->SetContent(m_surface.Get());

	LoadPicture(IDB_Github, L"JPG");
}

void Application::MoveVisual(float x, float y)
{
	m_visual->SetOffsetX(x);
	m_visual->SetOffsetY(y);
	m_device->Commit();
}

void Application::LoadPicture(wstring filename)
{
	try
	{
		if(!filename.empty())
			Helper::LoadPictureFromFile(m_factory.Get(), m_dc.Get(), &m_bmp.data, filename);
	}
	catch(FileNotSupportException e)
	{
		MessageBox(m_hWnd, e.msg.c_str(), L"不支持此文件", MB_ICONERROR);
		return;
	}
	m_bmp.sizeu = m_bmp.data->GetPixelSize();
	m_bmp.sizef = m_bmp.data->GetSize();
	Update();
}

void Application::LoadPicture(int resourceId, LPCTSTR type)
{
	Helper::LoadPictureFromResource(m_factory.Get(), m_dc.Get(), m_bmp.data.ReleaseAndGetAddressOf(), MAKEINTRESOURCE(resourceId), type);

	m_bmp.sizeu = m_bmp.data->GetPixelSize();
	m_bmp.sizef = m_bmp.data->GetSize();
	Update();
}

wstring Application::OpenFile()
{
	OPENFILENAME ofn;       // common dialog box structure
	TCHAR szFile[260];       // buffer for file name

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = L'\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"图像文件\0*.jpg;*.png;*bmp;*tiff\0所有文件\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = L"选择图像文件";
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn) == TRUE)
	{
		return szFile;
	}
	else
	{
		return L"";
	}
	
}

void Application::Update()
{
	m_surface->Resize(m_bmp.sizeu.width, m_bmp.sizeu.height);

	POINT offset;
	ComPtr<ID2D1DeviceContext> dc;
	auto hr = m_surface->BeginDraw(nullptr, IID_PPV_ARGS(&dc), &offset);
	if (dc)
	{
		dc->Clear(0);
		dc->SetTransform(D2D1::Matrix3x2F::Translation((float)offset.x, (float)offset.y));

		D2D1_RECT_F rcBmp = { 0,0, m_bmp.sizef.width,m_bmp.sizef.height };

		dc->DrawBitmap(m_bmp.data.Get());
		dc->DrawRectangle(rcBmp, m_brush.Get(), 4);
	}
	m_surface->EndDraw();
	m_device->Commit();
}