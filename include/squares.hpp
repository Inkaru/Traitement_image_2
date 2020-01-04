#ifndef PROJET_OPENCV_CMAKE_SQUARES_HPP
#define PROJET_OPENCV_CMAKE_SQUARES_HPP

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/utils/filesystem.hpp"

#include <iostream>
#include <math.h>
#include <string.h>
#include <algorithm>
#include <fstream>

#include "image_treatment.hpp"
#include "sift_comparator.hpp"

using namespace cv;
using namespace std;
using namespace cv::utils::fs;

double angle(Point pt1, Point pt2, Point pt0);
void findSquares(const Mat& image, vector<vector<Point>>& squares,  int mode = 0);
void drawSquares(Mat& image, const vector<Rect>& rectangles);
void pruneSquares(vector<vector<Point>> &squares, vector<Rect>& rectangles, int dist = 2);
void cropRectangles(const Mat& image, vector<Rect>& rectangles, const string& scripter, const string& page);
void getIcons(const Mat& image, const vector<Rect>& rectangles, vector<Rect>& icons);
void getIcon(const Mat& image, const vector<Rect>& rectangles, Mat& icon);
void uprightImage(const Mat& image, Mat& uprImage);

#endif //PROJET_OPENCV_CMAKE_SQUARES_HPP
