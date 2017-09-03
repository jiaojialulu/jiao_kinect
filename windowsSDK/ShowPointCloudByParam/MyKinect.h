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

// ��ȫ�ͷ�ָ��
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
	
	// ��ȡ�ĸ�������Դ��С
	int m_iColorWidth, m_iColorHeight, m_iDepthHeight, m_iDepthWidth;	

	// ��ȡKinect�豸
	IKinectSensor* m_pKinectSensor = NULL;
	ICoordinateMapper*      m_pCoordinateMapper = NULL;
	IMultiSourceFrameReader* m_pMultiFrameReader = NULL;
	
	// ��������֡������
	IDepthFrameReference* m_pDepthFrameReference = NULL;
	IColorFrameReference* m_pColorFrameReference = NULL;
	IDepthFrame* m_pDepthFrame = NULL;
	IColorFrame* m_pColorFrame = NULL;
	IMultiSourceFrame* m_pMultiFrame = NULL;

	DepthSpacePoint*        m_pDepthCoordinates = NULL;
	ColorSpacePoint*        m_pColorCoordinates = NULL;
	CameraSpacePoint*        m_pCameraCoordinates = NULL;

	// �ĸ���ͼƬ��ʽ
	Mat m_matRgb;      //ע�⣺�������Ϊ4ͨ����ͼ��Kinect������ֻ����Bgra��ʽ����
	Mat m_matDepth;

	// data
	UINT16 *depthData = NULL;
	BYTE *bgraData = NULL;
};