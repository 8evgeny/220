#include "tmml.hpp"

using namespace std;
using namespace cv;

__constant__ unsigned char const_img_temp_array[TEMPLATE_AREA];

void tmml::cuda_Malloc()
{
    cudaMalloc((void**)& dev_img_work_arr, sizeof(unsigned char) * WORK_AREA);
    cudaMalloc((void**)& dev_max_pix, sizeof(Pix));
    cudaMalloc((void**)& dev_val, sizeof(int));
} // -- END cuda_Malloc()

void tmml::cuda_Free()
{
    cudaFree(&dev_img_work_arr);
    cudaFree(&dev_max_pix);
    cudaFree(&dev_val);
} // -- END cudaFree()

__global__ void match_temp(unsigned char * dev_img_work_arr, int * dev_val, Pix * dev_v_res_pix)
{    
    int sum_roi_temp = 0, sum_temp_temp = 0, sum_roi_roi = 0, sum_roi = 0, sum_temp = 0,
            temp_id, work_id, temp, roi, temp_x, temp_y,
            result_id = blockIdx.x * blockDim.x + threadIdx.x,
            result_y = result_id * RESULT_WIDTH_1,
            result_x = result_id - result_y * RESULT_WIDTH,
            work_id0 = result_y * WORK_WIDTH + result_x;
    for(temp_y = 0; temp_y < TEMPLATE_WIDTH; ++temp_y)
    {
        temp_id = temp_y * TEMPLATE_WIDTH;
        work_id = work_id0 + temp_y * WORK_WIDTH;
        for(temp_x = 0; temp_x < TEMPLATE_WIDTH; ++temp_x)
        {
            temp = const_img_temp_array[temp_id + temp_x];
            roi = dev_img_work_arr[work_id + temp_x];
            sum_roi_temp += roi * temp;
            sum_temp_temp += temp * temp;
            sum_roi_roi += roi * roi;
#ifdef COMBINED
            sum_roi += roi;
            sum_temp += temp;
#endif // END ifdef COMBINED
        } // for(int tmp_x = 0; tmp_x < TEMPLATE_WIDTH; ++tmp_x)
    } // for(int tmp_y = 0; tmp_y < TEMPLATE_WIDTH; ++tmp_y)
    float sum_roi_temp1 = TEMPLATE_AREA_1 * sum_roi_temp;
    float sum_roi_roi1 = TEMPLATE_AREA_1 * sum_roi_roi;
    float sum_temp_temp1 = TEMPLATE_AREA_1 * sum_temp_temp;
    float diff_roi_temp2 = sum_roi_roi1 + sum_temp_temp1 - 2.f * sum_roi_temp1;
#ifdef COMBINED
    float sum_roi1 = TEMPLATE_AREA_1 * sum_roi;
    float sum_temp1 = TEMPLATE_AREA_1 * sum_temp;
    float ch  = sum_roi_temp1 - sum_roi1 * sum_temp1;
    float zn1 = sum_temp_temp1 - sum_temp1 * sum_temp1;
    float zn2 = sum_roi_roi1 - sum_roi1 * sum_roi1;
    float result_float = ch / sqrt(zn1 * zn2) - KOEFF2LIB_float * diff_roi_temp2 / sqrt(sum_roi_roi1 * sum_temp_temp1);
#endif // END ifdef COMBINED
#ifdef SQDIFF_NORMED
    float result_float = 1.f - KOEFF2LIB_float * diff_roi_temp2 / sqrt(sum_roi_roi1 * sum_temp_temp1);
#endif // END ifdef SQDIFF_NORMED
    int val = 1000000 * result_float;
    atomicMax(dev_val, val);
    __syncthreads();
    if(*dev_val == val)
    {
        dev_v_res_pix->x = result_x;
        dev_v_res_pix->y = result_y;
        dev_v_res_pix->bright = result_float;
    }  // END if(*dev_val == val)
}  // END match_temp

void tmml::work_tmml(const Mat& img_work, const Mat& img_temp, Pix& max_pix)
{
    cudaMemcpy(dev_val, &val0, sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(dev_img_work_arr, img_work.data, sizeof(unsigned char) * WORK_AREA, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(const_img_temp_array, img_temp.data, sizeof(unsigned char) * TEMPLATE_AREA);
    match_temp<<<blocks_match_temp, threads_match_temp>>>(dev_img_work_arr, dev_val, dev_max_pix);
    cudaMemcpy(&max_pix, dev_max_pix, sizeof(Pix), cudaMemcpyDeviceToHost);
} // END work_tmml
