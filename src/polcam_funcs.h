//
// polcam_funcs.h
// 2018-10-31
// S.Nakamura
//


#ifndef __POLCAM_FUNCS_H__
#define __POLCAM_FUNCS_H__

#include "opencv2/opencv.hpp"
#include "ArenaApi.h"


// IImage‚©‚çOpenCV‚ÌMat‚ðŽæ“¾‚·‚é
void GetMatFromIImage(Arena::IImage *src, cv::Mat& dst);

// 4‚Â‚ÌŠp“x{0,45,90,135}‚Ì‰æ‘œ‚ðŽæ“¾‚·‚é
void GetPolarizedImages(cv::Mat& img, cv::Mat* imgs);

#endif //__POLCAM_FUNCS_H__
