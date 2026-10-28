#ifndef FUSIONALGORITHM_H
#define FUSIONALGORITHM_H

#include <QObject>

#define WITHOUT 0
#define ALL_IMG 1

class FusionAlgorithm
{   
public:
    FusionAlgorithm();

    void setXoffset(int x_offset) {tv_offset_x = x_offset;}
    void setYoffset(int y_offset) {tv_offset_y = y_offset;}
    void setKorrCoefC(double val) {kor_coef_c = val;}
    void setOutputFormat(int val) {output_format = val;}

    int getXoffset() {return tv_offset_x;}
    int getYoffset() {return tv_offset_y;}
    double getKorrCoefC() {return kor_coef_c;}
    int getOutputFormat() {return output_format;}

    void fusion(uint8_t *lwArray, uint8_t *tvArray, int Xsz, int Ysz);
private:
    int tv_offset_x;
    int tv_offset_y;
    int output_format;
    double kor_coef_c;

    double Min(double * pData, int sz);
    double Max(double * pData, int sz);
    void shiftTVImg(uint8_t *tvArray, uint8_t *tvShiftArray, int Xsz, int Ysz);
    void fusionLinRegres(uint8_t *lwArray, uint8_t *tvArray, int Xsz, int Ysz);
};

#endif // FUSIONALGORITHM_H
