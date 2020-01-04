#ifndef SIFT_H_
#define SIFT_H_

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/xfeatures2d.hpp>
#include "opencv2/core/utils/filesystem.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include <opencv2/calib3d.hpp>

using namespace std;
using namespace cv;
using namespace cv::utils::fs;

void prepIcon(Mat& icon); //vire le texte en bas
void initIcons(vector<vector<KeyPoint>>& keypoints, vector<Mat>& descriptors, vector<string>& labels);
string identifyIcon(Mat& img);
string matchIcon(Mat& templ);
string matchSize(Mat& templ);

#endif /* SIFT_H_ */