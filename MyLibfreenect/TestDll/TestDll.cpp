// TestDll.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
// ����ָ��DLL�к�����ָ��
typedef unsigned char* (*GetColorData)();
typedef void(*Init)();

int _tmain(int argc, _TCHAR* argv[])
{
	HINSTANCE hlib = LoadLibrary(_T("MLibfreenectDll.dll"));		// ���DLL��ʵ����HINSTANCE������ʵ���ľ����

	if (!hlib)
	{
		std::cout << "��ȡDLLʵ��ʧ�ܣ�" << std::endl;
		FreeLibrary(hlib);							// ���ú���FreeLibrary�ͷ�DLL��õ��ڴ档
		return -1;
	}
	// GetProcAddress��������DLL�ľ���ͺ�����������Ϊ������������Ӧ�ĺ���ָ�룬ͬʱ����ʹ��ǿת
	GetColorData FunGetColorData = (GetColorData)GetProcAddress(hlib, "GetColorData");
	Init FuncInit = (Init)GetProcAddress(hlib, "Init");
	if (!FunGetColorData&&!FuncInit)
	{
		std::cout << "����ָ��Ϊ�գ�" << std::endl;
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

