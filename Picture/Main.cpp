#include "Pch.h"
#include "Application.h"

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, INT nCmdShow)
{
	//AllocConsole();
	CoInitialize(nullptr);
	return Application(1280, 720, hInstance).Run(nCmdShow);
}