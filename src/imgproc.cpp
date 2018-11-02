//
// imgproc.cpp
// 2018-10-31
// S.Nakamura
//


#include <string>
#include "imgproc.h"


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


// 角度{0,45,90,135}の画像からストークスパラメータによる画像{S0, S1, S2}を取得する
void GetStokesImages(cv::Mat *aImgs, cv::Mat* sImgs)
{
    // 元の画像データがある
    assert(!aImgs[0].empty() && !aImgs[1].empty() && !aImgs[2].empty() && !aImgs[3].empty());

    // 元の画像のサイズが全て同じ
    int h = aImgs[0].rows, w = aImgs[0].cols;
    assert(aImgs[1].rows == h && aImgs[2].rows == h && aImgs[3].rows == h &&
        aImgs[1].cols == w && aImgs[2].cols == w && aImgs[3].cols == w);

    // ストークスパラメータ画像の準備
    for (int i = 0; i < 3; i++)
        sImgs[i] = cv::Mat(h, w, CV_8U);

    // ストークスパラメータの計算
    uint8_t* aData[] = { aImgs[0].data, aImgs[1].data, aImgs[2].data, aImgs[3].data };
    uint8_t* sData[] = { sImgs[0].data, sImgs[1].data, sImgs[2].data };
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int pos = y * w + x;
            sData[0][pos] = (aData[0][pos] + aData[2][pos]) / 2;       //  I0 + I90
            sData[1][pos] = (aData[0][pos] - aData[2][pos] + 256) / 2; //  I0 - I90
            sData[2][pos] = (aData[1][pos] - aData[3][pos] + 256) / 2; // I45 - I135
        }
    }
}

// 角度{0,45,90,135}の画像からストークスパラメータの絶対値を表す画像{S0, S1, S2}を取得する
void GetAbsStokesImages(cv::Mat *aImgs, cv::Mat* sImgs)
{
    // 元の画像データがある
    assert(!aImgs[0].empty() && !aImgs[1].empty() && !aImgs[2].empty() && !aImgs[3].empty());

    // 元の画像のサイズが全て同じ
    int h = aImgs[0].rows, w = aImgs[0].cols;
    assert(aImgs[1].rows == h && aImgs[2].rows == h && aImgs[3].rows == h &&
        aImgs[1].cols == w && aImgs[2].cols == w && aImgs[3].cols == w);

    // ストークスパラメータ画像の準備
    for (int i = 0; i < 3; i++)
        sImgs[i] = cv::Mat(h, w, CV_8U);

    // ストークスパラメータの計算
    uint8_t* aData[] = { aImgs[0].data, aImgs[1].data, aImgs[2].data, aImgs[3].data };
    uint8_t* sData[] = { sImgs[0].data, sImgs[1].data, sImgs[2].data };
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int pos = y * w + x;
            sData[0][pos] = (aData[0][pos] + aData[2][pos]) / 2;//  I0 + I90
            sData[1][pos] = abs(aData[0][pos] - aData[2][pos]); //  I0 - I90
            sData[2][pos] = abs(aData[1][pos] - aData[3][pos]); // I45 - I135
        }
    }
}

// ４つの画像から平均画像を作成する
void GetAverageImage(cv::Mat* src, cv::Mat& dst)
{
    // 元になる4つのデータがある
    for (int i = 0; i < 4; i++)
        assert(!src[i].empty());

    // すべて8U
    for (int i = 0; i<4; i++)
        assert(src[i].type() == CV_8U);

    // すべてのサイズが同じ
    int h = src[0].rows, w = src[0].cols;
    assert(src[1].rows == h && src[2].rows == h && src[3].rows == h);
    assert(src[1].cols == w && src[2].cols == w && src[3].cols == w);

    // 出力画像作成
    dst = cv::Mat(h, w, CV_8U);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int pos = y * w + x;
            int sum = src[0].data[pos] + src[1].data[pos] + src[2].data[pos] + src[3].data[pos];
            dst.data[pos] = (uint8_t)(sum / 4);
        }
    }
}


