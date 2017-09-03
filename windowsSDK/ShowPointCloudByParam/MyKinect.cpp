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
			// ��ȡ������Դ����ȡ��  
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
	cout << "kinect�򿪳ɹ���";
}

MyKinect::~MyKinect()
{
	std::cout << "�ر�kinect" << endl;
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
		std::cout << "�ر�kinect ʧ�ܣ�����" << endl;

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
		// ��ȡ�µ�һ����Դ����֡
		hr = m_pMultiFrameReader->AcquireLatestFrame(&m_pMultiFrame);	// ���m_pMultiFrame��Ϊ�գ����Ὣ���ÿ�
		if (m_pMultiFrame == NULL)
		{
			//mtx1.unlock();
			SafeRelease(m_pMultiFrame);
			continue;
		}
		// �Ӷ�Դ����֡�з������ɫ���ݣ�������ݺͺ�������
		if (SUCCEEDED(hr))
			hr = m_pMultiFrame->get_ColorFrameReference(&m_pColorFrameReference);
		if (SUCCEEDED(hr))
			hr = m_pColorFrameReference->AcquireFrame(&m_pColorFrame);
		if (SUCCEEDED(hr))
			hr = m_pMultiFrame->get_DepthFrameReference(&m_pDepthFrameReference);
		if (SUCCEEDED(hr))
			hr = m_pDepthFrameReference->AcquireFrame(&m_pDepthFrame);


		// color������ͼƬ��
		UINT nColorBufferSize = m_iColorWidth * m_iColorHeight * 4;
		if (SUCCEEDED(hr))
			hr = m_pColorFrame->CopyConvertedFrameDataToArray(nColorBufferSize, bgraData, ColorImageFormat::ColorImageFormat_Bgra);

		// ���depth������
		UINT nDepthBufferSize = m_iDepthWidth * m_iDepthHeight;
		if (SUCCEEDED(hr))
		{
			hr = m_pDepthFrame->CopyFrameDataToArray(nDepthBufferSize, reinterpret_cast<UINT16*>(depthData));
		}

		// �ͷ���Դ
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