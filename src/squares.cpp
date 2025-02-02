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
 * @param mode 0 to binarize, 1 to binarize AND remove drawings,
 */
void findSquares(const Mat &image, vector<vector<Point> > &squares, int mode) {
    squares.clear();
    vector<vector<Point>> contours;
    Mat imgBin;
    if(mode == 0){
        imgBin = binarize(image);
    } else {
        imgBin = removeDrawings(image);
    }

    // Find contours and store them all as a list
    findContours(imgBin, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
    vector<Point> approx;

//    drawContours(image, contours, -1, Scalar(0, 255, 0),5, 1);
//    namedWindow("contours", WINDOW_NORMAL);
//    imshow("contours",image);
//    waitKey();

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
 * @brief Remove rectangles detected as squares, duplicated squares and small squares
 * @param rectangles The pruned list of squares
 * @param squares The list of squares to be pruned
 * @param dist Unused
 */
void pruneSquares(vector<vector<Point>> &rectangles, vector<Rect> &squares, int dist) {
    cout << "Number of square found so far : " << rectangles.size() << endl;
    vector<Rect> squares_tmp;
    for (auto const &rec: rectangles) {
        // Remove rectangles
        int isItSq = abs((rec[2].y - rec[0].y) - (rec[2].x - rec[0].x));
        if (isItSq < 20) {
            squares_tmp.emplace_back(Rect(rec[0], rec[2]));
        }
    }

    // Average height used for prunning small squares
    unsigned long height = 0.0;
    for (auto const &rec: squares_tmp) {
        if(rec.size().height > height ){
            height = rec.size().height;
        }
    }

    squares = squares_tmp;
    squares_tmp.clear();

    // Remove duplicate squares
    for (auto const &sq1: squares) {
        bool found = false;
        for (auto const &sq2: squares_tmp) {
            //Square not selected if its center is contained in another accepted square
            Point center = (sq1.br() + sq1.tl()) * 0.5;
            if (center.inside(sq2)) {
                found = true;
                break;
            }
        }

        if (!found && fabs(height - sq1.size().height) < 0.30 * height) { //Square not selected if its height is smaller than 15% of average heigth
            squares_tmp.push_back(sq1);
        }
    }

    squares = squares_tmp;
    squares_tmp.clear();
    cout << "Number of square found after pruning duplicates and small ones : " << squares.size() << endl;


    for (auto const &sq: squares) {
        Point pt(0.025*sq.width, 0.025*sq.height);
        squares_tmp.emplace_back(Rect(sq.tl() + pt,sq.br() - pt));
    }

    squares = squares_tmp;
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
    waitKey();
}

/**
 * @brief Generate icon images and txt from detected squares
 * @param image The image from which icon images will be extracted
 * @param rectangles The list of squares
 * @param filename String representing the name
 */
void cropRectangles(const Mat& image, vector<Rect> &rectangles, const string& scripter, const string& page) {
    // Sort rectangles by their y coordinates
    sort(rectangles.begin(), rectangles.end(), [](Rect a, Rect b) {
        return a.y < b.y;
    });

    // Split the vector in lines
    vector<vector<Rect>> splitted;
    splitted.emplace_back();
    double line = rectangles[0].y;
    int count = 0;
    for(auto const &rect: rectangles){
        if(fabs(line - rect.y) < 50 ){ // Fill splitted with current line
            splitted[count].emplace_back(rect);
        } else { // Current line full, switch to the next line
            line = rect.y;
            splitted.emplace_back();
            count++;
            splitted[count].emplace_back(rect);
        }
    }

    cout << "Start crop operation : " << endl;
    Mat icon;
    string cropname;
    String label = "";
    String size = "";
    int colnb = 0;
    int rownb = 0;

    for (auto &rectline: splitted) {
        try {
            // Sort rectangles by their x coordinates (for each line)
            sort(rectline.begin(), rectline.end(), [](Rect a, Rect b) {
                return a.x < b.x;
            });

            // Find the reference icon of the line and get label and size
            getIcon(image,rectline,icon);

//        namedWindow("test", WINDOW_NORMAL);
//        imshow("test", binarize(icon));
//        waitKey();

            label = matchIcon(icon);
            size = matchSize(icon);
            colnb = 0;
            for (auto const &rect: rectline) {
                Mat crop = image(rect);
                bool result = false;
                cropname = "../generated_images/" + label + "_" + scripter + "_" + page + "_" + to_string(rownb) + "_" + to_string(colnb);
                try {
                    // label_scripter_page_column_row.png
                    result = imwrite(cropname + ".png", crop);
                    ofstream txticon(cropname + ".txt");
                    txticon << "label " + label + "\n";
                    txticon << "form " + scripter + page + "\n";
                    txticon << "scripter " + scripter + "\n";
                    txticon << "page " + page + "\n";
                    txticon << "row " + to_string(rownb) + "\n";
                    txticon << "column " + to_string(colnb) + "\n";
                    txticon << "size " + size + "\n";
                    txticon.close();
                }
                catch (const Exception &ex) {
                    fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
                }
                if (result){
                    cout << "Saved PNG file n°" << cropname << endl;
                } else {
                    cout << "ERROR: Can't save PNG file. (Image n°" << cropname << ")" << endl;
                }
                colnb++;
            }
            rownb++;
        } catch(Exception e){
            cout << "Extraction failed " << cropname << endl;
            rownb++;
            continue;
        }
    }
}

/**
* @brief Extract reference icon of a given list of squares and an image
* @param image The given image of the page
* @param rectangles The list of squares
* @param icon The reference icon
*/
void getIcon(const Mat& image, const vector<Rect>& rectangles, Mat& icon) {
    cout << "Retrieving reference icon : " << endl;
    Rect iconrect;
    vector<int> lines;
    vector<int> columns;
    for (auto const &rect: rectangles) {
        bool linefound = false;
        bool columnfound = false;
        Point center = rect.tl(); //Top-left corner
        for (auto const &line: lines) { // Line already registered -> correction by averaging
            if (abs(line - center.y) < 25) {
                linefound = true;
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

    // Compute icon's position and convert to matrix
    sort(columns.begin(), columns.end());
    int icon_col = int(0.9 * (columns[0] - fabs(columns[0] - columns[1])));

    auto size = rectangles[0].size();
    iconrect = Rect(Point(icon_col,lines[0]), size);

    icon = image(iconrect);
}

/**
 * @brief Set an image upright relatively to its inner squares
 * @param image The crooked image
 * @param uprImage The upright image
 */
void uprightImage(const Mat &image, Mat &uprImage) {
    // Find rectangles in the image
    vector<vector<Point>> rectangles;
    findSquares(image, rectangles, 1);
    // Convert to rotatedRects
    vector<RotatedRect> squares;

    // Keeps only the rectangle for orientation detection
    for (auto const &rect: rectangles) {
        int sqRatio = abs((rect[2].x - rect[0].x) / (rect[2].y - rect[0].y));
        if (sqRatio > 3) {
            squares.emplace_back(minAreaRect(rect));
        }
    }

    /*/////////////////*/
//    for (auto const &rect: squares) {
//        Point2f vertices[4];
//        rect.points(vertices);
//        for(int i = 0; i<4; i++) {
//            line(image, vertices[i], vertices[(i+1)%4], Scalar(0, 255, 0), 5, LINE_AA);
//        }
//    }
//    namedWindow("Rotated square detection", WINDOW_NORMAL);
//    imshow("Rotated square detection", image);
//    waitKey();

    double angle = 90.0;
    double isUpright = true;
    if(!squares.empty()) {
        double ang = squares[0].angle;
        if(fabs(ang) > 1 && fabs(fabs(ang) - 90) > 1){
            isUpright = false;
//            cout << ang << endl ;
            angle += ang;
        }
    }

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
//        namedWindow("Upright image", WINDOW_NORMAL);
//        imshow("Upright image", uprImage);
//        waitKey();

    } else {
        uprImage = image;
    }
}


