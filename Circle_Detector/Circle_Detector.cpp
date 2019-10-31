#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

//#include "depth.h"

using namespace cv;
using namespace std;

Mat src; Mat dst_gray;
bool checkEllipseShape(Mat src, vector<Point> contour, RotatedRect ellipse, double ratio = 0.01)  //double ratio=0.01
{
	//get all the point on the ellipse point
	vector<Point> ellipse_point;

	//get the parameter of the ellipse
	Point2f center = ellipse.center;
	double a_2 = pow(ellipse.size.width*0.5, 2);
	double b_2 = pow(ellipse.size.height*0.5, 2);
	double ellipse_angle = (ellipse.angle*3.1415926) / 180;


	//the uppart
	for (int i = 0; i < ellipse.size.width; i++)
	{
		double x = -ellipse.size.width*0.5 + i;
		double y_left = sqrt((1 - (x*x / a_2))*b_2);

		//rotate
		//[ cos(seta) sin(seta)]
		//[-sin(seta) cos(seta)]
		cv::Point2f rotate_point_left;
		rotate_point_left.x = cos(ellipse_angle)*x - sin(ellipse_angle)*y_left;
		rotate_point_left.y = +sin(ellipse_angle)*x + cos(ellipse_angle)*y_left;

		//trans
		rotate_point_left += center;

		//store
		ellipse_point.push_back(Point(rotate_point_left));
	}
	//the downpart
	for (int i = 0; i < ellipse.size.width; i++)
	{
		double x = ellipse.size.width*0.5 - i;
		double y_right = -sqrt((1 - (x*x / a_2))*b_2);

		//rotate
		//[ cos(seta) sin(seta)]
		//[-sin(seta) cos(seta)]
		cv::Point2f rotate_point_right;
		rotate_point_right.x = cos(ellipse_angle)*x - sin(ellipse_angle)*y_right;
		rotate_point_right.y = +sin(ellipse_angle)*x + cos(ellipse_angle)*y_right;

		//trans
		rotate_point_right += center;

		//store
		ellipse_point.push_back(Point(rotate_point_right));

	}


	vector<vector<Point> > contours1;
	//contours1.push_back(ellipse_point);
	//drawContours(src,contours1,-1,Scalar(255,0,0),2);

	//match shape
	double a0 = matchShapes(ellipse_point, contour, CV_CONTOURS_MATCH_I1, 0);
	if (a0 > 0.1)  //if (a0>0.01)
	{
		return true;
	}

	return false;
}

/** @function main */
int main(int argc, char** argv)
{
	//load images
	string image_file_name = "C:/Program1/Circle_Detector/Circle_Detector/";
	string file = image_file_name + "tb11.jpg";
	src = imread(file, 1);
	Mat dst;
	src.copyTo(dst);
	cvtColor(dst, dst_gray, CV_BGR2GRAY);
	Mat threshold_output;
	vector<vector<Point> > contours;
	// find contours
	int threshold_value = threshold(dst_gray, threshold_output, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);    //0, 255, CV_THRESH_BINARY
	imshow("binary image", threshold_output);
	findContours(threshold_output, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE); //CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE
	//fit ellipse
	vector<RotatedRect> minEllipse(contours.size());
	int t = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		//point size check
		//if (contours[i].size() < 10 )   // 6 ,15 if (contours[i].size() < 30 || contours[i].size() > 100000)   // 6 ,15
		//{
		//	continue;
		//}
		Scalar color = Scalar(0, 0, 255);
		
		//point area
		if (contourArea(contours[i]) < 5 )   //contours[i])<10
		{
			continue;
		}
		drawContours(src, contours, i, color, 1, 8);
		minEllipse[i] = fitEllipse(Mat(contours[i]));

		//check shape
		if (checkEllipseShape(dst, contours[i], minEllipse[i]))
		{
			continue;
		}
		float center_x = minEllipse[i].center.x + 300;
		float center_y = minEllipse[i].center.y + 101;
		char label[50];
		//Scalar baseline;
		//putText(dst, label, Point(minEllipse[i].center.x - 2, minEllipse[i].center.y - 2), CV_FONT_HERSHEY_TRIPLEX, 0.4, Scalar(00, 00, 100), 1, 4);  //CV_FONT_HERSHEY_DUPLEX//FONT_HERSHEY_COMPLEX // CV_FONT_HERSHEY_COMPLEX//CV_FONT_HERSHEY_TRIPLEX
		cout << t << endl;
		// ellipse
		ellipse(dst, minEllipse[i], color, 1);
		//ellipse(src,  minEllipse[i].center, Size (minEllipse[i].size.height/2, minEllipse[i].size.width/2), minEllipse[i].angle, 0, 360,  color, 2);
		//   drawContours( drawing,contours_poly, i, color, 1, 8,vector<Vec4i>(), 0, Point() );
		
	}

	imshow("srcc", src);
	/// 结果在窗体中显示
	namedWindow("ellipse", 0);
	imshow("ellipse", dst);
	imshow("ellipse_2", src);
	waitKey(0);
	imwrite("ellipse.jpg", dst);
	imwrite("ellipse_detected.jpg", src);


	return(0);
}
