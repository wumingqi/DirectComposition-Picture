#include "Pch.h"
#include "Application.h"



#define HANDLE_MSG(msg) case msg: return app.Handle_##msg(wParam, lParam);
LRESULT CALLBACK Application::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	auto& app = *(reinterpret_cast<Application*>(GetWindowLongPtr(hWnd, GWLP_USERDATA)));
	switch (msg)
	{
		//HANDLE_MSG(WM_PAINT);
		HANDLE_MSG(WM_SIZE);
		HANDLE_MSG(WM_KEYDOWN);
		HANDLE_MSG(WM_LBUTTONDOWN);
		HANDLE_MSG(WM_LBUTTONUP);
		HANDLE_MSG(WM_MOUSEMOVE);
		HANDLE_MSG(WM_MOUSEWHEEL);
		HANDLE_MSG(WM_DROPFILES);
		HANDLE_MSG(WM_COMMAND);
		HANDLE_MSG(WM_DESTROY);
	default:return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}
#undef HANDLE_MSG

///<summary>����WM_PAINT��Ϣ�����ͻ�����Ҫ������ʱ����</summary>
///<param name="wParam">This parameter is not used.</param>
///<param name="lParam">This parameter is not used.</param>
LRESULT Application::Handle_WM_PAINT(WPARAM wParam, LPARAM lParam)
{
	ValidateRect(m_hWnd, nullptr);
	return 0;
}

///<summary>����WM_LBUTTONDOWN��Ϣ������������ʱ����</summary>
///<param name="wParam">��ʾ�Ƿ������ⰴ�����£���MK_CONTROL��</param>
///<param name="lParam">����ڿͻ������ϽǵĹ�������</param>
LRESULT Application::Handle_WM_LBUTTONDOWN(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

///<summary>����WM_LBUTTONUP��Ϣ���������ɿ�ʱ����</summary>
///<param name="wParam">��ʾ�Ƿ������ⰴ�����£���MK_CONTROL��</param>
///<param name="lParam">����ڿͻ������ϽǵĹ�������</param>
LRESULT Application::Handle_WM_LBUTTONUP(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

///<summary>����WM_MOUSE��Ϣ������ڿͻ����ƶ�ʱ����</summary>
///<param name="wParam">��ʾ�Ƿ������ⰴ�����£���MK_CONTROL��</param>
///<param name="lParam">����ڿͻ������ϽǵĹ�������</param>
LRESULT Application::Handle_WM_MOUSEMOVE(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

///<summary>����WM_SIZE��Ϣ�������ڴ�С�ı�ʱ����</summary>
///<param name="wParam">��ʾ��С�ı�����ͣ���SIZE_MAXIMIZED��SIZE_HIDE��</param>
///<param name="lParam">���ڿͻ����Ĵ�С</param>
LRESULT Application::Handle_WM_SIZE(WPARAM wParam, LPARAM lParam)
{
	UINT width = LOWORD(lParam);
	UINT height = HIWORD(lParam);
	m_width = width; m_height = height;

	MoveVisual();

	return 0;
}

///<summary>����WM_KEYDOWN��Ϣ�������̰�������ʱ����</summary>
///<param name="wParam">��ϵͳ���������ֵ��Virtual-Key Code��</param>
///<param name="lParam">������Ϣ</param>
LRESULT Application::Handle_WM_KEYDOWN(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_F1:
		LoadPicture(IDB_QQ, L"PNG");
		break;
	case VK_F2:
		LoadPicture(IDB_Github, L"JPG");
		break;
	}
	return 0;
}

///<summary>����WM_MOUSEWHEEL��Ϣ���������ֹ���ʱ����</summary>
///<param name="wParam">
///���ֱ������ֹ����ķ�����ֵ������ǰ������ֵ��������
///���ֱ���ʱ����������������£�MK_CONTROL���ȣ�
///</param>
///<param name="lParam">��������λ�ã��������Ļ���Ͻ�</param>
LRESULT Application::Handle_WM_MOUSEWHEEL(WPARAM wParam, LPARAM lParam)
{
	auto flag = HIWORD(wParam) & 0x8000;
	if (flag)
	{
		
	}
	else
	{

	}
	Update();
	return 0;
}

LRESULT Application::Handle_WM_DESTROY(WPARAM wParam, LPARAM lParam)
{
	PostQuitMessage(0);
	return 0;
}

LRESULT Application::Handle_WM_DROPFILES(WPARAM wParam, LPARAM lParam)
{
	auto hDrop = (HDROP)wParam;
	TCHAR filename[MAX_PATH];
	DragQueryFile(hDrop, 0, filename, _countof(filename));
	LoadPicture(filename);
	return 0;
}

LRESULT Application::Handle_WM_COMMAND(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case ID_OPENFILE:
		LoadPicture(OpenFile());
		break;
	case ID_SAVEFILE:
		break;
	case ID_CLOSE:
		DestroyWindow(m_hWnd);
		break;
	}
	return 0;
}