// 4つの変更画像から最大・最小値の画像を取得する
void GetMinMaxImages(cv::Mat* src, cv::Mat& min, cv::Mat& max)
{
    // 元の画像データがある
    assert(!src[0].empty() && !src[1].empty() && !src[2].empty() && !src[3].empty());

    // 元の画像のサイズが全て同じ
    int h = src[0].rows, w = src[0].cols;
    assert(src[1].rows == h && src[2].rows == h && src[3].rows == h &&
        src[1].cols == w && src[2].cols == w && src[3].cols == w);

    // 出力先データ準備
    min = cv::Mat(h, w, CV_8U);
    max = cv::Mat(h, w, CV_8U);

    // Min, Maxの計算
    uint8_t *pSrc[] = { src[0].data, src[1].data, src[2].data, src[3].data };
    uint8_t *pMin = min.data, *pMax = max.data;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int pos = y * w + x;
            uint8_t max, min;
            min = max = pSrc[0][pos];
            for (int i = 1; i < 4; i++) {
                if (min > pSrc[i][pos]) min = pSrc[i][pos];
                else if (max < pSrc[i][pos]) max = pSrc[i][pos];
            }
            pMin[pos] = min;
            pMax[pos] = max;
        }
    }
}

// 4方位の偏光画像から DoLP と AoLP とその合成画像を取得する
void GetXoLPImages(cv::Mat *pImgs, cv::Mat* dst, bool gamma_focus, double gamma, int th) {

    // 元の画像データがある
    assert(!pImgs[0].empty() && !pImgs[1].empty() && !pImgs[2].empty() && !pImgs[3].empty());

    // 元の画像のサイズが全て同じ
    int h = pImgs[0].rows, w = pImgs[0].cols;
    assert(pImgs[1].rows == h && pImgs[2].rows == h && pImgs[3].rows == h &&
        pImgs[1].cols == w && pImgs[2].cols == w && pImgs[3].cols == w);

    // 出力先データ準備
    dst[0] = cv::Mat(h, w, CV_8U);
    dst[1] = cv::Mat(h, w, CV_8UC3);
    dst[2] = cv::Mat(h, w, CV_8UC3);

    // DoLPの計算  [DoLP] = {(s1^2 + s2^2)^(1/2)} / s0;
    uint8_t* pData[] = { pImgs[0].data, pImgs[1].data, pImgs[2].data, pImgs[3].data };
    uint8_t* dData = dst[0].data;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int pos = y * w + x;
            double s0 = pData[0][pos] + pData[2][pos];
            double s1 = pData[0][pos] - pData[2][pos];
            double s2 = pData[1][pos] - pData[3][pos];
            double st = pData[1][pos] + pData[3][pos];
            double d = 255 * 2.0 * sqrt((s1 * s1 + s2 * s2)) / (s0 + st);

            // s0が小さいと DoLPが1を超えることがある。ノイズのせい？
            if (d > 255) d = 255;

            dData[pos] = (uint8_t)d;
        }
    }

    // ガンマ補正
    cv::Mat gammaMat;
    GammaCor(dst[0], gammaMat, gamma);
    dst[0] = gammaMat.clone();

    // 閾値処理
    if (!gamma_focus) {
        cv::Mat th_img;
        cv::threshold(gammaMat, th_img, th, 255.0, CV_THRESH_BINARY);
        cv::bitwise_and(gammaMat, th_img, dst[0]);
    }

    // AoLPの計算  [AoLP] = arctan(s2/s1) / 2
    cv::Mat AoLPImg(cv::Size(w, h), CV_8UC1);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int pos = y * w + x;
            double s1 = pData[0][pos] - pData[2][pos];
            double s2 = pData[1][pos] - pData[3][pos];

            double a = atan2(s2, s1) * 180 / CV_PI;
            if (a < 0) a += 360.0;
            a /= 2.0;
            uint8_t val = (uint8_t)a;
            if (val >= 180) val = 179; // <-- 0 <= AoLP <180

            AoLPImg.data[pos] = val;
        }
    }

    // AoLPをカラー画像にする
    cv::Mat whiteImg(cv::Size(w, h), CV_8U, cv::Scalar(255));
    cv::Mat hsvImg(cv::Size(w, h), CV_8UC3);
    cv::Mat hsvImgs[] = { AoLPImg, whiteImg, whiteImg };
    cv::merge(hsvImgs, 3, hsvImg);
    cv::cvtColor(hsvImg, dst[1], CV_HSV2BGR);

    // {DoLP + AoLP}画像を作成
    cv::Mat hsvImgs2[] = { AoLPImg, whiteImg, dst[0] };
    cv::Mat hsvImg2(cv::Size(w, h), CV_8UC3);
    cv::merge(hsvImgs2, 3, hsvImg2);
    cv::cvtColor(hsvImg2, dst[2], CV_HSV2BGR);
}

