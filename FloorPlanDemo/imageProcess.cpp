//  ---------------------------------------------------------------------------
//
//  @file       FloorPlanDemo
//  @tools used: 
//              AntTweakBar: http://anttweakbar.sourceforge.net/doc
//              OpenGL:      http://www.opengl.org
//				OpenCV:		 http://www.opencv.org
//              GLUT:        http://opengl.org/resources/libraries/glut
//  
//  @author     Yeqi Wang
//  @date       02/25/2015
//
//  ---------------------------------------------------------------------------

#include "imageProcess.h"

std::vector<std::vector<cv::Point> > contours_poly;
int maxImg;

void imageShow(Mat frame, String name){
	Mat img = frame;
	if (img.empty())
	{
		std::cout << "error";
		return;
	}
	imshow(name, img);
}

Mat colorExtract(Mat &src){
	Mat dst, res;
	cvtColor(src, dst, COLOR_BGR2HSV);
	Scalar hsv_min(30, 50, 50);
	Scalar hsv_max(90, 255, 255);
	inRange(dst, hsv_min, hsv_max, res);
	return res;
}

void erodeAndDilate(Mat &src, int erosion_size){
	int erosion_type = MORPH_ELLIPSE;
	Mat element = getStructuringElement(erosion_type,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));
	erode(src, src, element);
	dilate(src, src, element);
}

Mat imageProcess(Mat src, int picIndex){
	long startTime = clock();
	//erodeAndDilate(src, 10);
	cvtColor(src, src, COLOR_BGRA2GRAY);
	src = ~src;
	Scalar rgb_min(0);
	Scalar rgb_max(250);
	threshold(src, src, 230, 255, 0);
	//inRange(src, rgb_min, rgb_max, src);

	erodeAndDilate(src, 1);
	Mat midImage, dstImage;
	cvtColor(src, dstImage, CV_GRAY2BGR);
	medianBlur(src, src, 5);

	int thresh = 100;
	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(src, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	contours_poly.resize(contours.size());
	// approximate contours
	for (int i = 0; i < contours.size(); i++) {
		approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 1, true);
	}

	std::ofstream myfile;
	myfile.open("Output/edgeData/"+std::to_string(picIndex) + "_info.txt");

	Mat drawing = Mat::zeros(src.size(), CV_8UC3);
	std::cout << contours_poly.size() << std::endl;
	for (int i = 0; i < contours_poly.size(); i++)
	{
		drawContours(dstImage, contours_poly, i, Scalar(0, 0, 255), 1, 8, hierarchy, 0, Point());
		drawContours(drawing, contours_poly, i, Scalar(0, 0, 255), 1, 8, hierarchy, 0, Point());
		//std::cout << contours_poly[i] << std::endl;	
		myfile << contours_poly[i];
		myfile << "\n";
	}
	myfile.close();

	/// Show in a window
	namedWindow("Contours", CV_WINDOW_AUTOSIZE);
	imshow("Contours", drawing);
	imshow("dst", dstImage);
	return drawing;
}

void openCVProcess(){
	for (int i = 15; i < 16; i++){
		String s = ("data/") + std::to_string(i) + (".jpg");
		std::cout << s << std::endl;
		Mat src = imread(s);
		imshow("src", src);
		Mat dst = imageProcess(src, i);
		String d = ("output/image/") + std::to_string(i) + ("Out.jpg");
		imwrite(d, dst);
	}

	char waitkey = waitKey(0);
	while (waitkey != 'q'){
		waitKey(0);
	}
}