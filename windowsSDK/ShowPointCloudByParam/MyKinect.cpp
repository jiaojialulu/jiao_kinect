#include "MyKinect.h"

MyKinect::MyKinect()
{
	HRESULT hr;
	hr = GetDefaultKinectSensor(&m_pKinectSensor);
	if (FAILED(hr))
	{
		cout << "error!1";
		return;
	}
	if (m_pKinectSensor)
	{
		hr = m_pKinectSensor->Open();
		//Sleep(2000);
		if (SUCCEEDED(hr))
		{
			// 获取多数据源到读取器  
			hr = m_pKinectSensor->OpenMultiSourceFrameReader(
				FrameSourceTypes::FrameSourceTypes_Color |
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
		cout << "error!2";
		return;
	}

	bgraData = new BYTE[1080 * 1920 * 4];
	depthData = new UINT16[424 * 512];
	m_iColorWidth = 1920;
	m_iColorHeight = 1080;
	m_iDepthHeight = 424;
	m_iDepthWidth = 512;

	update();
	cout << "kinect打开成功！";
}

MyKinect::~MyKinect()
{
	std::cout << "关闭kinect" << endl;
	// done with frame reader
	if (!m_pMultiFrameReader)
	{
		SafeRelease(m_pMultiFrameReader);
	}

	// done with coordinate mapper
	if (!m_pCoordinateMapper)
	{
		SafeRelease(m_pCoordinateMapper);
	}
	HRESULT hr;
	hr = m_pKinectSensor->Close();
	if (FAILED(hr))
		std::cout << "关闭kinect 失败！！！" << endl;

	if (!m_pKinectSensor)
	{
		SafeRelease(m_pKinectSensor);
	}
}

void MyKinect::update()
{
	mutex mtx1;

	thread th = thread(bind(&MyKinect::getNewFrame, this));
	th.detach();
	/*thread th1 = thread([&]{
		while (true)
		{
			if (mtx1.try_lock())
			{
				
				mtx1.unlock();
			}
		}
	});*/
}

void MyKinect::getNewFrame()
{
	while (true)
	{
		if (m_pMultiFrameReader == NULL)
			continue;
		HRESULT hr = 0;
		// 获取新的一个多源数据帧
		hr = m_pMultiFrameReader->AcquireLatestFrame(&m_pMultiFrame);	// 如果m_pMultiFrame不为空，这句会将其置空
		if (m_pMultiFrame == NULL)
		{
			//mtx1.unlock();
			SafeRelease(m_pMultiFrame);
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
		UINT nColorBufferSize = m_iColorWidth * m_iColorHeight * 4;
		if (SUCCEEDED(hr))
			hr = m_pColorFrame->CopyConvertedFrameDataToArray(nColorBufferSize, bgraData, ColorImageFormat::ColorImageFormat_Bgra);

		// 获得depth的数据
		UINT nDepthBufferSize = m_iDepthWidth * m_iDepthHeight;
		if (SUCCEEDED(hr))
		{
			hr = m_pDepthFrame->CopyFrameDataToArray(nDepthBufferSize, reinterpret_cast<UINT16*>(depthData));
		}

		// 释放资源
		SafeRelease(m_pColorFrame);
		SafeRelease(m_pDepthFrame);
		SafeRelease(m_pColorFrameReference);
		SafeRelease(m_pDepthFrameReference);
		SafeRelease(m_pMultiFrame);
	}
	
}

Mat MyKinect::getColorMat()
{
	m_matRgb = Mat(m_iColorHeight, m_iColorWidth, CV_8UC4, bgraData);
	return m_matRgb;
}

Mat MyKinect::getDepthMat()
{
	m_matDepth = Mat(m_iDepthHeight, m_iDepthWidth, CV_16UC1, depthData);
	return m_matDepth;
}