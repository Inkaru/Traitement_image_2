//
// Created by picabare on 04/12/19.
//

#include "squares.hpp"

// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
double angle(Point pt1, Point pt2, Point pt0) {
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}

// returns sequence of squares detected on the image.
// the sequence is stored in the specified memory storage
void findSquares(const Mat &image, vector<vector<Point> > &squares, int thresh, int N) {
    squares.clear();
    vector<vector<Point> > contours;

    Mat imgBin = binarize(image);

    // find contours and store them all as a list
    findContours(imgBin, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
//    drawContours(imgBin, contours, -1, cv::Scalar(0, 255, 0),30, 8);
//    cv::namedWindow("contours", WINDOW_NORMAL);
//    imshow("contours",imgBin);
    vector<Point> approx;

    // test each contour
    for (size_t i = 0; i < contours.size(); i++) {
        // approximate contour with accuracy proportional
        // to the contour perimeter
        approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 0.02, true);

        // square contours should have 4 vertices after approximation
        // relatively large area (to filter out noisy contours)
        // and be convex.
        // Note: absolute value of an area is used because
        // area may be positive or negative - in accordance with the
        // contour orientation
        if (approx.size() == 4 &&
            fabs(contourArea(Mat(approx))) > 1000 &&
            isContourConvex(Mat(approx))) {
            double maxCosine = 0;

            for (int j = 2; j < 5; j++) {
                // find the maximum cosine of the angle between joint edges
                double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
                maxCosine = MAX(maxCosine, cosine);
            }

            // if cosines of all angles are small
            // (all angles are ~90 degree) then write quandrange
            // vertices to resultant sequence
            if (maxCosine < 0.3)
                squares.push_back(approx);
        }
    }

}

void pruneSquares(vector<vector<Point> > &rectangles, vector<Rect> &squares, int dist) {
    vector<Rect> squares_tmp;
    for (auto const &rec: rectangles) {
        // Remove rectangles
        int isItSq = abs((rec[2].y - rec[0].y) - (rec[2].x - rec[0].x));
        if (isItSq < 20) {
            squares_tmp.emplace_back(Rect(rec[0], rec[2]));
        }
    }

    squares = squares_tmp;
    cout << "sq found : " << rectangles.size() << endl;
    squares_tmp.clear();

    // Remove duplicate squares
    for (auto const &sq1: squares) {
        bool found = false;
        for (auto const &sq2: squares_tmp) {
//            if(rect2.contains(rect1.tl()) || rect2.contains(rect1.br())){
            Point center = (sq1.br() + sq1.tl()) * 0.5;
            if (center.inside(sq2)) {
                found = true;
                break;
            }
        }
        if (!found) {
            squares_tmp.push_back(sq1);
        }
    }

    squares = squares_tmp;
    cout << "sq found after : " << squares.size() << endl;

}

// the function draws all the squares in the image
void drawSquares(Mat &image, const vector<Rect> &rectangles) {

    for (auto const &rect: rectangles) {
        cv::rectangle(image, rect, cv::Scalar(0, 255, 0), 3, LINE_AA);
    }

    cv::namedWindow("Square detection", WINDOW_NORMAL);
    imshow("Square detection", image);
}

void cropRectangles(const Mat& image, const vector<Rect> &rectangles) {
    int counter = 0;

    for (auto const &rect: rectangles) {
        cv::Mat crop = image(rect);
//                imshow("crop" + to_string(counter), crop);
        bool result = false;
        try {
            result = imwrite("../generated_images/" + to_string(counter) + ".png", crop);
            counter++;
        }
        catch (const cv::Exception &ex) {
            fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
        }
        if (result)
            printf("Saved PNG file.\n");
        else
            printf("ERROR: Can't save PNG file.\n");
    }
}

void getIcons(Mat& image, const vector<Rect> &rectangles, vector<Rect> &icons) {
    vector<int> lines;
    vector<int> columns;
    for (auto const &rect: rectangles) {
        bool linefound = false;
        bool columnfound = false;
        Point center = rect.tl();
        for (auto const &line: lines) {
            if (abs(line - center.y) < 25) {
                linefound = true;
                std::replace(lines.begin(), lines.end(), line, (line + center.y) / 2);
                break;
            }
        }
        if (!linefound) {
            lines.emplace_back(center.y);
        }

        for (auto const &col: columns) {
            if (abs(col - center.x) < 25) {
                columnfound = true;
                std::replace(columns.begin(), columns.end(), col, (col + center.x) / 2);
                break;
            }
        }
        if (!columnfound) {
            columns.emplace_back(center.x);
        }
    }

    int cols = image.cols;
    int rows = image.rows;
    int lineThickness = 2;

    // debug : affichage des lignes et colonnes
//    cout << "Centres lignes : " << endl;
//    for (auto const &line: lines) {
//        cout << line << endl;
//        cv::line(image, Point(0,line), Point(cols,line), (0,255,0), lineThickness);
//    }
//
//    cout << "Centres columns : " << endl;
//    for (auto const &col: columns) {
//        cout << col << endl;
//        cv::line(image, Point(col,0), Point(col,rows), (0,255,0), lineThickness);
//    }

    // todo : tester si l'alignement marche sur différentes images
    // calcul de la posistion des icones
    sort(columns.begin(), columns.end());
    int icon_col = int(1.95 * columns[0] - columns[1]);
//    cv::line(image, Point(icon_col,0), Point(icon_col,rows), (0,255,0), lineThickness);

    auto size = rectangles[0].size();
    for (auto const &l : lines){
        icons.emplace_back(Rect(Point(icon_col,l), size));
    }

    // debug : affichage des boites autour des icones
//    drawSquares(image, icons);

//    cv::namedWindow("lines detection", WINDOW_NORMAL);
//    imshow("lines detection", image);

}

void uprightImage(const Mat &image, Mat &dst) {

    // find rectangles in the image
    vector<vector<Point>> rectangles;
    findSquares(image, rectangles);
    // convert to rotatedRects
    vector<RotatedRect> squares;
    for (auto const &rect: rectangles) {
        squares.emplace_back(minAreaRect(rect));
    }

    double angle = 90.0;
    for (auto const &rect: squares) {
        angle += rect.angle / squares.size();
    }
    cout << "Average angle : " << angle << endl;

    // get rotation matrix for rotating the image around its center in pixel coordinates
    cv::Point2f center((image.cols - 1) / 2.0, (image.rows - 1) / 2.0);
    cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);
    // determine bounding rectangle, center not relevant
    cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), image.size(), angle).boundingRect2f();
    // adjust transformation matrix
    rot.at<double>(0, 2) += bbox.width / 2.0 - image.cols / 2.0;
    rot.at<double>(1, 2) += bbox.height / 2.0 - image.rows / 2.0;

    cv::warpAffine(image, dst, rot, bbox.size());
    cv::namedWindow("rotated_im.png", WINDOW_NORMAL);
    cv::imshow("rotated_im.png", dst);
}


