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
		ComPtr<IWICBitmapFrameDecode>		frame;			//һ֡ͼ������
		ComPtr<IWICFormatConverter>			format;			//��ʽת����
		ComPtr<IWICBitmap>					bmp;			//λͼ
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

public:
	static void LoadPictureFromFile(IWICImagingFactory2* factory, ID2D1DeviceContext* dc, ID2D1Bitmap1** d2Bmp, wstring filename)
	{
		ComPtr<IWICBitmapDecoder> decoder;		//������
		auto hr = factory->CreateDecoderFromFilename(	//���ļ��д���������
			filename.c_str(),
			nullptr,
			GENERIC_READ,
			WICDecodeMetadataCacheOnDemand,
			&decoder);

		if (FAILED(hr))
		{
			throw FileNotSupportException(L"��֧�ֵ��ļ���" + filename);
			return;
		}

		CreateBitmapFromDecoder(factory, decoder.Get(), dc, d2Bmp);
	}

	static void LoadPictureFromResource(IWICImagingFactory2* factory, ID2D1DeviceContext* dc, ID2D1Bitmap1** d2Bmp, LPCWSTR name, LPCWSTR type)
	{
		//1��������Դ��λ�úʹ�С
		auto imageResHandle = FindResource(nullptr, name, type);
		auto imageResDataHandle = LoadResource(nullptr, imageResHandle);
		auto pImageFile = LockResource(imageResDataHandle);
		auto imageFileSize = SizeofResource(nullptr, imageResHandle);

		//2��������
		ComPtr<IWICStream> stream;
		factory->CreateStream(&stream);
		stream->InitializeFromMemory(reinterpret_cast<WICInProcPointer>(pImageFile), imageFileSize);

		//3������������
		ComPtr<IWICBitmapDecoder> decoder;		//������
		factory->CreateDecoderFromStream(
			stream.Get(),
			nullptr,
			WICDecodeMetadataCacheOnDemand,
			&decoder);

		CreateBitmapFromDecoder(factory, decoder.Get(), dc, d2Bmp);
	}
};