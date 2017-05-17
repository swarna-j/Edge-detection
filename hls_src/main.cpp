#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <stdio.h>
#include "edge.h"
#include <fstream>

#define INPUT_IMG_CORE "/home/jsk_027/PSOC_ED/EdgeDetection/skype.png"
//#define INPUT_IMG_CORE "/home/jsk_027/PSOC_ED/EdgeDetection/batman.png"
//#define INPUT_IMG_CORE "/home/jsk_027/PSOC_ED/EdgeDetection/lenna.bmp"
#define OUTPUT_IMG_CORE "/home/jsk_027/PSOC_ED/EdgeDetection/FilterResult.png"
#define OUTPUT_IMG_REF "/home/jsk_027/PSOC_ED/EdgeDetection/FilterResult_Ref.png"
using namespace cv;
using namespace std;

unsigned char outputImg[IMG_HEIGHT][IMG_WIDTH];
unsigned char tempImgRef[IMG_HEIGHT][IMG_WIDTH];
unsigned char outputImgRef[IMG_HEIGHT][IMG_WIDTH];

void conv(cv::InputArray imageIn, unsigned char imageOut[IMG_HEIGHT][IMG_WIDTH]);
short processWindowAndKernel(char *kernel, char *window, short numKernelElements);

int main()
{
   int i,j;
   // Input and output streams to test Top Function (edge_detect)
   hls::stream<uint32_channel> inStream;
   hls::stream<uint32_channel> outStream;
   // Values streamed in and streamed out
   uint32_channel inVal, outVal;
   rgb pixel_val;
   uint32_t pixelOut;
   //Read Image
   Mat imgIn;
   imgIn = imread(INPUT_IMG_CORE);   
    
   //OpenCV Mat pointing to Core and Reference output image arrays
   Mat cv_OutputImg(Size(imgIn.cols, imgIn.rows), CV_8UC1, outputImg, Mat::AUTO_STEP);
   Mat cv_OutputImgRef(Size(imgIn.cols, imgIn.rows), CV_8UC1, outputImgRef, Mat::AUTO_STEP);

   ofstream outfile;
   outfile.open("img_pixels.h");
   outfile<<"unsigned int img[49152] = { \n";
   // Pack RGB values in Input stream
   for(i = 0; i < IMG_WIDTH; i++)
   {
     for(j = 0; j < IMG_HEIGHT; j++)
     {
        pixel_val = imgIn.at<rgb>(i,j);
        inVal.data = pixel_val.blue;
        inVal.data = (inVal.data<<8)|pixel_val.green;
        inVal.data = (inVal.data<<8)|pixel_val.red;
        inVal.user = 1;
        inVal.id = 0; 
        inVal.dest = 0;
        inVal.keep = 1; 
        inVal.strb = 1;
        inStream << inVal;
        outfile<<(unsigned int)pixel_val.blue<<","<<(unsigned int)pixel_val.green<<","<<(unsigned int)pixel_val.red<<",\n";
     }
   }
   outfile<<"};";
   // Convolution on PL
   edge_detect(inStream, outStream);

   //Copy data from output stream to the outputImg array
   for(i = 0; i < IMG_WIDTH; i=i++)
   {
      for(j = 0; j < IMG_HEIGHT; j=j++)
      {      
    	  outStream.read(outVal);
    	  outputImg[i][j] = outVal.data;
      }
   }
   imwrite(OUTPUT_IMG_CORE, cv_OutputImg);
/*
   cvtColor(imgIn, imgIn, CV_BGR2GRAY);
      // Convolution
   conv(imgIn, tempImgRef);

   Mat tempSrc = Mat(128, 128, CV_8UC1, tempImgRef);
   conv(tempSrc, outputImgRef);

   ofstream outOpenCV;
   outOpenCV.open("openCv_out.txt");
   for(i = 0; i< 128; i++)
   {
	   for(j = 0; j < 128; j++)
	   {
		   outOpenCV<<(unsigned int)outputImgRef[i][j]<<"\n";
	   }
   }
   outOpenCV.close();
   imwrite(OUTPUT_IMG_REF, cv_OutputImgRef);*/
   return 0;
}

void conv(cv::InputArray imageIn, unsigned char imageOut[IMG_HEIGHT][IMG_WIDTH])
{
	static bool isFirst = 1;
	char kernel[9];
	if(isFirst){
		for(int i = 0; i < 9; i++)
			kernel[i] = x_op[i];
		isFirst = 0;
	}
	else
	{
		for(int i = 0; i < 9; i++)
			kernel[i] = y_op[i];
	}
	// Get the kernel center (For example on the 3x3 is 1)
	int kCenterX = K_ROW / 2;
	int kCenterY = K_COL / 2;
	short accConvolution = 0;
	short numKernelElements = K_ROW * K_COL;

	// Sampling window (Should be the same size of the kernel)
	unsigned char samplingWindow[K_ROW][K_COL];

	// Iterate on the image pixels
	for (int rows = 0; rows < IMG_WIDTH; rows++)
	{
		for (int cols = 0; cols < IMG_HEIGHT; cols++)
		{
			// Accumulator for the convolution should be reset
			accConvolution = 0;
			// Avoid calculate out of the image boundaries
			if ((rows >= K_ROW-1) && (cols >= K_COL-1))
			{
				// Populate our window by sampling on the image
				for (int rowsWindow = 0; rowsWindow < K_ROW; rowsWindow++)
				{
					for (int colsWindow = 0; colsWindow < K_COL; colsWindow++)
					{
						// The window must be centered on the current pixel, so we need to
						// subtract some pixels back from the current positon
						int repos_row = rows-(K_ROW-1)+rowsWindow;
						int repos_col = cols-(K_COL-1)+colsWindow;
						short currPixVal = imageIn.getMat().at<unsigned char>(repos_row,repos_col);
						samplingWindow[rowsWindow][colsWindow] = currPixVal;
					}
				}

				// Now multiply the window by the kernel and sum it's result
				accConvolution = processWindowAndKernel((char*)kernel,(char*)samplingWindow,numKernelElements);
			}

			imageOut[rows][cols] = accConvolution;
		}
	}
}

short processWindowAndKernel(char *kernel, char *window, short numKernelElements)
{
	short accumulator = 0;
	for (int idx = 0; idx < numKernelElements; idx++)
	{
		accumulator = accumulator + (kernel[idx] * (unsigned char)window[idx]);
	}
	return accumulator;
}
