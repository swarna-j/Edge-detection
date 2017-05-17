#include <hls_math.h> //Required by hls::abs()
#include <hls_video.h> //Required for streaming data
#include <ap_axi_sdata.h> //Required for defining channel attributes

#define IMG_WIDTH 128
#define IMG_HEIGHT 128
#define K_ROW 3
#define K_COL 3
#define N IMG_WIDTH*IMG_HEIGHT
#define DEPTH 30

typedef ap_axiu<32,4,5,5> uint32_channel;  //32-data, 1- user, 1-ID, 1-dest (Reference: http://electronics.stackexchange.com/questions/175207/xilinx-ap-axiu-parameters)

typedef struct {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
} rgb;

const char x_op[K_ROW* K_COL] = {1, 0, -1, 2, 0, -2, 1, 0, -1};
const char y_op[K_ROW* K_COL] = { 1, 2, 1, 0, 0, 0, -1, -2, -1};

//Utility Functions
void convert_to_gray(rgb in_image[N], unsigned char out_image[N]);
void convolution(unsigned char conv_in[N],const char kernel[K_ROW*K_COL],unsigned char conv_out[N]);
void find_edges(unsigned char in_image[N],unsigned char out_image[N]);

//IP Core Function
void edge_detect(hls::stream<uint32_channel> &inStream, hls::stream<uint32_channel> &outStream);
// Note: Stream data is 32 bits wide - RGB (3*8 = 24; Only 8, 16 and 32 bits supported by DMA)
