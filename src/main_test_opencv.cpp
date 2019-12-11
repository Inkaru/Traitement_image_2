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


// Squares detection
    vector<string> names;
    names.emplace_back("../00000.png");
    vector<vector<Point>> squares;
    vector<Rect> rectangles;

    for( int i = 0; i < names.size(); i++ )
    {
        Mat image = imread(names[i], 1);
        if( image.empty() )
        {
            cout << "Couldn't load " << names[i] << endl;
            continue;
        }
        
        Mat test = bina(image);

        findSquares(image, squares);
        cout << squares.size() << endl;
        pruneSquares(squares, rectangles);
        drawSquares(image, rectangles);
        imwrite( "out.png", image );
        generateImages(names[i], rectangles);
        int c = waitKey();
        if( (char)c == 27 )
            break;
    }
	
	return EXIT_SUCCESS;
}
