//
// polcam_funcs.cpp
// 2018-10-31
// S.Nakamura
//


#include "polcam_funcs.h"


// IImage����OpenCV��Mat���擾����
void GetMatFromIImage(Arena::IImage *src, cv::Mat& dst)
{
    // �摜�f�[�^�����邩�`�F�b�N
    assert(src->HasImageData());

    // �����ƕ�
    const int w = (int)src->GetWidth();
    const int h = (int)src->GetHeight();

    // Mat�摜�쐬
    dst = cv::Mat(h, w, CV_8U);
    size_t bufSize = src->GetSizeOfBuffer();
    const uint8_t* pSrc = src->GetData();
    memcpy(dst.data, pSrc, bufSize);
}


// 4�̊p�x{0,45,90,135}�̉摜���擾����
void GetPolarizedImages(cv::Mat& img, cv::Mat* imgs)
{
    // �摜�f�[�^������
    assert(!img.empty());

    // �T�C�Y�`�F�b�N
    int h = img.rows;
    int w = img.cols;
    assert(h % 2 == 0 && w % 2 == 0);

    // ����\( ���܂̂Ƃ���8bit�̂ݑΉ� )
    assert(img.type() == CV_8U);

    // ������Mat�쐬
    int h2 = h / 2, w2 = w / 2;
    for (int i = 0; i < 4; i++)
        imgs[i] = cv::Mat(h2, w2, CV_8U);

    // �S�ɕ�������
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