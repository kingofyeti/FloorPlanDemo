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

vector<Node> contoursToMap(vector<vector<Point>> &contours, float erosionRatio, int picIndex){
	vector<Node> nodeMap;
	int preIndex;
	for (int i = 0; i < contours.size(); i++){
		Node first;
		vector<int> fIndexmap;
		first.indexMap = fIndexmap;
		first.vertex = contours[i][0];
		first.index = nodeMap.size();
		nodeMap.push_back(first);
		preIndex = first.index;
		for (int j = 1; j < contours[i].size(); j++){
			int sign = false;
			int k;
			Point p1 = contours[i][j];
			for (k = 0; k < nodeMap.size(); k++){
				Point p2 = nodeMap[k].vertex;
				double distance = norm(Mat(p1), Mat(p2));
				if (distance < erosionRatio){
					sign = true;
					break;
				}
			}
			if (sign == true){
			//	nodeMap[k].vertex.x = (nodeMap[k].vertex.x + p1.x) / 2;
			//	nodeMap[k].vertex.y = (nodeMap[k].vertex.y + p1.y) / 2;
				preIndex = nodeMap[k].index;
			}
			else{
				Node temp;
				temp.vertex = p1;
				vector<int> indexmap;
				indexmap.push_back(preIndex);
				nodeMap[preIndex].indexMap.push_back(nodeMap.size());
				temp.index = nodeMap.size();
				lineRefinement(nodeMap[preIndex].vertex, contours[i][j], 10);
				temp.vertex = contours[i][j];
				temp.indexMap = indexmap;
				nodeMap.push_back(temp); 
				preIndex = temp.index;
			}
		}
	}
	std::ofstream myfileStream;
	String d = ("output/edgeData/") + std::to_string(picIndex) + ("_info.txt");
	std::cout << d << std::endl;
	std::cout <<  nodeMap.size()<< std::endl;
	myfileStream.open(d);
	myfileStream << "Node map size: " << nodeMap.size() << std::endl;
	for (Node x : nodeMap){
		//myfileStream << "Vertex index: " << x.index << "\n";
		//myfileStream << "Vertex position: " << x.vertex << "\n";
		//myfileStream << "Vertex connected: ";
		myfileStream << x.index << "\n";
		myfileStream << x.vertex.x << " "<< x.vertex.y << "\n";
		for (auto y : x.indexMap){
			myfileStream << y << " ";
		}
		myfileStream << "\n\n";
	}
	myfileStream.close();
	return nodeMap;
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

vector<Edge> contoursToMap2(vector<vector<Point>> &contours, float erosionRatio, int picIndex){
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
				edgeMap.push_back(tempEdge);
			}
		}
	}
	for (int i = 0; i < edgeMap.size(); i++){
		int j;
		int curMin = -1;
		float distance = 100;
		bool got = false;
		for (j = 0; j < i; j++){
			if (i != j && isOpposite(edgeMap[i], edgeMap[j])){
				float localDistance = pDistance(edgeMap[i].p1, edgeMap[i].p2, edgeMap[j].p1, edgeMap[j].p2);
				if (localDistance < edgeMap[i].span || localDistance < edgeMap[j].span){
					distance = localDistance;
					//std::cout << "YES@!!!!!" << i << " " << j << " " << distance << std::endl;
					curMin = j;
					edgeMap[i].span = min(edgeMap[i].span,distance);
					edgeMap[j].span = min(edgeMap[j].span, distance);
					got = true;
					std::cout << i << " " << j << std::endl;
				}
			}	
		}
		if (got){
			edgeMap[i].span = distance;
			edgeMap[curMin].span = distance;
			edgeMap[i].oppositeIndex = curMin;
			edgeMap[curMin].oppositeIndex = i;
		}
	}

	for (int i = 0; i < edgeMap.size(); i++){
		Edge temp = edgeMap[i];
		std::cout << temp.index << " " << temp.oppositeIndex << " " << temp.p1 << " " << temp.p2 << " " << temp.span << std::endl;
	}

	Point a = { 40, 434 };
	Point b = { 40, 729 };
	Point c = { 47, 722 };
	Point d = { 47, 434 };
	std::cout << pDistance(a, b, c, d) << " ????" << std::endl;

	//for (int i = 0; i < edgeMap.size(); i++){
	//	Edge e1 = edgeMap[i];
	//	if (e1.span < INT_MAX){
	//		Node n1, n2;
	//		n1.index = nodeMap.size();
	//		n2.index = nodeMap.size()+1;
	//		n1.vertex = edgeMap[i].p1;
	//		n2.vertex = edgeMap[i].p2;
	//		n1.indexMap.push_back(n2.index);
	//		n2.indexMap.push_back(n1.index);
	//		nodeMap.push_back(n1);
	//		nodeMap.push_back(n2);
	//	}
	//}

	return edgeMap;
}

Mat printContours(Vector<Node> nodeMap,Mat &src){
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

Mat printEdges(Vector<Edge> edgeMap, Mat &src){
	Mat edges2 = ~Mat::zeros(src.size(), CV_8UC3);
	//for (int i = 0; i < edgeMap.size(); i++){
	//	Edge temp = edgeMap[i];
	//	std::cout << temp.index << " " << temp.oppositeIndex << " " << temp.p1 << " " << temp.p2 << " " << temp.span << std::endl;
	//}
	imshow("Solution 2", edges2);
	return edges2;
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

	erodeAndDilate(src, 1.6);
	Mat midImage, dstImage;
	dstImage = ~dstImage;
	cvtColor(src, dstImage, CV_GRAY2BGR);
	medianBlur(src, src, 5);
	imshow("Threshold: ", src);

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

	Mat rawEdge = ~Mat::zeros(src.size(), CV_8UC3);

	for (int i = 0; i < contours_poly.size(); i++)
	{
		drawContours(rawEdge, contours_poly, i, Scalar(0, 0, 255), 1, 8, hierarchy, 0, Point());
	}
	imshow("Double edges", rawEdge);

	//vector<Node> nodeMap = contoursToMap(contours_poly, 10, picIndex);
	//Mat refinedEdge = printContours(nodeMap,src);
	vector<Edge> edgeMap = contoursToMap2(contours_poly, 10, picIndex);
	Mat edges2 = printEdges(edgeMap,src);


	/// Show in a window
	imshow("dst", dstImage);
	return rawEdge;
}

void openCVProcess(int imgNum){

	for (int i = 0; i < imgNum; i++){
		String s = ("data/") + std::to_string(i) + (".jpg");
		std::cout << s << std::endl;
		Mat src = imread(s);
		resize(src, src, Size(1024, 768), 0, 0, INTER_CUBIC); // resize to 1024x768 resolution
		imshow("src", src);
		Mat dst = imageProcess(src, i);
		String d = ("output/image/") + std::to_string(i) + ("Out.jpg");
		imwrite(d, dst);
	}
	char waitkey = waitKey(0);
	while (waitkey != ' '){
		waitKey(0);
	}
} 