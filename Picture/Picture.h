#pragma once
struct Picture
{
	//Composition信息
	ComPtr<IDCompositionSurface> surface;			//用于显示图片的surface
	ComPtr<IDCompositionVisual2> visual;			//视图
	
	//位图信息
	ComPtr<ID2D1Bitmap1>	data;					//图片数据
	D2D1_SIZE_F				sizef;					//float类型的尺寸
	D2D1_SIZE_U				sizeu;					//UINT类型的尺寸

	//图片在客户区的位置和尺寸
	D2D1_POINT_2F			offset;					//左上角的偏移	
	D2D1_SIZE_F				sizev;					//实际显示的大小

	void InitializeFromBmp(ID2D1Bitmap1* bmp, IDCompositionDesktopDevice* device);
	void Move(D2D1_POINT_2F offset);
	void Resize(D2D1_SIZE_F size);

	void UpdateBmpInfo(ID2D1Bitmap1* bmp);
	void UpdateSurface();

	bool Contains(D2D1_POINT_2F pos);
	bool operator==(Picture another)
	{
		return this->visual.Get() == another.visual.Get();
	}
};

