#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/background_segm.hpp>

#include <stdio.h>
#include <string>
#include <iostream>

#include "MyBGSubtractorColor.hpp"
#include "HandGesture.hpp"

using namespace std;
using namespace cv;



int main(int argc, char** argv)
{

	Mat frame, bgmask, out_frame;



	//Abrimos la webcam

	VideoCapture cap;
	cap.open(0); //capta la primera cámara que detecta
	if (!cap.isOpened())
	{
		printf("\nNo se puede abrir la c�mara\n");
		return -1;
	}
        int cont = 0;
        while (frame.empty()&& cont < 2000 ) {

                cap >> frame;
                ++cont;
        }
        if (cont >= 2000) {
                printf("No se ha podido leer un frame v�lido\n");
                exit(-1);
        }

	// Creamos las ventanas que vamos a usar en la aplicaci�n

	namedWindow("Reconocimiento");
	namedWindow("Fondo");

        // creamos el objeto para la substracci�n de fondo
	MyBGSubtractorColor mascara(cap);

	// creamos el objeto para el reconocimiento de gestos

	// iniciamos el proceso de obtenci�n del modelo del fondo


	mascara.LearnModel();
	for (;;)
	{
		cap >> frame;
		flip(frame, frame, 1);
		if (frame.empty())
		{
			printf("Le�do frame vac�o\n");
			continue;
		}
		int c = cvWaitKey(40);
		
		int paint;
		if ((char)c == 'q') break;
		if ((char)c == 'd') paint = 1;
		if ((char)c == 'e') paint = 0;

		// obtenemos la m�scara del fondo con el frame actual

		mascara.ObtainBGMask(frame, bgmask);
                // CODIGO 2.1
                // limpiar la m�scara del fondo de ruido
                //...
		
		//Filtro de la mediana. Reduce el ruido. Colorea una zona con el color que más predomine reduciendo el ruido
		medianBlur(bgmask, bgmask, 5); //Esto a lo mejor no nos hace falta
		
		int dilation_size = 0.7;

		Mat element = getStructuringElement(MORPH_RECT,Size(2 * dilation_size + 1, 2 * dilation_size + 1), Point(dilation_size, dilation_size));
		
		dilate(bgmask,bgmask, element);	
		dilate(bgmask,bgmask, element);
		dilate(bgmask,bgmask, element);
		erode(bgmask, bgmask , element);
		medianBlur(bgmask, bgmask, 5); //Esto a lo mejor no nos hace falta
		
		dilate(bgmask,bgmask, element);
		erode(bgmask, bgmask , element);
		
 		
		// deteccion de las caracter�sticas de la mano
		
		
                // mostramos el resultado de la sobstracci�n de fondo	
	        circle(bgmask, Point(0,0), 2, cv::Scalar(255, 255, 255), -1);
		imshow("Fondo", bgmask);	
        
	
		// mostramos el resultado del reconocimento de gestos
		HandGesture reconocimiento;
		reconocimiento.FeaturesDetection(bgmask, frame, paint);
	
	        imshow("Reconocimiento", frame);
	

	}

	destroyWindow("Reconocimiento");
	destroyWindow("Fondo");
	cap.release();
	return 0;
}
 
