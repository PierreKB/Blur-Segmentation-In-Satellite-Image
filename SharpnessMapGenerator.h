#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include <vector>
#include <iostream>


class SharpnessMapGenerator
{
public:
	SharpnessMapGenerator();
	~SharpnessMapGenerator();

	void GetSharpnessMap(cv::Mat& input, cv::Mat& output, float threshold, int size, int k);
	void Segmentation(cv::Mat& input, cv::Mat& output);


private:
	void GetLBPMap(cv::Mat& input, cv::Mat& output, float threshold);
	uchar GetUpperLTP(const std::vector<int>& pixelsValues, float threshold);
	uchar GetLowerLTP(const std::vector<int>& pixelsValues, float threshold);
	bool IsUniformPattern(uchar value);
	bool IsUniformPatternType_6_7_8(uchar value);
};