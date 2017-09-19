/*
使用礼拜freenect的第一个例程，显示原始的图片数据
*/

// libfreenect2
#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>
#include <libfreenect2/registration.h>
#include <libfreenect2/packet_pipeline.h>
#include <libfreenect2/logger.h>

/// opencv
#include <opencv2/opencv.hpp>

int main()
{
	// kinect_1
	libfreenect2::Freenect2 freenect2;
	libfreenect2::Freenect2Device *dev_1 = 0;
	libfreenect2::PacketPipeline *pipeline_1 = 0;
	std::string serial_1 = freenect2.getDeviceSerialNumber(0);
	dev_1 = freenect2.openDevice(serial_1);
	int types = libfreenect2::Frame::Color | libfreenect2::Frame::Ir | libfreenect2::Frame::Depth;
	libfreenect2::SyncMultiFrameListener listener_1(types);
	libfreenect2::FrameMap frames_1;
	dev_1->setColorFrameListener(&listener_1);
	dev_1->setIrAndDepthFrameListener(&listener_1);
	dev_1->start();

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


	return 0;
}

