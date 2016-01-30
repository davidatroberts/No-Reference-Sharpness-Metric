#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <cmath>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Util.hpp"

const int CHUNK_SIZE = 64;
const float BETA = 3.6;
const float T = 0.002;

float local_contrast(cv::Mat chunk) {
	double min_value, max_value;
	cv::minMaxLoc(chunk, &min_value, &max_value);

	float contrast = max_value - min_value;
	return contrast;
}

int jnb_edge_width(float contrast) {
	if (contrast <= 50) {
		return 5;
	}
	else {
		return 3;
	}
}

float get_block_distortion(std::vector<int> &edges, int w_jnb, float beta) {
	float distortion = 0;
	for (std::vector<int>::size_type i=0; i < edges.size(); ++i) {
		int width = edges[i];
		float ratio = (((float)width)/((float)w_jnb));
		float edge_distortion = pow(ratio, beta);
		distortion+=edge_distortion;
	}

	distortion = pow(distortion, (((float)1.0f)/beta));
	return distortion;
}

float get_image_distortion(std::vector<float> &block_distortions, float beta) {
	float image_distortion = 0;
	for (std::vector<float>::size_type i=0; i<block_distortions.size(); ++i) {
		float dist = block_distortions[i];				// changed from float
		float single_dist = pow(fabs(dist), beta); 		// changed from abs	
		image_distortion+=single_dist;
	}

	image_distortion = pow(image_distortion, (((float)1.0)/beta));
	return image_distortion;
}

float get_sharpness_measure(float distortion, float processed_blocks) {
	return processed_blocks/distortion;
}

float get_blurriness_measure(float distortion, float processed_blocks) {
	return distortion/processed_blocks;
}

float probability_detecting_blur(float distortion, float beta) {
	return ((float)1.0f) - exp(pow(-distortion, beta));
}

int main(int argc, char **argv) {
	std::string image_str = std::string(argv[1]);
	cv::Mat src = cv::imread(image_str, 1);

	// convert to luminance
	cv::Mat lum, lum_8;
	cv::cvtColor(src, lum, CV_RGB2GRAY);
	cv::convertScaleAbs(lum, lum_8);

	// run sobel filter
	cv::Mat sobel, sobel_abs, img_threshold;
	cv::Sobel(lum_8, sobel, CV_16S, 1, 0, 3, 1, 0);
	convertScaleAbs(sobel, sobel_abs);

	cv::imwrite("raw_edge.bmp", sobel);
	cv::imwrite("edge_abs.bmp", sobel_abs);

	int processed_blocks = 0;
	float threshold = T * (CHUNK_SIZE*CHUNK_SIZE);

	cv::Size im_size = sobel.size();
	int x_chunks = ceil(im_size.width / CHUNK_SIZE);
	int y_chunks = ceil(im_size.height / CHUNK_SIZE);

	std::vector<float> block_distortions;

	// scan each 64*64 block 
	for (int x = 0; x < x_chunks; ++x) {
		int cx = x*CHUNK_SIZE;
		
		for (int y = 0; y < y_chunks; ++y) {
			int cy = y*CHUNK_SIZE;
			cv::Mat edge_chunk = sobel_abs(cv::Range(cy, cy+CHUNK_SIZE), cv::Range(cx, cx+CHUNK_SIZE));
			
			// count the number of edges in the chunk
			int edge_count = 0;
			for (int i = 0; i < edge_chunk.rows; ++i) {
				for (int j = 0; j < edge_chunk.cols; ++j) {
					if (edge_chunk.at<unsigned char>(i, j) > 0) {
						edge_count++;
					}
				}
			}

			// process further if chunk is edge chunk
			if (edge_count > threshold) {
				processed_blocks++;
				cv::Mat lum_chunk = lum_8(cv::Range(cy, cy+CHUNK_SIZE), cv::Range(cx, cx+CHUNK_SIZE));

				// estimage local contrast of edge and compute corresponding edge width
				float contrast = local_contrast(lum_chunk); 
				int jnb_width = jnb_edge_width(contrast);

				// for each edge compute corresponding edge width
				std::vector<int> edge_widths;
				for (int i = 0; i < edge_chunk.rows; ++i) {
					cv::Mat row = lum_chunk.row(i);
					for (int j = 0; j < edge_chunk.cols; ++j) {
						if (edge_chunk.at<unsigned char>(i, j) > 0) {
							int edge_width = Util::local_extrema(row, j);
							edge_widths.push_back(edge_width);
						}
					}		
				}

				// compute block distortion
				float block_dist = get_block_distortion(edge_widths, jnb_width, BETA);
				block_distortions.push_back(block_dist);

			}
		}
	}

	// obtain overall distortion by pooling over blocks
	float total_distortion = get_image_distortion(block_distortions, BETA);

	// return measures
	float sharp_distortion = get_sharpness_measure(total_distortion, processed_blocks);
	float blur_distortion = get_blurriness_measure(total_distortion, processed_blocks);

	// output the calculated values
	std::cout << "Sharpness measure: " << sharp_distortion << std::endl;
	std::cout << "Blurriness measure: " << blur_distortion << std::endl;

	return 0;
}