//////////////////////////////////////////////////////////////////////////
// Option Images
// Projet, séance 1
// thème : premier pas en OpenCV
// contenu : charge, affiche, réduction, calcul et affichage d'histogramme
// version : 17.1128
//////////////////////////////////////////////////////////////////////////


#include <iostream>
#include "squares.hpp"
#include "image_treatment.hpp"
using namespace std;

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
using namespace cv;

#include "histogram.hpp"


int main (void) {

//    test();
// Squares detection
    vector<string> names;
    names.emplace_back("../00000rotated.png");
    vector<vector<Point>> squares;
    vector<RotatedRect> rectangles;

    for( int i = 0; i < names.size(); i++ )
    {
        Mat image = imread(names[i], 1);
        if( image.empty() )
        {
            cout << "Couldn't load " << names[i] << endl;
            continue;
        }

        findSquares(image, squares);
        cout << squares.size() << endl;
        pruneSquares(squares, rectangles);
        drawSquares(image, rectangles);
        cropRectangles(image, rectangles);
        imwrite( "out.png", image );
//        getIcons(names[0], rectangles);
        int c = waitKey();
        if( (char)c == 27 )
            break;
    }
	
	return EXIT_SUCCESS;
}
