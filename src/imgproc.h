//
// polcam_funcs.h -> imgproc.h
// 2018-10-31
// S.Nakamura
//


#ifndef __IMGPROC_H__
#define __IMPPROC_H__

#include "opencv2/opencv.hpp"
#include "ArenaApi.h"


// IImageからOpenCVのMatを取得する
void GetMatFromIImage(Arena::IImage *src, cv::Mat& dst);

// 4つの角度{0,45,90,135}の画像を取得する
void GetPolarizedImages(cv::Mat& img, cv::Mat* imgs);

// 4つの偏光画像から平均画像を取得する
void GetAverageImage(cv::Mat* src, cv::Mat& dst);

// 4つの変更画像から最大・最小値の画像を取得する
void GetMinMaxImages(cv::Mat* src, cv::Mat& min, cv::Mat& max);

// 角度{0,45,90,135}の画像からストークスパラメータによる画像{S0, S1, S2}を取得する
void GetStokesImages(cv::Mat *aImgs, cv::Mat* sImgs);

// 角度{0,45,90,135}の画像から絶対値ストークスパラメータ画像{S0, S1, S2}を取得する
void GetAbsStokesImages(cv::Mat *aImgs, cv::Mat* sImgs);

// 4方位の偏光画像から DoLP, AoLP, {DoLP+AoLP} 画像を取得する
void GetXoLPImages(cv::Mat *pImgs, cv::Mat* dst, bool focus_gamma, double gamma, int th);

// 4つまでの画像を4区画で統合, すべての画像サイズは同じであること
void CombineImagesAt4(cv::Mat* imgs, cv::Mat& dst, const int count = 4);

// 表示用アベレージ画像を作成する
void MakeAverageImage(cv::Mat& src, cv::Size size, cv::Mat& dst, bool caption = true);

// 表示用の偏光画像を作成する
void MakePolarizationImage(cv::Mat* src, cv::Size size, cv::Mat& dst, bool caption = true);

// 表示用の絶対値ストークス画像を作成する
void MakeAbsStokesImage(cv::Mat* src, cv::Size size, cv::Mat& dst, bool caption = true);

// 表示用のXoPL画像を作成する
void MakeXoPLImage(cv::Mat* src, cv::Size size, cv::Mat& dst, bool focus_gamma, double gamma, int th, bool caption = true);

// 全画像を統合した表示画像を作成する
void MakeAllImage(cv::Mat& ave, cv::Mat* pol, cv::Mat* stk, cv::Mat* XoLP, cv::Size size, cv::Mat& dst, bool focus_gamma, double gamma, int th, bool drawline = true, bool caption = true);

// 画像にラベルをつける
void Labeling(cv::Mat& img, cv::String str, cv::Point pos = cv::Point(0, 0));

// 数値付きのカラーバーを作成する
void GetColorBar(cv::Mat& dst, cv::Size size);

// カラーバーを作成する。バー部分の画像のみ
void MakeRawColorBar(cv::Mat& dst, int outR, int inR);

// ガンマ値を示すバーを作成
void GetGammaBar(cv::Mat& dst, cv::Size size, double val, double min, double max, bool active);

// ガンマ値を示すバーを作成
void GetThresholdBar(cv::Mat& dst, cv::Size size, int val, bool active);

// 3chの白黒のグラデーションバーを作成(だけど3ch)
void MakeGrayGradation(cv::Mat& dst, cv::Size size);

// 1ch画像を3Ch化する( InPlace OK)
void MakeInto3Ch(cv::Mat &src, cv::Mat& dst);

// ガンマ補正をする
void GammaCor(cv::Mat &src, cv::Mat& dst, double gamma);


#endif //__IMGPROC_H__
