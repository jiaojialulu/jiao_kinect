/*
�����freenect����dll�����ڵ�����Ӧ����
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
		// ��ʼ��
		LIBFREENECT_API void Init();
		// �ر��豸
		LIBFREENECT_API void Close();
		// ��ȡ��ɫͼƬ
		LIBFREENECT_API unsigned char* GetColorData();
		// ��ʾ��ɫͼƬ
		LIBFREENECT_API void ShowImage();
	}
}
