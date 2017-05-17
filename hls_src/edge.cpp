#include "edge.h"
//#include <fstream> //For debug only

using namespace std;

//ofstream gray, edge;

void edge_detect(hls::stream<uint32_channel> &inStream, hls::stream<uint32_channel> &outStream)
{
#pragma HLS_INTERFACE axis_port = inStream
#pragma HLS_INTERFACE axis_port = outStream
#pragma HLS INTERFACE s_axilite port=return bundle=CRTL_BUS
    int idx;
    uint8_t pixelOut[N]; //Single channel
    uint8_t grayOut[N];
    uint32_t tempIn, tempOut;
    rgb pixelIn[N]; //3 channels for R,G,B components
    uint32_channel RGBInputChannel;
    uint32_channel outputChannel;
    
    for(idx = 0; idx < N; idx++)
    {
#pragma HLS PIPELINE
      //Read and Cache Input channel value once (Block here if FIFO sender is empty)
    	RGBInputChannel = inStream.read();
    	tempIn = RGBInputChannel.data;
        //Extract RGB Values - Format: [7:0]->R, [15:8]->G, [23:16]->B
    	pixelIn[idx].red = tempIn;
    	tempIn = tempIn>>8; //Shift [15:8] bits to [7:0]
        pixelIn[idx].green = tempIn;
        tempIn = tempIn>>8; //Shift [23:16] bits to [7:0]
        pixelIn[idx].blue = tempIn;
    }
    convert_to_gray(pixelIn,grayOut);
    find_edges (grayOut, pixelOut);

    for(idx = 0; idx < N-1; idx++)
    {
    	outputChannel.data = pixelOut[idx];
    	outputChannel.keep = RGBInputChannel.keep;
    	outputChannel.strb = RGBInputChannel.strb;
    	outputChannel.user = RGBInputChannel.user;
    	outputChannel.last = 0;
    	outputChannel.id = RGBInputChannel.id;
    	outputChannel.dest = RGBInputChannel.dest;
    	outStream.write(outputChannel); //Reference: Vivado Design Suite User Guide
    }

    outputChannel.data = pixelOut[N-1];
   	outputChannel.keep = RGBInputChannel.keep;
   	outputChannel.strb = RGBInputChannel.strb;
   	outputChannel.user = RGBInputChannel.user;
   	outputChannel.last = 1;
   	outputChannel.id = RGBInputChannel.id;
   	outputChannel.dest = RGBInputChannel.dest;
   	outStream.write(outputChannel);
}

void convert_to_gray(rgb gray_in[N], uint8_t gray_out[N])
{
    int i;
    for(i=0; i<N; i++)
    {
      gray_out[i] = 0.2989 * gray_in[i].red + 0.5870 * gray_in[i].green + 0.1140 * gray_in[i].blue;
    }
   //Debug - grayscale pixels for verification in MATLAB
/*
    gray.open("grayscale.txt");
    gray<<"x = [\n";   
    for(i = 0; i < IMG_WIDTH; i++)
    {
       for(int j=0;j<(IMG_HEIGHT - 1); j++)
       {
    	  gray<<(unsigned int)gray_out[j*IMG_WIDTH+i]<<" ";
       }
       gray<<(unsigned int)gray_out[127*IMG_WIDTH+i]<<";\n";
    }
    gray<<"];\n";
    gray.close();   
*/
}

void find_edges(uint8_t edge_in[N], uint8_t edge_out[N])
{
    uint8_t temp_image[N];
    convolution(edge_in,x_op,temp_image);
    convolution (temp_image,y_op,edge_out);
    //Debug - Edges for verification in MATLAB
/*
    edge.open("edge_values.txt");
    edge<<"x = [";

    for(int i = 0; i < IMG_WIDTH; i++)
    {
       	for(int j=0;j<(IMG_HEIGHT - 1); j++)
       	{
       	   edge<<(unsigned int)edge_out[j*IMG_WIDTH+i]<<" ";
       	}
       	edge<<(unsigned int)edge_out[127*IMG_WIDTH+i]<<";\n";
    }
    edge<<"];\n";
    edge.close();
*/
}

void convolution(uint8_t conv_in[N],const char kernel[K_ROW*K_COL],uint8_t conv_out[N])
{
    int i, j, m, n, mm, nn;
    int kCenterX, kCenterY;   // Kernel centre index
    float sum;                // Temporary Accumulation buffer
    int rowIndex, colIndex;

    // Find the kernel centre (half of kernel size)
    kCenterX = K_ROW>>1;
    kCenterY = K_COL>>1;

    convolution_label1:for(i=0; i < IMG_WIDTH; ++i)
    {
       convolution_label2:for(j=0; j < IMG_HEIGHT; ++j)
       {
         sum = 0;                       
         convolution_label3:for(m=0; m < K_ROW; ++m)
         {
           // Row Index of flipped kernel
           mm = K_ROW - 1 - m;
           convolution_label4:for(n=0; n < K_COL; ++n)
           {
             // Column index of flipped kernel
             nn = K_COL - 1 - n;
             // Index of input signal, to check boundary
             rowIndex = i + m - kCenterY;
             colIndex = j + n - kCenterX;
             // Ignore input samples which are out of bounds
             if(rowIndex >= 0 && rowIndex < IMG_WIDTH && colIndex >= 0 && colIndex < IMG_HEIGHT)
                 sum += (conv_in[IMG_HEIGHT * rowIndex + colIndex]) * (kernel[K_COL * mm + nn]);
           }
        }
        conv_out[IMG_HEIGHT * i + j] = hls::abs(sum);
     }
  }
}
