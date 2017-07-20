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
	/* чтение видео */
	VideoCapture cap(filename);
	if(!cap.isOpened()){
		cout << "Oops! I have not found the video"<< endl;
		return -1;
	}

	double fps = cap.get(CV_CAP_PROP_FPS);
	cout << "Frame per seconds : " << fps << endl;

	Mat gray; // матрица для хранения изображения в черно-белом
	Mat edges; // матрица для хранения изображения в двоичном виде после обработки фильтрами
	Mat closed; // матрица для хранения изображения в двоичном виде после соединения контуров
	vector<vector<Point> > contours; // массив для хранения контуров

	vector<Vec4i> hierarchy;

	Mat frame; // матрица для хранения считываемого фрейма
	double peri; // переменная для периметра контура
	double area; // переменная для площади контура

	Scalar boundColor( 0, 255, 0 ); // цвет для boundbox - зеленый
	Scalar Color( 0, 0, 255 ); // цвет контура - красный
	while(1){
		bool bSuccess = cap.read(frame); // чтение фрейма из видео
		if(!bSuccess){
			cout << "Oops! Can't read the frame from video" << endl;
			break;
		}
		cvtColor(frame, gray, CV_BGR2GRAY);
		threshold(gray, gray, 128, 255, CV_THRESH_BINARY);
		Mat kernel = getStructuringElement(MORPH_RECT, Size(35, 35));
		morphologyEx(gray, gray, MORPH_OPEN, kernel);
		morphologyEx(gray, gray, MORPH_CLOSE, kernel);
		threshold(gray, gray, 128, 255, CV_THRESH_BINARY_INV);


		Canny(gray, edges, 10, 250);
		morphologyEx(edges, closed, MORPH_CLOSE, kernel);
		findContours( closed, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));

		vector<vector<Point> > approx(contours.size());
		vector<Rect> boundRect( contours.size() );

		for(size_t c = 0; c < contours.size(); c++){
			peri = arcLength(contours[c], true); // подсчет периметра контура
			area = contourArea(contours[c]); // подсчет площади контура
			approxPolyDP(Mat(contours[c]), approx[c], 0.002*peri, true); // аппрокcимация контура
			if(approx[c].size() <= 12){
				boundRect[c] = boundingRect( Mat(approx[c]) ); // boundbox
				drawContours(frame, approx, -1, Color, 4); // рисование контуров
				rectangle( frame, boundRect[c].tl(), boundRect[c].br(), boundColor, 2, 8, 0 ); // рисование boundbox
			}
		}
		imshow("MyVideo", frame); // вывод кадра на экран
		if(waitKey(30) >= 0){
			cout << "esc is pressed" << endl;
			break;
		}
	}
}
