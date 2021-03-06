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
#define GL_DISPLAY
//#define SAVE_IMG
using namespace cv;
using namespace std;

#pragma comment(lib, "opengl32.lib")
double yaw, pitch, lastX, lastY; int ml;
static void on_mouse_button(GLFWwindow * win, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) ml = action == GLFW_PRESS;
}
static double clamp(double val, double lo, double hi) { return val < lo ? lo : val > hi ? hi : val; }
static void on_cursor_pos(GLFWwindow * win, double x, double y)
{
	if (ml)
	{
		yaw = clamp(yaw - (x - lastX), -120, 120);
		pitch = clamp(pitch + (y - lastY), -80, 80);
	}
	lastX = x;
	lastY = y;
}

typedef  unsigned char* (*BilateralFilter)(unsigned char* ImageIn, unsigned char* ImageOut, int width, int height, int value, int option);
typedef  unsigned char* (*GaussianFilter)(unsigned char* ImageIn, unsigned char* ImageOut, int width, int height, int sizeWidth, int sizeHeight, int sigmaX, int sigmaY, int option);

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

int main()
{

	// dll
	HINSTANCE hDll = LoadLibrary(L"DepthFilterDll.dll");
	if (!hDll)
	{
		return -1;
	}
	BilateralFilter DepthFilter = (BilateralFilter)GetProcAddress(hDll, "BilateralFilter");
	GaussianFilter DepthFilter2 = (GaussianFilter)GetProcAddress(hDll, "GaussianFilter");
	if (!DepthFilter || !DepthFilter2)
	{
		return -2;
	}

#ifdef GL_DISPLAY
	/// [gl display]
	// Open a GLFW window to display our output
	int a = glfwInit();
	GLFWwindow * win = glfwCreateWindow(1024, 768, "gl_win", nullptr, nullptr);
	glfwSetCursorPosCallback(win, on_cursor_pos);
	glfwSetMouseButtonCallback(win, on_mouse_button);
	glfwMakeContextCurrent(win);
#endif
#pragma region 初始化设备
	// 获取Kinect设备
	IKinectSensor* m_pKinectSensor;
	ICoordinateMapper*      m_pCoordinateMapper;
	CameraIntrinsics* m_pCameraIntrinsics = new CameraIntrinsics();
	HRESULT hr;
	hr = GetDefaultKinectSensor(&m_pKinectSensor);
	if (FAILED(hr))
	{
		return hr;
	}

	IMultiSourceFrameReader* m_pMultiFrameReader;
	IBodyFrameSource* m_pBodyFrameSource;
	IBodyFrameReader* m_pBodyFrameReader;
	if (m_pKinectSensor)
	{
		hr = m_pKinectSensor->Open();
		//Sleep(2000);
		if (SUCCEEDED(hr))
		{
			m_pKinectSensor->get_BodyFrameSource(&m_pBodyFrameSource);
			// 获取多数据源到读取器  
			hr = m_pKinectSensor->OpenMultiSourceFrameReader(
				FrameSourceTypes::FrameSourceTypes_Color |
				FrameSourceTypes::FrameSourceTypes_Infrared |
				FrameSourceTypes::FrameSourceTypes_Depth,
				&m_pMultiFrameReader);
		}
	}
	if (SUCCEEDED(hr))
	{
		hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
	}
	if (!m_pKinectSensor || FAILED(hr))
	{
		return E_FAIL;
	}
	// 三个数据帧及引用
	IDepthFrameReference* m_pDepthFrameReference = NULL;
	IColorFrameReference* m_pColorFrameReference = NULL;
	IDepthFrame* m_pDepthFrame = NULL;
	IColorFrame* m_pColorFrame = NULL;
	// 四个个图片格式
	Mat i_rgb(1080, 1920, CV_8UC4);      //注意：这里必须为4通道的图，Kinect的数据只能以Bgra格式传出
	//Mat i_depth_raw(424, 512, CV_16UC1);


	IMultiSourceFrame* m_pMultiFrame = NULL;

	DepthSpacePoint*        m_pDepthCoordinates = NULL;
	ColorSpacePoint*        m_pColorCoordinates = NULL;
	CameraSpacePoint*        m_pCameraCoordinates = NULL;

#pragma endregion

	m_pColorCoordinates = new ColorSpacePoint[512 * 424];
	m_pCameraCoordinates = new CameraSpacePoint[512 * 424];
	UINT16 *depthData = new UINT16[424 * 512];
	BYTE *bDepthData = new BYTE[424 * 512];
	BYTE *bgraData = new BYTE[1080 * 1920 * 4];
#pragma region 获取点云帧线程
	while (1)
	{

		//mutex1.lock();
		HRESULT hr = 0;
		// 获取新的一个多源数据帧
		hr = m_pMultiFrameReader->AcquireLatestFrame(&m_pMultiFrame);	// 如果m_pMultiFrame不为空，这句会将其置空
		if (m_pMultiFrame == NULL)
		{
			//mutex1.unlock();
			continue;
		}
		// 从多源数据帧中分离出彩色数据，深度数据和红外数据
		if (SUCCEEDED(hr))
			hr = m_pMultiFrame->get_ColorFrameReference(&m_pColorFrameReference);
		if (SUCCEEDED(hr))
			hr = m_pColorFrameReference->AcquireFrame(&m_pColorFrame);
		if (SUCCEEDED(hr))
			hr = m_pMultiFrame->get_DepthFrameReference(&m_pDepthFrameReference);
		if (SUCCEEDED(hr))
			hr = m_pDepthFrameReference->AcquireFrame(&m_pDepthFrame);


		// color拷贝到图片中
		UINT nColorBufferSize = 1920 * 1080 * 4;
		if (SUCCEEDED(hr))
			hr = m_pColorFrame->CopyConvertedFrameDataToArray(nColorBufferSize, bgraData, ColorImageFormat::ColorImageFormat_Bgra);

		//// filter
		//uchar* temp = new uchar[1920 * 1080 * 4];
		//bgraData =(DepthFilter2((uchar*)bgraData, temp, 1920, 1080, 11, 11, 0, 0, 4));

		// 获得depth的数据
		UINT nDepthBufferSize = 424 * 512;
		if (SUCCEEDED(hr))
		{
			hr = m_pDepthFrame->CopyFrameDataToArray(nDepthBufferSize, reinterpret_cast<UINT16*>(depthData));
		}
		//// filter
		//uchar* temp = new uchar[512 * 424*2];
		//depthData = reinterpret_cast<UINT16*>(DepthFilter((uchar*)depthData, temp, 512, 424,5,1));

		// 深度图映射到彩色图上
		if (SUCCEEDED(hr))
		{
			HRESULT hr = m_pCoordinateMapper->MapDepthFrameToColorSpace(512 * 424, depthData, 512 * 424, m_pColorCoordinates);		// 注意这里的彩色点数要写成424*512，至于为什么。。。可能是为了代表下一个参数colorSpacePoints的大小
		}


#ifdef GL_DISPLAY
		glfwPollEvents();
		// Set up a perspective transform in a space that we can rotate by clicking and dragging the mouse
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60, (float)1280 / 960, 0.01f, 20.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(0, 0, 0, 0, 0, 1, 0, -1, 0);
		glTranslatef(0, 0, +0.5f);
		glRotated(pitch, 1, 0, 0);
		glRotated(yaw, 0, 1, 0);
		glTranslatef(0, 0, -0.5f);

		// We will render our depth data as a set of points in 3D space
		glPointSize(2);
		glEnable(GL_DEPTH_TEST);
		glBegin(GL_POINTS);
#endif
		// 深度图像空间映射到相机空间
		if (SUCCEEDED(hr))
		{
			HRESULT hr = m_pCoordinateMapper->MapDepthFrameToCameraSpace(512 * 424, depthData, 512 * 424, m_pCameraCoordinates);
		}
		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < 512 * 424; i++)
			{
				CameraSpacePoint p = m_pCameraCoordinates[i];
				ColorSpacePoint p_bgra = m_pColorCoordinates[i];
				int colorX = static_cast<int>(p_bgra.X + 0.5f);
				int colorY = static_cast<int>(p_bgra.Y + 0.5f);
				if ((colorX >= 0 && colorX < 1920) && (colorY >= 0 && colorY < 1080))
				{
					float bgra = ((float *)bgraData)[(colorY * 1920 + colorX)];
					//cout << bgra << endl;
					if (bgra != 0 && p.X >-0.2&&p.X<0.5 && p.Y>-1 && p.Y<1.0f && p.Z <1.5f && p.Z > 0.5f)
					{

#ifdef GL_DISPLAY
						GLubyte *rgb = new GLubyte();
						rgb[2] = bgraData[(colorY * 1920 + colorX) * 4];
						rgb[1] = bgraData[(colorY * 1920 + colorX) * 4 + 1];
						rgb[0] = bgraData[(colorY * 1920 + colorX) * 4 + 2];
						// 显示点
						glColor3ubv(rgb);
						glVertex3f(-p.X, -p.Y, p.Z);
#endif
					}
				}
			}
		}

#pragma region 保存图片
#ifdef SAVE_IMG
		imwrite("depth.png", i_depth_raw);
		imwrite("color.jpg", i_rgb);
		imwrite("depth2rgb.jpg", i_depthToRgb);
#endif
#pragma endregion
#ifdef GL_DISPLAY
		glEnd();
		glfwSwapBuffers(win);
#endif

		// 释放资源
		SafeRelease(m_pColorFrame);
		SafeRelease(m_pDepthFrame);
		SafeRelease(m_pColorFrameReference);
		SafeRelease(m_pDepthFrameReference);
		SafeRelease(m_pMultiFrame);

		//mutex1.unlock();
	}
#pragma endregion
	// 关闭窗口，设备
	cv::destroyAllWindows();
	SafeRelease(m_pCoordinateMapper);
	m_pKinectSensor->Close();
	std::system("pause");
	return 0;
}
