#ifndef SIFT_H_
#define SIFT_H_

#include <iostream>
using namespace std;
using namespace cv;

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/xfeatures2d.hpp>

vector<std::vector<cv::KeyPoint>> keypoints;
vector<Mat> descriptors;
vector<String> names;

void initIcons(vector<String> names);

String identifyIcon(const Mat& img);


#endif /* SIFT_H_ */