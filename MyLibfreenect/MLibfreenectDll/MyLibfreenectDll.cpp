#include "MyLibfreenectDll.h"

static libfreenect2::SyncMultiFrameListener listener_1(libfreenect2::Frame::Color | libfreenect2::Frame::Ir | libfreenect2::Frame::Depth);
static libfreenect2::SyncMultiFrameListener listener_2(libfreenect2::Frame::Color | libfreenect2::Frame::Ir | libfreenect2::Frame::Depth);
static libfreenect2::Freenect2Device *dev_1 = 0;
static libfreenect2::Freenect2Device *dev_2 = 0;
static libfreenect2::Freenect2 *freenect2 = 0;
static unsigned char *uchData_1 = new unsigned char[1920 * 1080 * 4 + 512 * 424 * 2];
static unsigned char *uchData_2 = new unsigned char[1920 * 1080 * 4 + 512 * 424 * 2];


LIBFREENECT_API int MyLibFreenect::InitDouble()
{
	// 实例化Freenect2类
	freenect2 = new libfreenect2::Freenect2();
	// 获取序列号
	std::string serial_1 = freenect2->getDeviceSerialNumber(0);
	std::string serial_2 = freenect2->getDeviceSerialNumber(1);
	if (serial_1 == ""&& serial_2 == "")
		return -1;
	// 获取设备
	dev_1 = freenect2->openDevice(serial_1);
	dev_2 = freenect2->openDevice(serial_2);
	if (!dev_1 || !dev_2)
		return -2;
	// 获取监听器
	dev_1->setColorFrameListener(&listener_1);
	dev_1->setIrAndDepthFrameListener(&listener_1);
	dev_2->setColorFrameListener(&listener_2);
	dev_2->setIrAndDepthFrameListener(&listener_2);
	if (dev_1->start() && dev_2->start())
		return 1;
	return 0;
}

LIBFREENECT_API void MyLibFreenect::CloseDouble()
{
	if (dev_1)
	{
		dev_1->stop();
		dev_1->close();
	}
	
	if (dev_2)
	{
		dev_2->stop();
		dev_2->close();
	}
}

LIBFREENECT_API unsigned char* MyLibFreenect::GetColorAndDepthData_1()
{
	libfreenect2::FrameMap frames_1;
	listener_1.waitForNewFrame(frames_1);
	if (frames_1.empty())
	{
		return nullptr;
	}
	libfreenect2::Frame *rgb_1 = frames_1[libfreenect2::Frame::Color];
	libfreenect2::Frame *depth_1 = frames_1[libfreenect2::Frame::Depth];

	memcpy(uchData_1, rgb_1->data, rgb_1->width*rgb_1->height * 4);
	memcpy(uchData_1 + rgb_1->width*rgb_1->height * 4, depth_1->data, depth_1->width*depth_1->height * 2);
	listener_1.release(frames_1);
	return uchData_1;
}

LIBFREENECT_API unsigned char* MyLibFreenect::GetColorAndDepthData_2()
{
	libfreenect2::FrameMap frames_2;
	listener_2.waitForNewFrame(frames_2);
	if (frames_2.empty())
	{
		return nullptr;
	}
	libfreenect2::Frame *rgb_2 = frames_2[libfreenect2::Frame::Color];
	libfreenect2::Frame *depth_2 = frames_2[libfreenect2::Frame::Depth];

	memcpy(uchData_2, rgb_2->data, rgb_2->width*rgb_2->height * 4);
	memcpy(uchData_2 + rgb_2->width*rgb_2->height * 4, depth_2->data, depth_2->width*depth_2->height * 2);
	listener_2.release(frames_2);
	return uchData_2;
}



#pragma region kinectSingle
static cv::Mat matColor;

LIBFREENECT_API void MyLibFreenect::Init()
{
	matColor = cv::Mat(1080, 1920, CV_8UC4);
	freenect2 = new libfreenect2::Freenect2();
	// kinect_1
	std::string serial_1 = freenect2->getDeviceSerialNumber(0);
	dev_1 = freenect2->openDevice(serial_1);
	dev_1->setColorFrameListener(&listener_1);
	dev_1->setIrAndDepthFrameListener(&listener_1);
	dev_1->start();
}

LIBFREENECT_API void MyLibFreenect::Close()
{
	dev_1->stop();
	dev_1->close();
}

LIBFREENECT_API unsigned char* MyLibFreenect::GetColorData()
{
	libfreenect2::FrameMap frames_1;
	listener_1.waitForNewFrame(frames_1);
	if (frames_1.empty())
	{
		return nullptr;
	}
	libfreenect2::Frame *rgb_1 = frames_1[libfreenect2::Frame::Color];

	memcpy(matColor.data, rgb_1->data, rgb_1->width*rgb_1->height*4);
	listener_1.release(frames_1);
	return matColor.data;
}

LIBFREENECT_API void MyLibFreenect::ShowImage()
{
	libfreenect2::FrameMap frames_1;
	cv::Mat matColor = cv::Mat(1080, 1920, CV_8UC4);
	while (true)
	{
		listener_1.waitForNewFrame(frames_1);
		if (frames_1.empty())
		{
			continue;
		}
		//listener_1.waitForNewFrame(frames_1);
		libfreenect2::Frame *ir_1 = frames_1[libfreenect2::Frame::Ir];
		libfreenect2::Frame *rgb_1 = frames_1[libfreenect2::Frame::Color];
		libfreenect2::Frame *depth_1 = frames_1[libfreenect2::Frame::Depth];

		matColor.data = rgb_1->data;

		cv::imshow("color", matColor);
		cvWaitKey(1);
		listener_1.release(frames_1);
	}
}
#pragma endregion