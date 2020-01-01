#include "squares.hpp"

/**
 * @brief Finds a cosine of angle between two vectors from pt0->pt1 and from pt0->pt2
 * @param pt1 The second point of the first vector
 * @param pt2 The second point of the second vector
 * @param pt0 The first point of the two vectors
 * @return The cosine of angle between the two vectors formed by the three points
 */
double angle(Point pt1, Point pt2, Point pt0) {
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}

/**
 * @brief Detect squares in image
 * @param image The given image
 * @param squares The vector of squares found
 * @param thresh Unused
 * @param N Also unused
 */
void findSquares(const Mat &image, vector<vector<Point> > &squares, int thresh, int N) {
    squares.clear();
    vector<vector<Point>> contours;
    Mat imgBin = binarize(image);

    // Find contours and store them all as a list
    findContours(imgBin, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
    vector<Point> approx;
    /*
    drawContours(imgBin, contours, -1, Scalar(0, 255, 0),30, 8);
    namedWindow("contours", WINDOW_NORMAL);
    imshow("contours",imgBin);
    */

    // Test each contour
    for (size_t i = 0; i < contours.size(); i++) {
        // Approximate contour with accuracy proportional to the contour perimeter
        approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 0.02, true);

        // Square contours should have 4 vertices after approximation relatively large area (to filter out noisy contours) and be convex.
        // Note: absolute value of an area is used because area may be positive or negative - in accordance with the contour orientation
        if (approx.size() == 4 &&
            fabs(contourArea(Mat(approx))) > 1000 &&
            isContourConvex(Mat(approx))) {
            double maxCosine = 0;

            for (int j = 2; j < 5; j++) {
                // Find the maximum cosine of the angle between joint edges
                double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
                maxCosine = MAX(maxCosine, cosine);
            }

            // If cosines of all angles are small (all angles are ~90 degree) then write quandrange vertices to resultant sequence
            if (maxCosine < 0.3)
                squares.push_back(approx);
        }
    }
}

/**
 * @brief Remove rectangles detected as squares and duplicated squares
 * @param rectangles The pruned list of squares
 * @param squares The list of squares to be pruned
 * @param dist Unused
 */
void pruneSquares(vector<vector<Point>> &rectangles, vector<Rect> &squares, int dist) {
    vector<Rect> squares_tmp;
    for (auto const &rec: rectangles) {
        // Remove rectangles
        int isItSq = abs((rec[2].y - rec[0].y) - (rec[2].x - rec[0].x));
        if (isItSq < 20) {
            squares_tmp.emplace_back(Rect(rec[0], rec[2]));
        }
    }

    // average height used for prunning small squares
    unsigned long height = 0.0;
    for (auto const &rec: squares_tmp) {
        height += rec.size().height;
        cout << rec.size() << endl;
    }
    height /= squares_tmp.size();
    cout << height << endl;

    squares = squares_tmp;
    cout << "Number of square found so far : " << rectangles.size() << endl;
    squares_tmp.clear();

    // Remove duplicate squares
    for (auto const &sq1: squares) {
        bool found = false;
        for (auto const &sq2: squares_tmp) {
            //Square removed if its center is contained in another accepted square
            Point center = (sq1.br() + sq1.tl()) * 0.5;
            if (center.inside(sq2)) {
                found = true;
                break;
            }
        }
        if (!found && fabs(sq1.size().height - height) < 0.1 * height) {
            squares_tmp.push_back(sq1);
        }
    }

    squares = squares_tmp;
    cout << "Number of square found after pruning : " << squares.size() << endl;
}

/**
 * @brief Draws all the squares in the image
 * @param image The given image
 * @param rectangles The list of squares to be drawn
 */
void drawSquares(Mat &image, const vector<Rect> &rectangles) {
    for (auto const &rect: rectangles) {
        rectangle(image, rect, Scalar(0, 255, 0), 3, LINE_AA);
    }

    namedWindow("Square detection", WINDOW_NORMAL);
    imshow("Square detection", image);
}

/**
 * @brief Generate icon images from detected squares
 * @param image The image from which icon images will be extracted
 * @param rectangles The list of squares
 */
void cropRectangles(const Mat& image, const vector<Rect> &rectangles, const string& filename) {
    int counter = 0;
    int modulo;

    cout << "Start crop operation : " << endl;
    for (auto const &rect: rectangles) {
        Mat crop = image(rect);
        //imshow("Cropped image n°" + to_string(counter), crop);
        bool result = false;
        try {
            modulo = counter % 5;
            result = imwrite(filename + "_" + to_string(counter/5) + "_" + to_string(modulo) + ".png", crop);
            counter++;
        }
        catch (const Exception &ex) {
            fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
        }
        if (result){
            cout << "Saved PNG file n°" << to_string(counter-1) << endl;
        } else {
            cout << "ERROR: Can't save PNG file. (Image n°" << to_string(counter-1) << ")" << endl;
        }
    }
}

/**
 * @brief Extract reference icons of a given list of squares and an image
 * @param image The given image
 * @param rectangles The list of squares
 * @param icons The list of squares corresponding to the reference icons
 */
