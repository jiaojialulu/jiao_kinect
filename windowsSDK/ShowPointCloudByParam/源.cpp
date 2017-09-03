#include "MyKinect.h"

int main()
{
	MyKinect *jiaoKinect = new MyKinect();

	while (true)
	{
		imshow("Color", jiaoKinect->getColorMat());
		waitKey(1);
		imshow("Depth", jiaoKinect->getDepthMat());
		waitKey(1);
	}
	system("pause");
	return -1;
}