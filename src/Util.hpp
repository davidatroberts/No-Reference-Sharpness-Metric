#ifndef UTIL_H
#define UTIL_H

#include <opencv2/opencv.hpp>

namespace Util
{
	int local_extrema(cv::Mat &row, int x_pos);	
}

#endif