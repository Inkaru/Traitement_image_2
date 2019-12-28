#ifndef PROJET_OPENCV_CMAKE_SQUARES_HPP
#define PROJET_OPENCV_CMAKE_SQUARES_HPP

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <math.h>
#include <string.h>
#include <algorithm>

#include "image_treatment.hpp"

using namespace cv;
using namespace std;

double angle(Point pt1, Point pt2, Point pt0);
void findSquares(const Mat& image, vector<vector<Point>>& squares,  int thresh = 50, int N = 5);
void drawSquares(Mat& image, const vector<Rect>& rectangles);
void pruneSquares(vector<vector<Point>> &squares, vector<Rect>& rectangles, int dist = 2);
void cropRectangles(const Mat& image, const vector<Rect>& rectangles);
void getIcons(Mat& image, const vector<Rect>& rectangles, vector<Rect>& icons);
void uprightImage(const Mat& image, Mat& uprImage);

#endif //PROJET_OPENCV_CMAKE_SQUARES_HPP
