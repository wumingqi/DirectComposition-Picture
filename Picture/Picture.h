#pragma once
struct Picture
{
	//Composition��Ϣ
	ComPtr<IDCompositionSurface> surface;			//������ʾͼƬ��surface
	ComPtr<IDCompositionVisual2> visual;			//��ͼ
	
	//λͼ��Ϣ
	ComPtr<ID2D1Bitmap1>	data;					//ͼƬ����
	D2D1_SIZE_F				sizef;					//float���͵ĳߴ�
	D2D1_SIZE_U				sizeu;					//UINT���͵ĳߴ�

	//ͼƬ�ڿͻ�����λ�úͳߴ�
	D2D1_POINT_2F			offset;					//���Ͻǵ�ƫ��	
	D2D1_SIZE_F				sizev;					//ʵ����ʾ�Ĵ�С

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

