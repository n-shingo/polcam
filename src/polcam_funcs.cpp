//
// polcam_funcs.cpp
// 2018-10-31
// S.Nakamura
//


#include "polcam_funcs.h"


// IImageからOpenCVのMatを取得する
void GetMatFromIImage(Arena::IImage *src, cv::Mat& dst)
{
    // 画像データがあるかチェック
    assert(src->HasImageData());

    // 高さと幅
    const int w = (int)src->GetWidth();
    const int h = (int)src->GetHeight();

    // Mat画像作成
    dst = cv::Mat(h, w, CV_8U);
    size_t bufSize = src->GetSizeOfBuffer();
    const uint8_t* pSrc = src->GetData();
    memcpy(dst.data, pSrc, bufSize);
}


// 4つの角度{0,45,90,135}の画像を取得する
void GetPolarizedImages(cv::Mat& img, cv::Mat* imgs)
{
    // 画像データがある
    assert(!img.empty());

    // サイズチェック
    int h = img.rows;
    int w = img.cols;
    assert(h % 2 == 0 && w % 2 == 0);

    // 分解能( いまのところ8bitのみ対応 )
    assert(img.type() == CV_8U);

    // 分解先のMat作成
    int h2 = h / 2, w2 = w / 2;
    for (int i = 0; i < 4; i++)
        imgs[i] = cv::Mat(h2, w2, CV_8U);

    // ４つに分解する
    const uint8_t* pData = img.data;
    for (int y = 0; y < h; y += 2) {
        for (int x = 0; x < w; x += 2) {
            int pos = (y / 2)*(w2)+x / 2;
            imgs[2].data[pos] = pData[y*w + x];           // 90deg
            imgs[1].data[pos] = pData[y*w + x + 1];       // 45deg
            imgs[3].data[pos] = pData[(y + 1)*w + x];     // 135deg
            imgs[0].data[pos] = pData[(y + 1)*w + x + 1]; // 0deg
        }
    }
}