void getIcons(Mat& image, const vector<Rect> &rectangles, vector<Rect> &icons) {
    cout << "Retrieving reference icons : " << endl;
    vector<int> lines;
    vector<int> columns;
    for (auto const &rect: rectangles) {
        bool linefound = false;
        bool columnfound = false;
        Point center = rect.tl(); //Top-left corner
        for (auto const &line: lines) { // Line already registered -> correction by averaging
            if (abs(line - center.y) < 25) {
                linefound = true;
                //std::replace(lines.begin(), lines.end(), line, (line + center.y) / 2);
                break;
            }
        }
        if (!linefound) { // New line -> add to the list
            lines.emplace_back(center.y);
        }

        for (auto const &col: columns) {
            if (abs(col - center.x) < 25) { // Column already registered -> correction by averaging
                columnfound = true;
                std::replace(columns.begin(), columns.end(), col, (col + center.x) / 2);
                break;
            }
        }
        if (!columnfound) { // New column -> add to the list
            columns.emplace_back(center.x);
        }
    }

    /*
    // Debug : affichage des lignes et colonnes
    int cols = image.cols;
    int rows = image.rows;
    int lineThickness = 2;

    cout << "Centres lignes : " << lines.size() << endl;
    for (auto const &line: lines) {
        cout << line << " ";
        line(image, Point(0,line), Point(cols,line), (0,0,255), lineThickness);
    }
    cout << endl;

    cout << "Centres columns : " << columns.size() << endl;
    for (auto const &col: columns) {
        cout << col << " ";
        line(image, Point(col,0), Point(col,rows), (0,255,255), lineThickness);
    }
    cout << endl;
    */

    // todo : tester si l'alignement marche sur différentes images
    // Calcul de la position des icones
    sort(columns.begin(), columns.end());
    int icon_col = int(0.9 * (columns[0] - fabs(columns[0] - columns[1])));

    auto size = rectangles[0].size();
    for (auto const &l : lines){
        icons.emplace_back(Rect(Point(icon_col,l), size));
    }

    cout << "Total icons : " << icons.size() << endl;
    int counter = 0;
    for (auto const &rect: icons){
        bool result;
        Mat crop = image(rect);
        try {
            result = imwrite("../generated_images/r" + to_string(counter) + ".png", crop);
            counter++;
        }
        catch (const Exception &ex) {
            fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
        }
        if (result){
            cout << "Saved PNG file n°" << to_string(counter-1) << endl;
        } else {
            cout << "ERROR: Can't save PNG file. (Image n°" << to_string(counter-1) << ")" << endl;
        }
    }

    /*
    // Debug : affichage des boites autour des icones
    line(image, Point(icon_col,0), Point(icon_col,rows), (255, 0, 0), 5);
    drawSquares(image, icons);
    namedWindow("Lines detection", WINDOW_NORMAL);
    imshow("Lines detection", image);
    */
}

/**
 * @brief Set an image upright relatively to its inner squares
 * @param image The crooked image
 * @param uprImage The upright image
 */
void uprightImage(const Mat &image, Mat &uprImage) {
    // Find rectangles in the image
    vector<vector<Point>> rectangles;
    findSquares(image, rectangles);
    // Convert to rotatedRects
    vector<RotatedRect> squares;
    for (auto const &rect: rectangles) {
        squares.emplace_back(minAreaRect(rect));
    }

    /*/////////////////*/
//    for (auto const &rect: squares) {
//        Point2f vertices[4];
//        rect.points(vertices);
//        for(int i = 0; i<4; i++) {
//            line(image, vertices[i], vertices[(i+1)%4], Scalar(0, 255, 0), 3, LINE_AA);
//        }
//    }
//    namedWindow("Rotated square detection", WINDOW_NORMAL);
//    imshow("Rotated square detection", image);

    // NEW : needs testing if the first icon contains a square
    double angle = 90.0;
    double isUpright = true;
    double ang = squares[0].angle;
    if(fabs(ang) > 1 && fabs(fabs(ang) - 90) > 1){
        isUpright = false;
        angle += ang;
    }
// OLD - does not work
//    double angle = 90.0;
//    double isUpright = true;
//    for (auto const &rect: squares) {
//        angle += rect.angle / squares.size();
//        if(fabs(rect.angle) > 1 && fabs(fabs(rect.angle) - 90) > 1) {
//            isUpright = false;
//        }
//    }
    cout << "Average angle : " << angle << ", " << (isUpright ? "image is already upright, no further treatment" : "image is crooked, it will be set upright") << endl;

    if(!isUpright) {
        // Get rotation matrix for rotating the image around its center in pixel coordinates
        Point2f center((image.cols - 1) / 2.0, (image.rows - 1) / 2.0);
        Mat rot = getRotationMatrix2D(center, angle, 1.0);
        // Determine bounding rectangle, center is not relevant
        Rect2f bbox = RotatedRect(Point2f(), image.size(), angle).boundingRect2f();
        // Adjust transformation matrix
        rot.at<double>(0, 2) += bbox.width / 2.0 - image.cols / 2.0;
        rot.at<double>(1, 2) += bbox.height / 2.0 - image.rows / 2.0;

        // Apply transformations to obtain upright image
        warpAffine(image, uprImage, rot, bbox.size());
        namedWindow("Upright image", WINDOW_NORMAL);
        imshow("Upright image", uprImage);
    } else {
        uprImage = image;
    }
}


