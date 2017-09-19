#include "MyLibfreenectDll.h"

static libfreenect2::SyncMultiFrameListener listener_1(libfreenect2::Frame::Color | libfreenect2::Frame::Ir | libfreenect2::Frame::Depth);
static libfreenect2::Freenect2Device *dev_1 = 0;
static libfreenect2::Freenect2 *freenect2 = 0;
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