// 4つまでの画像を4区画で統合, すべての画像サイズは同じであること
void CombineImagesAt4(cv::Mat* imgs, cv::Mat& dst, const int n)
{
    // 高さと幅（すべて同じ）
    int h = imgs[0].rows, w = imgs[0].cols;
    for (int i = 1; i < n; i++) {
        assert(imgs[i].rows == h);
        assert(imgs[i].cols == w);
    }

    // カラー画像が含まれているかチェック
    bool isColor = false;
    for (int i = 0; i < n; i++) {
        if (imgs[i].type() == CV_8UC3) {
            isColor = true;
            break;
        }
    }

    // キャンバス作成
    int type = isColor ? CV_8UC3 : CV_8U;
    dst = cv::Mat::zeros(2 * h, 2 * w, type);

    // 順番に貼り付け
    for (int i = 0; i < n; i++)
    {
        // 貼り付け位置
        int x = (i % 2)*w;
        int y = (i / 2)*h;
        cv::Mat roi = dst(cv::Rect(x, y, w, h));

        // タイプが同じであればそのままコピー
        if (roi.type() == imgs[i].type()) {
            imgs[i].copyTo(roi);
        }
        else {
            // タイプをあわせる
            if (imgs[i].type() == CV_8U && roi.type() == CV_8UC3) {
                cv::Mat mgd[3] = { imgs[i], imgs[i], imgs[i] };
                cv::Mat img_3c;
                cv::merge(mgd, 3, img_3c);
                img_3c.copyTo(roi);
            }
            else {
                // このパターンは対応しない
            }
        }
    }
}

// 表示用アベレージ画像を作成する
void MakeAverageImage(cv::Mat& src, cv::Size size, cv::Mat& dst, bool caption)
{
    cv::resize(src, dst, size);
    if (caption)
        Labeling(dst, "Average");
}

// 表示用の偏光画像を作成する
void MakePolarizationImage(cv::Mat* src, cv::Size size, cv::Mat& dst, bool caption)
{
    using namespace cv;

    // 幅と高さ
    int w = size.width;
    int w1 = w / 2;
    int w2 = w - w1;
    int h = size.height;
    int h1 = h / 2;
    int h2 = h - h1;

    // 偏光画像
    Mat pImg[4];
    resize(src[0], pImg[0], Size(w1, h1));
    resize(src[1], pImg[1], Size(w2, h1));
    resize(src[2], pImg[2], Size(w1, h2));
    resize(src[3], pImg[3], Size(w2, h2));
    if (caption) {
        Labeling(pImg[0], "0 deg");
        Labeling(pImg[1], "45 deg");
        Labeling(pImg[2], "90 deg");
        Labeling(pImg[3], "135 deg");
    }

    // 最終画像
    dst = Mat::zeros(size, CV_8U);
    Mat roi;
    roi = dst(Rect(0, 0, w1, h1));       pImg[0].copyTo(roi);
    roi = dst(Rect(0 + w1, 0, w2, h1));  pImg[1].copyTo(roi);
    roi = dst(Rect(0, h1, w1, h2));      pImg[2].copyTo(roi);
    roi = dst(Rect(w1, h1, w2, h2)); pImg[3].copyTo(roi);

}

