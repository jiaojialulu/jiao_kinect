// TestDll.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
// 定义指向DLL中函数的指针
typedef unsigned char* (*GetColorData)();
typedef void(*Init)();

int _tmain(int argc, _TCHAR* argv[])
{
	HINSTANCE hlib = LoadLibrary(_T("MLibfreenectDll.dll"));		// 获得DLL的实例（HINSTANCE类型是实例的句柄）

	if (!hlib)
	{
		std::cout << "获取DLL实例失败！" << std::endl;
		FreeLibrary(hlib);							// 调用函数FreeLibrary释放DLL获得的内存。
		return -1;
	}
	// GetProcAddress函数利用DLL的句柄和函数的名称作为参数，返回相应的函数指针，同时必须使用强转
	GetColorData FunGetColorData = (GetColorData)GetProcAddress(hlib, "GetColorData");
	Init FuncInit = (Init)GetProcAddress(hlib, "Init");
	if (!FunGetColorData&&!FuncInit)
	{
		std::cout << "函数指针为空！" << std::endl;
		FreeLibrary(hlib);
		return -2;
	}
	FuncInit();
	
	cv::Mat matColor = cv::Mat(1080, 1920, CV_8UC4);
	while (true)
	{
		if ((matColor.data = FunGetColorData())== nullptr)
			continue;
		cv::imshow("color", matColor);
		cvWaitKey(1);
	}

	system("pause");
	return 0;
}

