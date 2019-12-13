#include "HandGesture.hpp"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/background_segm.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

//findContours -> devuelve una imagen con una linea de color con el contorno detectado (generalmente más de uno)(lo devuelve como secuencias de píxeles)


HandGesture::HandGesture() {
	
}


double HandGesture::getAngle(Point s, Point e, Point f) {
	
	double v1[2],v2[2];
	v1[0] = s.x - f.x;
	v1[1] = s.y - f.y;

	v2[0] = e.x - f.x;
	v2[1] = e.y - f.y;

	double ang1 = atan2(v1[1],v1[0]);
	double ang2 = atan2(v2[1],v2[0]);

	double angle = ang1 - ang2;
	if (angle > CV_PI) angle -= 2 * CV_PI;
	if (angle < -CV_PI) angle += 2 * CV_PI;
	return (angle * 180.0/CV_PI);
}
void HandGesture::FeaturesDetection(Mat mask, Mat output_img, int flag) {
	
	vector<vector<Point> > contours;
	Mat temp_mask; //copia de la máscara  //-> falla si es un negro puro (se soluciona pintando un circulito en una esquina)
	
	mask.copyTo(temp_mask);
		
		int index = 0;
	// hacemos find contours
		findContours(temp_mask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	// entramos en un bucle y cogemos el mas largo
        // CODIGO 3.1
        // detección del contorno de la mano y selección del contorno más largo
        //...
		
		int maxContour = contours[0].size();
		for(int i = 1; i < contours.size(); i++){
			if(maxContour < contours[i].size()){
				maxContour = contours[i].size();
				index = i;
			}
		}
        // pintar el contorno
        //...
		drawContours(output_img, contours, index , cv::Scalar(255, 0, 0), 2, 8, vector<Vec4i>(), 0, Point());

		
	
	//obtener el convex hull	
	vector<int> hull;
	convexHull(contours[index], hull);
	 
	// pintar el convex hull
	Point pt0 = contours[index][hull[hull.size()-1]];
	for (int i = 0; i < hull.size(); i++)
	{
		Point pt = contours[index][hull[i]];
		line(output_img, pt0, pt, Scalar(0, 0, 255), 2, CV_AA);
		pt0 = pt;
	}
        //obtener los defectos de convexidad
		
	vector<Vec4i> defects;
	convexityDefects(contours[index], hull, defects);	
	Rect boundRect = boundingRect(contours[index]);
	float maxSize;
	float minSize;
	Point rectcenter((boundRect.tl().x + boundRect.br().x) / 2, (boundRect.tl().y + boundRect.br().y) / 2);
	if (boundRect.height > boundRect.width){
		maxSize = boundRect.height;
		minSize = boundRect.width;
	}else{
		maxSize = boundRect.width;
		minSize = boundRect.height;
	}
        
		
			
	int cont = 0;
	int fPointNum = 0;
	for (int i = 0; i < defects.size(); i++) {
		Point s = contours[index][defects[i][0]];
		Point e = contours[index][defects[i][1]];
		Point f = contours[index][defects[i][2]];
		float depth = (float)defects[i][3] / 256.0;
		double angle = getAngle(s, e, f);

	    // CODIGO 3.2
        // filtrar y mostrar los defectos de convexidad
        //...
		
		if(angle < 120.0 && depth > maxSize*0.23){	
		  	fPointNum ++;		
			circle(output_img, f, 3, cv::Scalar(0, 255, 0), 5); //vector<Point> far_points;
	vector<Point> start_points;
	
		}
    }
	
	circle(output_img, rectcenter, 3, cv::Scalar(0, 255, 0), 5);
	// Pintar rectangulo del area minima del contorno (boundingRect)
		
	 rectangle(output_img, boundRect, Scalar(0,255,255), 2);
	 
		
	
	
	switch (fPointNum)
	{
		case 1: putText(output_img, "2", Point(100,100),3,3,Scalar(0,0,0) ); 
				break;

		case 2: putText(output_img, "3", Point(100,100),3,3,Scalar(0,0,0) ); 
				break;

		case 3: putText(output_img, "4", Point(100,100),3,3,Scalar(0,0,0) ); 
				break;

		case 4: putText(output_img, "5", Point(100,100),3,3,Scalar(0,0,0) ); 
				break;

		default:
			if(maxSize/minSize < 1.2 ){
				rectangle(output_img, boundRect, Scalar(255,255,0), 2);
		
				putText(output_img, "0", Point(100,100),3,3,Scalar(0,0,0) ); 
	          	Point oldrectcenter = rectcenter; 
			  	line(output_img, rectcenter , oldrectcenter, Scalar(0, 0, 255), 2, CV_AA);
				
	
			} else {
				putText(output_img, "1", Point(100,100),3,3,Scalar(0,0,0) ); 
			}
				
			break;
	}

}


