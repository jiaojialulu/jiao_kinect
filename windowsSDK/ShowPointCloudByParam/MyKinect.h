#include <time.h>
#include "kinect.h"
#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <GLFW\glfw3.h>
#include <glut.h>
#include <thread>
#include <mutex>
#include <queue>

using namespace cv;
using namespace std;

// 安全释放指针
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}


class MyKinect
{
public:
	MyKinect();
	~MyKinect();
	Mat getColorMat();
	Mat getDepthMat();

private:
	void getNewFrame();
	void update();
	void readParam();
	void showCloud();
	
	// 获取的各个数据源大小
	int m_iColorWidth, m_iColorHeight, m_iDepthHeight, m_iDepthWidth;	

	// 获取Kinect设备
	IKinectSensor* m_pKinectSensor = NULL;
	ICoordinateMapper*      m_pCoordinateMapper = NULL;
	IMultiSourceFrameReader* m_pMultiFrameReader = NULL;
	
	// 三个数据帧及引用
	IDepthFrameReference* m_pDepthFrameReference = NULL;
	IColorFrameReference* m_pColorFrameReference = NULL;
	IDepthFrame* m_pDepthFrame = NULL;
	IColorFrame* m_pColorFrame = NULL;
	IMultiSourceFrame* m_pMultiFrame = NULL;

	DepthSpacePoint*        m_pDepthCoordinates = NULL;
	ColorSpacePoint*        m_pColorCoordinates = NULL;
	CameraSpacePoint*        m_pCameraCoordinates = NULL;

	// 四个个图片格式
	Mat m_matRgb;      //注意：这里必须为4通道的图，Kinect的数据只能以Bgra格式传出
	Mat m_matDepth;

	// data
	UINT16 *depthData = NULL;
	BYTE *bgraData = NULL;
};