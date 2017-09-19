/*
对礼拜freenect生成dll以用于调用响应函数
*/

#ifdef LIBFREENECT_EXPORT
#define LIBFREENECT_API __declspec(dllexport)
#else
#define LIBFREENECT_API __declspec(dllimport)
#endif

// libfreenect2
#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>
#include <libfreenect2/registration.h>
#include <libfreenect2/packet_pipeline.h>
#include <libfreenect2/logger.h>

/// opencv
#include <opencv2/opencv.hpp>

using namespace cv;

extern "C"
{
	namespace MyLibFreenect
	{
		/// kinect double
		LIBFREENECT_API int InitDouble();
		LIBFREENECT_API void CloseDouble();
		LIBFREENECT_API unsigned char* GetColorAndDepthData_1();
		LIBFREENECT_API unsigned char* GetColorAndDepthData_2();



		/// kinect
		// 初始化
		LIBFREENECT_API void Init();
		// 关闭设备
		LIBFREENECT_API void Close();
		// 获取彩色图片
		LIBFREENECT_API unsigned char* GetColorData();
		// 显示彩色图片
		LIBFREENECT_API void ShowImage();
	}
}