// 表示用の絶対値ストークス画像を作成する
void MakeAbsStokesImage(cv::Mat* src, cv::Size size, cv::Mat& dst, bool caption)
{
    using namespace cv;

    // 幅と高さ
    int w = size.width;
    int w1 = w / 2;
    int w2 = w - w1;
    int h = size.height;
    int h1 = h / 2;
    int h2 = h - h1;

    // ストークス画像作成
    Mat sImg[3];
    resize(src[0], sImg[0], Size(w1, h1));
    resize(src[1], sImg[1], Size(w2, h1));
    resize(src[2], sImg[2], Size(w1, h2));
    if (caption) {
        Labeling(sImg[0], "S0 = I0+I90");
        Labeling(sImg[1], "|S1| = |I0-I90|");
        Labeling(sImg[2], "|S2| = |I45-I135|");
    }

    // 最終画像
    dst = Mat::zeros(size, CV_8U);
    Mat roi;
    roi = dst(Rect(0, 0, w1, h1));      sImg[0].copyTo(roi);
    roi = dst(Rect(0 + w1, 0, w2, h1)); sImg[1].copyTo(roi);
    roi = dst(Rect(0, h1, w1, h2));     sImg[2].copyTo(roi);
}


// 表示用のXoPL画像を作成する
void MakeXoPLImage(cv::Mat* src, cv::Size size, cv::Mat& dst, bool focus_gamma, double gamma, int th, bool caption)
{
    using namespace cv;

    // 幅と高さ
    int w = size.width;
    int w1 = w / 2;
    int w2 = w - w1;
    int h = size.height;
    int h1 = h / 2;
    int h2 = h - h1;

    // XoLP画像作成
    Mat xImg[3];
    resize(src[0], xImg[0], Size(w1, h1));
    MakeInto3Ch(xImg[0], xImg[0]);
    resize(src[1], xImg[1], Size(w2, h1));
    resize(src[2], xImg[2], Size(w1, h2));
    if (caption) {
        Labeling(xImg[0], "DoLP");
        Labeling(xImg[1], "AoLP");
        Labeling(xImg[2], "DoLP + AoLP");
    }

    // 最終画像
    dst = Mat::zeros(size, CV_8UC3);
    Mat roi;
    roi = dst(Rect(0, 0, w1, h1));      xImg[0].copyTo(roi);
    roi = dst(Rect(0 + w1, 0, w2, h1)); xImg[1].copyTo(roi);
    roi = dst(Rect(0, h1, w1, h2));     xImg[2].copyTo(roi);

    // カラーバー
    Mat colorBar;
    GetColorBar(colorBar, Size(w2, h2 / 2));
    roi = dst(Rect(w1, h1, w2, h2 / 2));
    colorBar.copyTo(roi);

    // Gammaバー
    Mat gammaBar;
    GetGammaBar(gammaBar, Size(w2, h2 / 4), gamma, 0.5, 2.0, focus_gamma);
    roi = dst(Rect(w1, h1 + h2 / 2, w2, h2 / 4));
    gammaBar.copyTo(roi);

    // Thresholdバー
    Mat threshBar;
    Rect thRect = Rect(w1, h1 + colorBar.rows + gammaBar.rows, w2, h2 - colorBar.rows - gammaBar.rows);
    GetThresholdBar(threshBar, thRect.size(), th, !focus_gamma);
    roi = dst(thRect);
    threshBar.copyTo(roi);
}




