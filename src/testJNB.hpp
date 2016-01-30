#ifndef TEST_SPHERE_H
#define TEST_SPHERE_H

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Util.hpp"

class testJNB: public::testing::Test 
{
protected:
	static void SetupTestCase() {

	}
};

TEST_F(testJNB, local_extrema) {
	int vals[] = {8, 1, 2, 3, 4, 5, 6, 7, 8, 9, 5};
	cv::Mat row(1, 11, CV_8UC1, vals);

	int expected_width = 8;
	int width = Util::local_extrema(row, 3);
	ASSERT_EQ(expected_width, width);
}

TEST_F(testJNB, local_extrema2) {
	int vals[] = {7, 2, 3, 4, 5, 1, 7, 8, 9, 10, 1};
	cv::Mat row(1, 11, CV_8UC1, vals);

	int expected_width = 3;
	int width = Util::local_extrema(row, 2);
	ASSERT_EQ(expected_width, width);

	expected_width = 4;
	width = Util::local_extrema(row, 5);
	ASSERT_EQ(expected_width, width);

	expected_width = 1;
	width = Util::local_extrema(row, 10);
	ASSERT_EQ(expected_width, width);
}

#endif