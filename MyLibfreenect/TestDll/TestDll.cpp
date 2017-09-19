// TestDll.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
// 定义指向DLL中函数的指针
typedef unsigned char* (*GetColorAndDepthData_1)();
typedef void(*InitDouble)();
typedef void(*CloseDouble)();

int _tmain(int argc, _TCHAR* argv[])
{
	HINSTANCE hlib = LoadLibrary(_T("MyLibfreenectDll.dll"));		// 获得DLL的实例（HINSTANCE类型是实例的句柄）

	if (!hlib)
	{
		std::cout << "获取DLL实例失败！" << std::endl;
		FreeLibrary(hlib);							// 调用函数FreeLibrary释放DLL获得的内存。
		return -1;
	}
	// GetProcAddress函数利用DLL的句柄和函数的名称作为参数，返回相应的函数指针，同时必须使用强转
	GetColorAndDepthData_1 FunGetColorAndDepthData_1 = (GetColorAndDepthData_1)GetProcAddress(hlib, "GetColorAndDepthData_1");
	GetColorAndDepthData_1 FunGetColorAndDepthData_2 = (GetColorAndDepthData_1)GetProcAddress(hlib, "GetColorAndDepthData_2");
	InitDouble FunInitDouble = (InitDouble)GetProcAddress(hlib, "InitDouble");
	CloseDouble FunCloseDouble = (CloseDouble)GetProcAddress(hlib, "CloseDouble");
	if (!FunGetColorAndDepthData_1&&!FunInitDouble&&!FunCloseDouble&&!FunGetColorAndDepthData_2)
	{
		std::cout << "函数指针为空！" << std::endl;
		FreeLibrary(hlib);
		return -2;
	}
	FunInitDouble();

	cv::Mat matColor_1 = cv::Mat(1080, 1920, CV_8UC4);
	cv::Mat matColor_2 = cv::Mat(1080, 1920, CV_8UC4);
	while (true)
	{
		if ((matColor_1.data = FunGetColorAndDepthData_1()) == nullptr)
			continue;
		cv::imshow("color_1", matColor_1);
		if (cvWaitKey(10) == 'q')
		{
			FunCloseDouble();
			break;
		}

		if ((matColor_2.data = FunGetColorAndDepthData_2()) == nullptr)
			continue;
		cv::imshow("color_2", matColor_2);
		if (cvWaitKey(10) == 'q')
		{
			FunCloseDouble();
			break;
		}
	}
	FunCloseDouble();
	system("pause");
	return 0;
}

