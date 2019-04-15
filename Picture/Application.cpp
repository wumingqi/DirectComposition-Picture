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

	//创建WIC工厂
	CoCreateInstance(
		CLSID_WICImagingFactory2, nullptr,
		CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_factory));

	DCompositionCreateDevice3(d2dDevice.Get(), IID_PPV_ARGS(&m_device));

	m_device->CreateTargetForHwnd(m_hWnd, FALSE, &m_target);
	m_device->CreateVisual(&m_root);

	m_target->SetRoot(m_root.Get());

	m_bmps.push_front(LoadPicture(IDB_QQ, L"PNG"));
	m_bmps.push_front(LoadPicture(IDB_Github, L"JPG"));
	for (auto bmp : m_bmps)
	{
		m_root->AddVisual(bmp.visual.Get(), true, nullptr);
	}
	
	m_device->Commit();
}

Picture Application::LoadPicture(wstring filename)
{
	ComPtr<ID2D1Bitmap1> bmp;
	try
	{
		if (!filename.empty())
			Helper::LoadPictureFromFile(m_factory.Get(), m_dc.Get(), &bmp, filename);
		else
			return {};
	}
	catch(FileNotSupportException e)
	{
		MessageBox(m_hWnd, e.msg.c_str(), L"不支持此文件", MB_ICONERROR);
		return Picture();
	}

	Picture pic;
	pic.InitializeFromBmp(bmp.Get(), m_device.Get());
	return pic;
}

Picture Application::LoadPicture(int resourceId, LPCTSTR type)
{
	ComPtr<ID2D1Bitmap1> bmp;
	Helper::LoadPictureFromResource(m_factory.Get(), m_dc.Get(), &bmp, MAKEINTRESOURCE(resourceId), type);

	Picture pic;
	pic.InitializeFromBmp(bmp.Get(), m_device.Get());
	return pic;
}



void Application::Update()
{
}