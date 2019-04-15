#define WIN32_LEAN_AND_MEAN
#include <wrl.h>
using Microsoft::WRL::ComPtr;

#include <list>
#include <string>
using std::wstring;

#include <d3d11.h>
#include <dcomp.h>
#include <d2d1_3.h>
#include <wincodec.h>
#include <shellapi.h>
#include <commdlg.h>

#pragma comment(lib, "d3d11")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dcomp")
#pragma comment(lib, "windowscodecs")
#pragma comment(lib, "Shell32")
#pragma comment(lib, "ComDlg32")


#include "resource.h"
#include "Exceptions.h"

class Helper
{
private:
	static void CreateBitmapFromDecoder(IWICImagingFactory2* factory, IWICBitmapDecoder* decoder, ID2D1DeviceContext* dc, ID2D1Bitmap1** d2Bmp)
	{
		ComPtr<IWICBitmapFrameDecode>		frame;			//一帧图像数据
		ComPtr<IWICFormatConverter>			format;			//格式转换器
		ComPtr<IWICBitmap>					bmp;			//位图
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

public:
	static void LoadPictureFromFile(IWICImagingFactory2* factory, ID2D1DeviceContext* dc, ID2D1Bitmap1** d2Bmp, wstring filename)
	{
		ComPtr<IWICBitmapDecoder> decoder;		//解码器
		auto hr = factory->CreateDecoderFromFilename(	//从文件中创建解码器
			filename.c_str(),
			nullptr,
			GENERIC_READ,
			WICDecodeMetadataCacheOnDemand,
			&decoder);

		if (FAILED(hr))
		{
			throw FileNotSupportException(L"不支持的文件：" + filename);
			return;
		}

		CreateBitmapFromDecoder(factory, decoder.Get(), dc, d2Bmp);
	}

	static void LoadPictureFromResource(IWICImagingFactory2* factory, ID2D1DeviceContext* dc, ID2D1Bitmap1** d2Bmp, LPCWSTR name, LPCWSTR type)
	{
		//1、锁定资源的位置和大小
		auto imageResHandle = FindResource(nullptr, name, type);
		auto imageResDataHandle = LoadResource(nullptr, imageResHandle);
		auto pImageFile = LockResource(imageResDataHandle);
		auto imageFileSize = SizeofResource(nullptr, imageResHandle);

		//2、创建流
		ComPtr<IWICStream> stream;
		factory->CreateStream(&stream);
		stream->InitializeFromMemory(reinterpret_cast<WICInProcPointer>(pImageFile), imageFileSize);

		//3、创建解码器
		ComPtr<IWICBitmapDecoder> decoder;		//解码器
		factory->CreateDecoderFromStream(
			stream.Get(),
			nullptr,
			WICDecodeMetadataCacheOnDemand,
			&decoder);

		CreateBitmapFromDecoder(factory, decoder.Get(), dc, d2Bmp);
	}


	static wstring OpenFile(HWND hWnd)
	{
		OPENFILENAME ofn;       // common dialog box structure
		TCHAR szFile[260];       // buffer for file name

		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = hWnd;
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

	static ComPtr<IDCompositionEffect> CreateEffect1(IDCompositionDevice2* device,float degree, float centerX, float centerY, float centerZ)
	{
		ComPtr<IDCompositionAnimation> ani;
		device->CreateAnimation(&ani);
		ani->AddCubic(0.0, 0.0, degree, 0.0, 0.0);
		ani->End(1.0, degree);

		ComPtr<IDCompositionRotateTransform3D> eff;
		device->CreateRotateTransform3D(&eff);
		eff->SetAngle(ani.Get());
		eff->SetCenterX(centerX);
		eff->SetCenterY(centerY);
		eff->SetCenterZ(centerZ);

		eff->SetAxisZ(1);

		return { eff.Get() };
	}


	static void Log(wstring msg)
	{
		static auto hStd = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD n;
		WriteConsole(hStd, msg.data(), msg.length(), &n, nullptr);
	}
};