// 全画像を統合した画像を作成する
void MakeAllImage(cv::Mat& ave, cv::Mat* pol, cv::Mat* stk, cv::Mat* XoLP, cv::Size size, cv::Mat& dst, bool focus_gamma, double gamma, int th, bool drawline, bool caption)
{
    using namespace cv;

    // 幅と高さ
    int w = size.width;
    int w1 = w / 2;
    int w2 = w - w1;
    int h = size.height;
    int h1 = h / 2;
    int h2 = h - h1;

    // キャンバス準備
    dst = Mat::zeros(size, CV_8UC3);
    Mat roi;

    // 平均画像
    Mat aveImg;
    MakeAverageImage(ave, Size(w1, h1), aveImg, caption);
    MakeInto3Ch(aveImg, aveImg);
    roi = dst(Rect(0, 0, w1, h1));
    aveImg.copyTo(roi);

    // 偏光画像
    Mat polImg;
    MakePolarizationImage(pol, Size(w2, h1), polImg, caption);
    MakeInto3Ch(polImg, polImg);
    roi = dst(Rect(w1, 0, w2, h1));
    polImg.copyTo(roi);

    // 絶対値ストークス画像
    Mat stkImg;
    MakeAbsStokesImage(stk, Size(w1, h2), stkImg, caption);
    MakeInto3Ch(stkImg, stkImg);
    roi = dst(Rect(0, h1, w1, h2));
    stkImg.copyTo(roi);

    // XoLP画像
    Mat xoLPImg;
    MakeXoPLImage(XoLP, Size(w2, h2), xoLPImg, focus_gamma, gamma, th, caption);
    roi = dst(Rect(w1, h1, w2, h2));
    xoLPImg.copyTo(roi);

    // 分割線描画
    if (drawline) {

        const Scalar color = Scalar(255, 255, 255); // 色
        line(dst, Point(w1, 0), Point(w1, h), color, 3); // 縦線
        line(dst, Point(0, h1), Point(w, h1), color, 3); // 横線
    }

}

// 画像左上にラベルをつける
void Labeling(cv::Mat& img, cv::String str, cv::Point pos) {

    // 文字領域のサイズ取得
    const int fontface = cv::FONT_HERSHEY_TRIPLEX;
    const double fontscale = 0.4;
    const int thickness = 1;
    const int linetype = CV_AA;
    int baseline = 0;
    const cv::Size textsize = cv::getTextSize(str, fontface, fontscale, thickness, &baseline);

    // ラベル準備
    const int mgn = 5;
    const int w = textsize.width + 2 * mgn;
    const int h = textsize.height + 2 * mgn + baseline;
    cv::Size size(w, h);
    cv::Mat label = cv::Mat::zeros(size, img.type());

    // ラベルに文字描画
    cv::Point textPos((size.width - textsize.width) / 2, (size.height + textsize.height - baseline) / 2);
    cv::putText(label, str, textPos, fontface, fontscale, cvScalar(255, 255, 255), thickness, linetype);

    // ラベルを画像に貼りつkる
    cv::Mat roi = img(cv::Rect(pos.x, pos.y, w, h));
    label.copyTo(roi);
}


// 数値キャプション付きのカラーバーを作成する
void GetColorBar(cv::Mat& dst, cv::Size size)
{
    // 出力画像準備
    dst = cv::Mat::zeros(size, CV_8UC3);

    // カラーバー作成
    const int mgn = 20;
    int tmpW = size.width - 2 * mgn;;
    int tmpH = size.height - 2 * mgn;
    if (tmpW <= 10 || tmpH <= 10) return; // あまりにも小さい場合はカラーバーなし
    int outR = tmpH < tmpW / 2 ? tmpH : tmpW / 2;
    int inR = (int)(0.7*outR);
    cv::Mat rawCB;
    MakeRawColorBar(rawCB, outR, inR);

    // カラーバー描画
    int tmpX = (size.width - rawCB.cols) / 2;
    int tmpY = (size.height - rawCB.rows) / 2;
    cv::Rect rectCB(tmpX, tmpY, rawCB.cols, rawCB.rows);
    cv::Mat roiCB = dst(rectCB);
    rawCB.copyTo(roiCB);

    // 数値キャプション描画
    const cv::String str[] = { "0 deg", "90 deg", "180 deg" };
    const int fontface = cv::FONT_HERSHEY_TRIPLEX;
    const double fontscale = 0.4;
    const int thickness = 1;
    const cv::Scalar color(255, 255, 255);
    const int linetype = CV_AA;
    cv::Size strSize[3];
    int baseline;
    for (int i = 0; i < 3; i++)
        strSize[i] = cv::getTextSize(str[i], fontface, fontscale, thickness, &baseline);
    cv::Point strLoc[3];
    strLoc[0] = cv::Point(rectCB.x + (outR - inR - strSize[0].width) / 2, rectCB.y + rectCB.height + strSize[0].height + 5);  // 0deg
    strLoc[1] = cv::Point(rectCB.x + outR - strSize[1].width / 2, rectCB.y - 5);
    strLoc[2] = cv::Point(rectCB.x + (3 * outR + inR - strSize[2].width) / 2, rectCB.y + rectCB.height + strSize[2].height + 5); // 180deg
    for (int i = 0; i<3; i++)
        cv::putText(dst, str[i], strLoc[i], fontface, fontscale, color, thickness, linetype);

}

