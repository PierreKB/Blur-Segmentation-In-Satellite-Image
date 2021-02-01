#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

#include "SharpnessMapGenerator.h"

void mouse_callback(int  event, int  x, int  y, int  flag, void* param)
{
	if (event == cv::EVENT_MOUSEMOVE) 
	{
		cv::Mat& xyz = *((cv::Mat*)param);
		cv::Vec3b val = xyz.at<cv::Vec3b>(y, x);

		std::cout << "x= " << x << " y= " << y << "val= " << val << std::endl;

	}
}

int main()
{
	SharpnessMapGenerator sharpnessMapGenerator;

	cv::Mat image = cv::imread("./dataset/real/4.png", cv::IMREAD_COLOR);
	cv::Mat output1;
	cv::Mat output2;

	sharpnessMapGenerator.GetSharpnessMap(image, output1, 6, 17, 0);
	sharpnessMapGenerator.Segmentation(output1, output2);


	cv::Size size(image.size().width / 2, image.size().width / 2);

	cv::namedWindow("Original", cv::WINDOW_NORMAL);
	cv::resizeWindow("Original", size);
	cv::namedWindow("Sharpness map", cv::WINDOW_NORMAL);
	cv::resizeWindow("Sharpness map", size);
	cv::namedWindow("Segmented", cv::WINDOW_NORMAL);
	cv::resizeWindow("Segmented", size);
	cv::setMouseCallback("Original", mouse_callback, &image);

	
	cv::imshow("Original", image);
	cv::imshow("Sharpness map", output1);
	cv::imshow("Segmented", output2);
	
	cv::waitKey(0);
	return 0;
}

