#include "Pch.h"
#include "Picture.h"

void Picture::InitializeFromBmp(ID2D1Bitmap1* bmp, IDCompositionDesktopDevice* device)
{
	UpdateBmpInfo(bmp);

	device->CreateVisual(&visual);
	device->CreateSurface(sizeu.width, sizeu.height, DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ALPHA_MODE_PREMULTIPLIED, &surface);
	visual->SetContent(surface.Get());

	UpdateSurface();

	device->Commit();
}

void Picture::Move(D2D1_POINT_2F offset)
{
	this->offset = offset;
}

void Picture::Resize(D2D1_SIZE_F size)
{
	this->sizev = sizev;
}

void Picture::UpdateBmpInfo(ID2D1Bitmap1* bmp)
{
	this->data = bmp;
	this->sizef = bmp->GetSize();
	this->sizeu = bmp->GetPixelSize();
	this->offset = {};
	this->sizev = this->sizef;
}

void Picture::UpdateSurface()
{
	POINT offset;
	ComPtr<ID2D1DeviceContext> dc;
	auto hr = surface->BeginDraw(nullptr, IID_PPV_ARGS(&dc), &offset);
	if (dc)
	{
		dc->Clear(0);
		dc->SetTransform(D2D1::Matrix3x2F::Translation((float)offset.x, (float)offset.y));

		D2D1_RECT_F rcBmp = { 0,0, sizef.width,sizef.height };

		dc->DrawBitmap(data.Get());

		ComPtr<ID2D1SolidColorBrush> brush;
		dc->CreateSolidColorBrush(D2D1::ColorF(0x2255ff), &brush);
		dc->DrawRectangle(rcBmp, brush.Get(), 4);
	}
	surface->EndDraw();
}

bool Picture::Contains(D2D1_POINT_2F pos)
{
	auto a = pos.x - offset.x;
	auto b = pos.y - offset.y;
	if (a < sizef.width && a>0 && b< sizef.height&&b>0)
		return true;
	else
		return false;
}