// カラーバーを作成する。バー部分の画像のみ
void MakeRawColorBar(cv::Mat& dst, int outR, int inR) {

    // 外側の半径の方が大
    assert(outR > inR);


    // 画像準備
    cv::Mat hImg = cv::Mat::zeros(cv::Size(2 * outR - 1, outR), CV_8U);
    cv::Mat vImg = cv::Mat::zeros(cv::Size(2 * outR - 1, outR), CV_8U);

    // 半径自乗
    int ro2 = outR * outR;
    int ri2 = inR * inR;

    // h,v画像作成
    uint8_t *pH = hImg.data, *pV = vImg.data;
    int h = hImg.rows, w = hImg.cols;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int dx = x - outR + 1, dy = outR - 1 - y;
            int r2 = dx * dx + dy * dy;

            // 描画すべき領域
            if (ri2 <= r2 && r2 <= ro2) {

                // 角度計算
                double angle = atan2(dy, dx) * 180 / CV_PI;
                if (angle < 0) angle += 360;

                // 値代入
                int pos = y * w + x;
                pH[pos] = (uint8_t)(angle);
                pV[pos] = 255;

            }
        }
    }

    // hsvからRGBへ変換
    cv::Mat hsv[] = { hImg, vImg, vImg }, hsv3ch;
    merge(hsv, 3, hsv3ch);
    cv::cvtColor(hsv3ch, dst, CV_HSV2BGR);

}

// ガンマ値を示すバーを作成
void GetGammaBar(cv::Mat& dst, cv::Size size, double val, double min, double max, bool active) {

    // 出力画像準備
    dst = cv::Mat::zeros(size, CV_8UC3);

    // activeは黄色
    cv::Scalar color = active ? cv::Scalar(0, 255, 255) : cv::Scalar(255, 255, 255);

    // カラーバー領域
    int mgnW = int(size.width*0.1);
    int barW = size.width - 2 * mgnW;
    int barH = barW / 12;
    int mgnH = (size.height - barH) / 2;
    if (barW <= 10 || barH <= 10) return; // あまりにも小さい場合はなし

                                          // カラーバー作成 & 描画
    cv::Mat bar;
    MakeGrayGradation(bar, cv::Size(barW, barH));
    cv::Rect barRect(mgnW, mgnH, barW, barH);
    cv::Mat roi = dst(barRect);
    bar.copyTo(roi);
    if (active)
        cv::rectangle(dst, barRect, color, 2);
    else
        cv::rectangle(dst, barRect, cv::Scalar(128, 128, 128));

    // 針位置決定＆描画
    double ratio = (log2(val) - log2(min)) / (log2(max) - log2(min));
    int pos = (int)(mgnW + ratio * barW);
    cv::line(dst, cv::Point(pos, mgnH - 3), cv::Point(pos, mgnH + barH + 3), color, 3);

    // 数値キャプション描画
    std::ostringstream oss;
    oss << std::setprecision(2) << std::setiosflags(std::ios::fixed) << val;
    std::string valStr = oss.str();
    const int fontface = cv::FONT_HERSHEY_TRIPLEX;
    const double fontscale = 0.3;
    const int thickness = 1;
    const int linetype = CV_AA;
    cv::Size strSize;
    int baseline;
    strSize = cv::getTextSize(valStr, fontface, fontscale, thickness, &baseline);
    cv::Point strLoc;
    strLoc = cv::Point(pos - (strSize.width) / 2, mgnH + barH + strSize.height + 5);
    cv::putText(dst, valStr, strLoc, fontface, fontscale, color, thickness, linetype);

}

