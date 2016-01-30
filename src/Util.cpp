#include "Util.hpp"

int Util::local_extrema(cv::Mat &row, int x_pos) {
	int x_value = row.at<unsigned char>(x_pos);

	// go right of the edge pixel, inc
	int last_x_value = x_value;
	int right_inc_width = 0;
	for (int x=x_pos; x<row.cols; ++x) {
		if (x != x_pos) {
			int current_value = row.at<int>(x);
			if (current_value > last_x_value) {
				right_inc_width++;
				last_x_value = current_value;
			}
			else {
				break;
			}
		}
	}

	// go right of the edge pixel, dec
	last_x_value = x_value;
	int right_dec_width = 0;
	for (int x=x_pos; x<row.cols; ++x) {
		if (x != x_pos) {
			int current_value = row.at<int>(x);
			if (current_value < last_x_value) {
				right_dec_width++;
				last_x_value = current_value;
			}
			else {
				break;
			}
		}
	}

	// go left of the edge pixel, inc
	last_x_value = x_value;
	int left_inc_width = 0;
	for (int x=x_pos; x >= 0; --x) {
		if (x!=x_pos) {
			int current_value = row.at<int>(x);
			if (current_value > last_x_value) {
				left_inc_width++;
				last_x_value = current_value;
			}
			else break;
		}
	}

	last_x_value = x_value;
	int left_dec_width = 0;
	for (int x=x_pos; x>=0; --x) {
		if (x!=x_pos) {
			int current_value = row.at<int>(x);
			if (current_value < last_x_value) {
				left_dec_width++;
				last_x_value = current_value;
			}
			else {
				break;
			}
		}
	}

	int right_width = 0;
	int left_width = 0;

	if (right_inc_width > right_dec_width) {
		right_width = right_inc_width;
		left_width = left_dec_width;
	}
	else if (right_inc_width < left_inc_width) {
		right_width = right_dec_width;
		left_width = left_inc_width;
	}
	else {
		// right_inc_width == right_decwidth
		right_width = right_inc_width;
		left_width = (left_inc_width > left_dec_width)? left_inc_width: left_dec_width; 
	} 

	return left_width+right_width;
}