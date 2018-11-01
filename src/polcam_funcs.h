//
// polcam_funcs.h
// 2018-10-31
// S.Nakamura
//


#ifndef __POLCAM_FUNCS_H__
#define __POLCAM_FUNCS_H__

#include "opencv2/opencv.hpp"
#include "ArenaApi.h"


// IImageからOpenCVのMatを取得する
void GetMatFromIImage(Arena::IImage *src, cv::Mat& dst);

// 4つの角度{0,45,90,135}の画像を取得する
void GetPolarizedImages(cv::Mat& img, cv::Mat* imgs);

#endif //__POLCAM_FUNCS_H__