// ガンマ値を示すバーを作成
void GetThresholdBar(cv::Mat& dst, cv::Size size, int val, bool active) {

    // 出力画像準備
    dst = cv::Mat::zeros(size, CV_8UC3);

    // activeは黄色
    cv::Scalar color = active ? cv::Scalar(0, 255, 255) : cv::Scalar(255, 255, 255);

    // カラーバー領域
    int mgnW = int(size.width*0.1);
    int barW = size.width - 2 * mgnW;
    int barH = barW / 12;
    int mgnH = (size.height - barH) / 2;
    if (barW <= 10 || barH <= 10) return; // あまりにも小さい場合はなし

                                          // カラーバー作成 & 描画
    cv::Mat bar;
    MakeGrayGradation(bar, cv::Size(barW, barH));
    cv::Rect barRect(mgnW, mgnH, barW, barH);
    cv::Mat roi = dst(barRect);
    bar.copyTo(roi);
    if (active)
        cv::rectangle(dst, barRect, color, 2);
    else
        cv::rectangle(dst, barRect, cv::Scalar(128, 128, 128));

    // 針位置決定＆描画
    double ratio = val / 255.0;
    int pos = (int)(mgnW + ratio * barW);
    cv::line(dst, cv::Point(pos, mgnH - 3), cv::Point(pos, mgnH + barH + 3), color, 3);

    // 数値キャプション描画
    std::string valStr = std::to_string(val);
    const int fontface = cv::FONT_HERSHEY_TRIPLEX;
    const double fontscale = 0.3;
    const int thickness = 1;
    const int linetype = CV_AA;
    cv::Size strSize;
    int baseline;
    strSize = cv::getTextSize(valStr, fontface, fontscale, thickness, &baseline);
    cv::Point strLoc;
    strLoc = cv::Point(pos - (strSize.width) / 2, mgnH + barH + strSize.height + 5);
    cv::putText(dst, valStr, strLoc, fontface, fontscale, color, thickness, linetype);
}

// 3chの白黒のグラデーションバーを作成
void MakeGrayGradation(cv::Mat& dst, cv::Size size)
{
    dst = cv::Mat(size, CV_8UC3);
    int h = size.height;
    int w = size.width;

    uchar *p = dst.data;
    // 1行目だけ計算
    for (int i = 0; i < w; i++)
        p[3 * i] = p[3 * i + 1] = p[3 * i + 2] = (uchar)(256 * i / w);

    // 2行目以降はコピー
    for (int i = 1; i < h; i++)
        memcpy(p + i * dst.step, p, dst.step);

}

// 1ch画像を3Ch化する( InPlace OK)
void MakeInto3Ch(cv::Mat& src, cv::Mat& dst)
{
    assert(src.type() == CV_8U || src.type() == CV_8UC3);

    // すでに3ch画像の場合
    if (src.type() == CV_8UC3) {

        // srcとdstが同じインスタンスの可能性があるので
        // 段階を分けてコピー
        cv::Mat clone = src.clone();
        dst = clone;
    }
    // 1Chの場合
    else if (src.type() == CV_8U)
    {
        cv::Mat clone;
        cv::Mat tmp[] = { src,src,src };
        cv::merge(tmp, 3, clone);
        dst = clone;
    }
}

// ガンマ補正をする
void GammaCor(cv::Mat &src, cv::Mat& dst, double gamma)
{
    assert(src.type() == CV_8U);

    static double old_gamma = 123.4567;
    static cv::Mat lut = cv::Mat(1, 256, CV_8U);

    // ガンマ値が変わったら、ルックアップテーブル計算しなおし
    if (old_gamma != gamma) {
        uchar* p = lut.data;
        for (int i = 0; i<256; i++)
            p[i] = (int)(pow((double)i / 255.0, 1.0 / gamma) * 255.0);
    }
    // 適用
    cv::LUT(src, lut, dst);
}
