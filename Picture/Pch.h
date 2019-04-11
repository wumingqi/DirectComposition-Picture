#define WIN32_LEAN_AND_MEAN
#include <wrl.h>
using Microsoft::WRL::ComPtr;

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
};