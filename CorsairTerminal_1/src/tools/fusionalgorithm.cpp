#include "fusionalgorithm.h"
#include <math.h>

FusionAlgorithm::FusionAlgorithm()
{
    tv_offset_x = 0;
    tv_offset_y = 0;
    output_format = 1;
    kor_coef_c = 2.0;
}

void FusionAlgorithm::fusion(uint8_t *lwArray, uint8_t *tvArray, int Xsz, int Ysz)
{
    uint8_t* tvShiftArray = new uint8_t[Xsz * Ysz];
    shiftTVImg(tvArray, tvShiftArray, Xsz, Ysz);
    fusionLinRegres(lwArray, tvShiftArray, Xsz, Ysz);
    delete[] tvShiftArray;
}

void FusionAlgorithm::fusionLinRegres(uint8_t *lwArray, uint8_t *tvArray, int Xsz, int Ysz)
{
    double sum_tv = 0, sum_lw = 0;
    double sum_sq_tv = 0;//, sum_sq_lw = 0;
    double sum_mult_tv_lw = 0;
    int n;
    for(int j = 0; j < Ysz; j++)
        for(int i = 0; i < Xsz; i++)
        {
            n = j * Xsz + i;
            sum_tv += tvArray[n];
            sum_lw += lwArray[n];
            sum_sq_tv += (tvArray[n]*tvArray[n]);
            sum_mult_tv_lw += (tvArray[n]*lwArray[n]);
        }
    double a, b;
    a = (Xsz * Ysz * sum_mult_tv_lw) - (sum_tv * sum_lw);
    double tmp = Xsz * Ysz * sum_sq_tv - sum_lw * sum_lw;
    a = (tmp == 0.0) ? 0.0 : a / tmp;

    b = (sum_lw - a * sum_tv);
    b = b / (Xsz * Ysz);

    double* fusionArray = new double[Xsz * Ysz];
    double avrgFusionArray = 0;
    for(int j = 0; j < Ysz; j++)
        for(int i = 0; i < Xsz; i++)
        {
            n = j * Xsz + i;
            fusionArray[n] = a * double(tvArray[n]) + b;
            avrgFusionArray += fusionArray[n];
        }
    avrgFusionArray /= (Xsz * Ysz);

    double buf;
    double c = kor_coef_c;
    for(int j = 0; j < Ysz; j++)
        for(int i = 0; i < Xsz; i++)
        {
            n = j * Xsz + i;
            buf = double(tvArray[n]) + c * (double(lwArray[n]) - avrgFusionArray);
            fusionArray[n] = buf;
        }

    double buff, min_fusion, max_fusion;
    double coef;
    switch(output_format){
    case WITHOUT:
        for(int j = 0; j < Ysz; j++)
            for(int i = 0; i < Xsz; i++)
            {
                buff = fusionArray[j * Xsz + i];
                if(buff > 255)
                    lwArray[j * Xsz + i] = 255;
                else if(buff < 0)
                    lwArray[j * Xsz + i] = 0;
                else
                    lwArray[j * Xsz + i] = uchar(buff);
            }
        break;
    case ALL_IMG:
        min_fusion = Min(fusionArray, Ysz*Xsz);
        for(int j = 0; j < Ysz; j++)
            for(int i = 0; i < Xsz; i++)
                fusionArray[j * Xsz + i] -= min_fusion;
        max_fusion = Max(fusionArray, Ysz*Xsz);
        for(int j = 0; j < Ysz; j++)
            for(int i = 0; i < Xsz; i++)
            {
                buff = fusionArray[j * Xsz + i];
                coef = (max_fusion == 0.0) ? 255.0 : (255.0/double(max_fusion));
                buff = int(round(double(buff) * coef));
                lwArray[j * Xsz + i] = uchar(buff);
            }
        break;
    }

    delete[] fusionArray;
}

double FusionAlgorithm::Min(double *pData, int sz)
{
    double min = pData[0];
    for(int i = 0; i < sz; i++)
    {
        if(pData[i] < min)
            min = pData[i];
    }
    return min;
}

double FusionAlgorithm::Max(double *pData, int sz)
{
    double max = pData[0];
    for(int i = 0; i < sz; i++)
    {
        if(pData[i] > max)
            max = pData[i];
    }
    return max;
}

void FusionAlgorithm::shiftTVImg(uint8_t *tvArray, uint8_t *tvShiftArray, int Xsz, int Ysz)
{
    int tv_y, tv_x;
    int i, j;
    for(j = 0; j < Ysz; j++)
        for(i = 0; i < Xsz; i++)
        {
            tv_y = j + tv_offset_y;
            tv_x = i + tv_offset_x;
            uint8_t pixTV = 0;
            if ((tv_y >= 0) && (tv_y < Ysz) && (tv_x >= 0) && (tv_x < Xsz))
                pixTV = tvArray[tv_y*Xsz + tv_x];
            else
                pixTV = 0;
            tvShiftArray[j*Xsz + i] = pixTV;
        }
}
