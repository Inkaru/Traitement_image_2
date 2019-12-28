#ifndef SIFT_H_
#define SIFT_H_

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/xfeatures2d.hpp>

using namespace std;
using namespace cv;

void initIcons(vector<vector<KeyPoint>>& keypoints, vector<Mat>& descriptors, vector<String>& names);
String identifyIcon(const Mat& img, vector<Mat>& descriptors, vector<String>& names);


#endif /* SIFT_H_ */