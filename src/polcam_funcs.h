//
// polcam_funcs.h
// 2018-10-31
// S.Nakamura
//


#ifndef __POLCAM_FUNCS_H__
#define __POLCAM_FUNCS_H__

#include "opencv2/opencv.hpp"
#include "ArenaApi.h"


// IImage����OpenCV��Mat���擾����
void GetMatFromIImage(Arena::IImage *src, cv::Mat& dst);

// 4�̊p�x{0,45,90,135}�̉摜���擾����
void GetPolarizedImages(cv::Mat& img, cv::Mat* imgs);

#endif //__POLCAM_FUNCS_H__
