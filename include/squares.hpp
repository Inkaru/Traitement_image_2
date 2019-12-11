//
// Created by picabare on 04/12/19.
//

#ifndef PROJET_OPENCV_CMAKE_SQUARES_HPP
#define PROJET_OPENCV_CMAKE_SQUARES_HPP

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <math.h>
#include <string.h>

#include "image_treatment.hpp"

using namespace cv;
using namespace std;

double angle( Point pt1, Point pt2, Point pt0 );
void findSquares( const Mat& image, vector<vector<Point> >& squares,  int thresh = 50, int N = 5 );
void drawSquares( Mat& image, const vector<Rect>& rectangles);
void pruneSquares(vector<vector<Point> >& squares, vector<Rect>& rectangles, int dist = 2);
void cropRectangles(string filename, const vector<Rect>& rectangles);

#endif //PROJET_OPENCV_CMAKE_SQUARES_HPP
