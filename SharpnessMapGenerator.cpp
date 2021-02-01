#include "SharpnessMapGenerator.h"


SharpnessMapGenerator::SharpnessMapGenerator()
{}

SharpnessMapGenerator::~SharpnessMapGenerator()
{}

void SharpnessMapGenerator::GetSharpnessMap(cv::Mat& input, cv::Mat& output, float threshold, int size, int k)
{
	cv::Mat ltpMap;
	output = cv::Mat::zeros(input.size(), CV_8UC1);
	GetLBPMap(input, ltpMap, threshold);

	int start = (int)(size / 2);

	//std::cout << "Start: " << start << std::endl;
	//std::cout << "lbpMap size: " << lbpMap.size() << std::endl;

	for (int y = start; y < ltpMap.rows - start; ++y)
	{
		for (int x = start; x < ltpMap.cols - start; ++x)
		{
			//cv::Vec2b value = ltpMap.at<cv::Vec2b>(y, x);

			//std::cout << value << std::endl;
			
			cv::Mat patch = ltpMap(cv::Rect(x - start, y - start, size, size));
			float sharpness = 0.0f;

			for (int r = 0; r < patch.rows; ++r)
			{
				for (int c = 0; c < patch.cols; ++c)
				{
					cv::Vec2b value = patch.at<cv::Vec2b>(r, c);

					if (value[0] > 5 && value[1] > 5)
					{
						++sharpness;
					}
				}
			}

			sharpness /= (size * size);
			output.at<uchar>(y, x) = sharpness * 255.0f;
		}
	}
	
	//output = output(cv::Rect(start, start, input.size().width - size, input.size().height - size));
}

void SharpnessMapGenerator::Segmentation(cv::Mat& input, cv::Mat& output)
{
	output = cv::Mat::zeros(input.size(), CV_8UC1);

	float threshold = 0.0f;

	for (int y = 0; y < input.rows; ++y)
	{
		for (int x = 0; x < input.cols; ++x)
		{
			threshold += input.at<uchar>(y, x);
		}
	}

	threshold /= (input.rows * input.cols);

	std::cout << "Threshold: " << threshold << std::endl;

	for (int y = 0; y < input.rows; ++y)
	{
		for (int x = 0; x < input.cols; ++x)
		{
			output.at<uchar>(y, x) = input.at<uchar>(y, x) > threshold ?
				255 : 0;
		}
	}
}

void SharpnessMapGenerator::GetLBPMap(cv::Mat& input, cv::Mat& output, float threshold)
{
	cv::Mat gray;
	output = cv::Mat::zeros(input.size(), CV_8UC2);

	cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);

	std::vector<int> pixelsValues;

	for (int y = 1; y < input.rows - 1; ++y)
	{
		for (int x = 1; x < input.cols - 1; ++x)
		{
			pixelsValues.push_back(gray.at<uchar>(y, x));
			pixelsValues.push_back(gray.at<uchar>(y - 1, x));
			pixelsValues.push_back(gray.at<uchar>(y - 1, x + 1));
			pixelsValues.push_back(gray.at<uchar>(y, x + 1));
			pixelsValues.push_back(gray.at<uchar>(y + 1, x + 1));
			pixelsValues.push_back(gray.at<uchar>(y + 1, x));
			pixelsValues.push_back(gray.at<uchar>(y + 1, x - 1));
			pixelsValues.push_back(gray.at<uchar>(y, x - 1));
			pixelsValues.push_back(gray.at<uchar>(y - 1, x - 1));

			uchar upperltp =  GetUpperLTP(pixelsValues, threshold);
			uchar lowerltp = GetLowerLTP(pixelsValues, threshold);

			output.at<cv::Vec2b>(y, x) = cv::Vec2b(upperltp, lowerltp);

			pixelsValues.clear();
		}
	}
}

uchar SharpnessMapGenerator::GetUpperLTP(const std::vector<int>& pixelsValues, float threshold)
{
	int center = pixelsValues[0];
	uchar lbp = 0;

	for (int i = 0; i < 8; ++i)
	{
		int diff = pixelsValues[i + 1] - center;
		uchar s = diff > threshold ? 1 : 0;

		lbp += (s * (uchar)std::pow(2, i));
	}

	return lbp;
}

uchar SharpnessMapGenerator::GetLowerLTP(const std::vector<int>& pixelsValues, float threshold)
{
	int center = pixelsValues[0];
	uchar lbp = 0;

	for (int i = 0; i < 8; ++i)
	{
		int diff = pixelsValues[i + 1] - center;
		uchar s = diff >= threshold ? 0 : 1;

		lbp += (s * (uchar)std::pow(2, i));
	}

	return lbp;
}


bool SharpnessMapGenerator::IsUniformPattern(uchar value)
{
	if (value == 0 || value == 255) { return true; }

	int transition = 0;

	for (size_t i = 1; i < CHAR_BIT * sizeof value; ++i)
	{
		uchar current = (value & (1 << i));
		uchar previous = (value & (1 << (i - 1)));

		if (i == (CHAR_BIT * sizeof value) - 1)
		{
			current = (value & (1 << i));
			previous = value & 1;
		}

		if (current != previous) { ++transition; }
		if (transition > 2) { return false; }

	}

	return true;
}

bool SharpnessMapGenerator::IsUniformPatternType_6_7_8(uchar value)
{
	std::vector<int> positionsOfZeros;

	for (size_t i = 0; i < CHAR_BIT * sizeof value; ++i)
	{
		if ((value & (1 << i)) == 0) { positionsOfZeros.push_back(i); }

		if (positionsOfZeros.size() > 2) { return false; }
	}

	if (positionsOfZeros.size() == 2)
	{
		if ((positionsOfZeros[1] - positionsOfZeros[0]) == 1 || (positionsOfZeros[1] - positionsOfZeros[0]) == 7) { return true; }

		return false;
	}

	return true;

	
}