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

void lineRefinement(Point &p1, Point &p2, float refinRatio){
	if (abs(p1.x - p2.x) < refinRatio){
		p2.x = p1.x;
	}
	if (abs(p1.y - p2.y) < refinRatio){
		p2.y = p1.y;
	}
}

bool isOpposite(Edge &e1, Edge &e2){
	Point p1 = e1.p2 - e1.p1;
	Vec2f v1(p1.x, p1.y);
	Point p2 = e2.p2 - e2.p1;
	Vec2f v2(p2.x, p2.y);
	Vec2f goal = v1/norm(v1) + v2/norm(v2);
	//std::cout << goal << std::endl;
	if (goal[0] < 0.1 && goal[0] > -0.1 && goal[1] < 0.1 && goal[1] > -0.1){
		return true;
	}
	else{
		return false;
	}
	
}

double calTwoVectorGap(Vec2f &v1, Vec2f & v2){
	double res = v1.ddot(v2) / norm(v2);
	return res > 0 ? res : 0;
}

double edgeGap(Edge &e1, Edge &e2){
	// Vector 1 & 2
	Vec2f v1(e1.p2.x - e1.p1.x, e1.p2.y - e1.p1.y);
	Vec2f v2(e2.p1.x - e2.p2.x, e2.p1.y - e2.p2.y);

	// Vector 3 & 4 relate to 1
	Vec2f v3(e2.p2.x - e1.p1.x, e2.p2.y - e1.p1.y);
	Vec2f v4(e2.p1.x - e1.p1.x, e2.p1.y - e1.p1.y);

	// Vector 5 & 6 relate to 2
	Vec2f v5(e1.p1.x - e2.p2.x, e1.p1.y - e2.p2.y);
	Vec2f v6(e1.p2.x - e2.p2.x, e1.p2.y - e2.p2.y);

	double res = min(abs(calTwoVectorGap(v1, v4) - calTwoVectorGap(v1, v3)), abs(calTwoVectorGap(v2, v5) - calTwoVectorGap(v2, v6)));
	return res;
}

float pDistance(Point e1p1, Point e1p2, Point e2p1, Point e2p2){
	float distance;
	Point temp;
	temp.x = (e2p2.x + e2p1.x) / 2;
	temp.y = (e2p2.y + e2p1.y) / 2; 
	if (e1p1.x - e1p2.x == 0){
		distance = abs(temp.x - e1p1.x);
		return distance;
	}
	float k = (e1p1.y - e1p2.y) / (e1p1.x - e1p2.x);
	float b = e1p1.y - k * e1p1.x;

	distance = (k*temp.x - temp.y + b) / sqrt(k*k + 1);
	return abs(distance);
}

vector<Edge> contoursToMap(vector<vector<Point>> &contours, float erosionRatio, int picIndex){
	vector<Edge> edgeMap;
	for (int i = 0; i < contours.size(); i++){
		for (int j = 0; j < contours[i].size(); j++){
			int second = j != contours[i].size() - 1 ? j + 1 : 0; // Next point index
			double distance = norm(Mat(contours[i][j]), Mat(contours[i][second]));
			if (distance > erosionRatio){
				Edge tempEdge;
				tempEdge.index = edgeMap.size();
				tempEdge.oppositeIndex = -1;
				tempEdge.span = INT_MAX;
				tempEdge.p1 = contours[i][j];
				tempEdge.p2 = contours[i][second];
				tempEdge.sign = false;
				edgeMap.push_back(tempEdge);
			}
		}
	}
	for (int i = 0; i < edgeMap.size(); i++){
		int j;
		float minEdge = INT_MAX;
		int minEdgeIndex = -1;
		for (j = 0; j < edgeMap.size(); j++){
			if (i != j && isOpposite(edgeMap[i], edgeMap[j]) && edgeGap(edgeMap[i], edgeMap[j])>0 
				&& !edgeMap[i].sign && !edgeMap[j].sign && pDistance(edgeMap[i].p1, edgeMap[i].p2, edgeMap[j].p1, edgeMap[j].p2)< edgeMap[i].span ){
						minEdge = pDistance(edgeMap[i].p1, edgeMap[i].p2, edgeMap[j].p1, edgeMap[j].p2);
						minEdgeIndex = j; 
						edgeMap[i].span = minEdge;
					}
		}
		if (minEdgeIndex != -1 && i != minEdgeIndex && minEdge < 20){
			float fi = norm(edgeMap[i].p1 - edgeMap[i].p2);
			float fj = norm(edgeMap[minEdgeIndex].p1 - edgeMap[minEdgeIndex].p2);
			int smallerIndex = fi < fj ? i : minEdgeIndex;
			int biggerIndex = fi < fj ? minEdgeIndex : i;
			edgeMap[smallerIndex].span = minEdge;
			edgeMap[smallerIndex].sign = true;
			edgeMap[smallerIndex].oppositeIndex = biggerIndex;
		}
		
	}

	vector<Edge> res;
	for (int i = 0; i < edgeMap.size(); i++){
		Edge temp = edgeMap[i];
		if (temp.sign)
			res.push_back(temp);
	//	std::cout << temp.index << " " << temp.oppositeIndex << " " << temp.p1 << " " << temp.p2 << " " << temp.span << std::endl;
	}

	return res;
}

