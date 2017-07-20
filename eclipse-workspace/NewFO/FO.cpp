#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/nonfree.hpp"

#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

int main(int argc, char* argv[]){
	string filename;
	cout << "Enter file name: " << endl;
	cin >> filename;
	//VideoCapture cap("/home/regardunix/Downloads/IMG_0676.MOV");
	VideoCapture cap(filename);
	if(!cap.isOpened()){
		cout << "Oops! I have not found the video"<< endl;
		return -1;
	}

	double fps = cap.get(CV_CAP_PROP_FPS);
	cout << "Frame per seconds : " << fps << endl;

	Mat gray;
	Mat edges;
	Mat closed;
	vector<vector<Point> > contours;

	vector<Vec4i> hierarchy;
	Mat frame;
	double peri;
	Scalar boundColor( 0, 255, 0 );
	Scalar Color( 0, 0, 255 );
	while(1){
		bool bSuccess = cap.read(frame);
		if(!bSuccess){
			cout << "Oops! Can't read the frame from video" << endl;
			break;
		}

		cvtColor(frame, gray, COLOR_BGR2GRAY);
		GaussianBlur(gray, gray, Size(3,3), 0, 0);

		Canny(gray, edges, 10, 250);

		Mat kernel = getStructuringElement(MORPH_RECT, Size(7, 7));
		morphologyEx(edges, closed, MORPH_CLOSE, kernel);

		findContours( closed, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));

		vector<vector<Point> > approx(contours.size());
		vector<Rect> boundRect( contours.size() );

		for(size_t c = 0; c < contours.size(); c++){
			peri = arcLength(contours[c], true);
			approxPolyDP(Mat(contours[c]), approx[c], 0.002*peri, true);
			if(approx[c].size() >= 2 && approx[c].size() <= 8 && peri >= 500 ){
				boundRect[c] = boundingRect( Mat(approx[c]) );
				drawContours(frame, approx, -1, Color, 4);
				rectangle( frame, boundRect[c].tl(), boundRect[c].br(), boundColor, 2, 8, 0 );
				cout << arcLength(approx[c], true) << endl;
			}
		}
		imshow("MyVideo", frame);
		if(waitKey(30) >= 0){
			cout << "esc is pressed" << endl;
			break;
		}
	}
}
