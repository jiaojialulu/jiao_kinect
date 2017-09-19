// TestDll.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
// ����ָ��DLL�к�����ָ��
typedef unsigned char* (*GetColorAndDepthData_1)();
typedef void(*InitDouble)();
typedef void(*CloseDouble)();

int _tmain(int argc, _TCHAR* argv[])
{
	HINSTANCE hlib = LoadLibrary(_T("MyLibfreenectDll.dll"));		// ���DLL��ʵ����HINSTANCE������ʵ���ľ����

	if (!hlib)
	{
		std::cout << "��ȡDLLʵ��ʧ�ܣ�" << std::endl;
		FreeLibrary(hlib);							// ���ú���FreeLibrary�ͷ�DLL��õ��ڴ档
		return -1;
	}
	// GetProcAddress��������DLL�ľ���ͺ�����������Ϊ������������Ӧ�ĺ���ָ�룬ͬʱ����ʹ��ǿת
	GetColorAndDepthData_1 FunGetColorAndDepthData_1 = (GetColorAndDepthData_1)GetProcAddress(hlib, "GetColorAndDepthData_1");
	GetColorAndDepthData_1 FunGetColorAndDepthData_2 = (GetColorAndDepthData_1)GetProcAddress(hlib, "GetColorAndDepthData_2");
	InitDouble FunInitDouble = (InitDouble)GetProcAddress(hlib, "InitDouble");
	CloseDouble FunCloseDouble = (CloseDouble)GetProcAddress(hlib, "CloseDouble");
	if (!FunGetColorAndDepthData_1&&!FunInitDouble&&!FunCloseDouble&&!FunGetColorAndDepthData_2)
	{
		std::cout << "����ָ��Ϊ�գ�" << std::endl;
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