Mat printContours(vector<Node> nodeMap,Mat &src){
	Mat refinedEdge = ~Mat::zeros(src.size(), CV_8UC3);
	for (int i = 0; i < nodeMap.size(); i++){
		for (int j = 0; j < nodeMap[i].indexMap.size(); j++){
			Point p1 = nodeMap[i].vertex;
			int index = nodeMap[i].indexMap[j];
			Point p2 = nodeMap[index].vertex;
			line(refinedEdge, cvPoint(p1.x, p1.y), cvPoint(p2.x, p2.y), Scalar(0, 0, 255), 3, 8);
		}
	}
	imshow("Solution 1", refinedEdge);
	return refinedEdge;
}

Mat printEdges(vector<Edge> edgeMap, Mat &src, int picIndex){
	std::ofstream myfileStream;
	String d = ("output/edgeData2/") + std::to_string(picIndex) + ("_info.txt");
	myfileStream.open(d);
	myfileStream << "Edge map size: " << edgeMap.size() << "\n\n";
	Mat edges2 = ~Mat::zeros(src.size(), CV_8UC3);
	for (int i = 0; i < edgeMap.size(); i++){
			Edge temp = edgeMap[i];
				Point p1 = temp.p1;
				Point p2 = temp.p2;
				//if (temp.span > 50) temp.span = 10;
				//line(edges2, cvPoint(p1.x, p1.y), cvPoint(p2.x, p2.y), Scalar(0, 0, 255), 1, 8);
				line(edges2, cvPoint(p1.x, p1.y), cvPoint(p2.x, p2.y), Scalar(0, 0, 255), temp.span, 8);
				myfileStream << temp.p1 << " " << temp.p2 << " " << temp.span << "\n\n";
			
	}
	myfileStream.close();
	imshow("Solution 2", edges2);
	return edges2;
}

Mat imageProcess(Mat src, int picIndex){
	resize(src, src, Size(1024, 768), 0, 0, INTER_CUBIC); // resize to 1024x768 resolution
	cvtColor(src, src, COLOR_BGRA2GRAY);
	src = ~src;
	imshow("Threshold: ", src);
	threshold(src, src, 180, 255, 0);
	erodeAndDilate(src, 1);
	Mat midImage, dstImage;
	dstImage = ~dstImage;
	cvtColor(src, dstImage, CV_GRAY2BGR);
//	medianBlur(src, src, 5);

//	GaussianBlur(src, src, Size(5, 5),0, 0);
	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(src, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	contours_poly.resize(contours.size());
	// approximate contours
	for (int i = 0; i < contours.size(); i++) {
		approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 1, true);
	}

	Mat rawEdge = ~Mat::zeros(src.size(), CV_8UC3);

	for (int i = 0; i < contours_poly.size(); i++)
	{
		drawContours(rawEdge, contours_poly, i, Scalar(0, 0, 255), 1, 8, hierarchy, 0, Point());
	}
	imshow("Double edges", rawEdge);

	vector<Edge> edgeMap = contoursToMap(contours_poly, 10, picIndex);
	//vector<Node> nodeMap = edgeMapToNodeMap(edgeMap);
	Mat edges = printEdges(edgeMap, src, picIndex);

	/// Show in a window
	return edges;
}

void openCVProcess(int imgNum){

	for (int i = 0; i < imgNum; i++){
		String s = ("data/") + std::to_string(i) + (".jpg");
		std::cout << s << std::endl;
		Mat src = imread(s);
		imshow("src", src);
		long startTime = clock();
		Mat dst = imageProcess(src, i);
		long endTime = clock();
		std::cout << "Time: " << endTime - startTime << "ms" << std::endl;
		String d = ("output/image/") + std::to_string(i) + ("Out.jpg");
		imwrite(d, dst);
	}
	char waitkey = waitKey(0);
	while (waitkey != ' '){
		waitKey(0);
	}
} 