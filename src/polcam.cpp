//
// polcam.cpp
// 2018-10-31
// S.Nakamura
//

#include "opencv2/opencv.hpp"
#include "ArenaApi.h"
#include "imgproc.h"
#include "polcam.h"

#define TIMEOUT 2000
#define RESIZE_RATIO 0.5
#define USB_CAM_ID 0

#define RESULT_W 1200
#define RESULT_H 900

int main( int argc, char *argv[] )
{
    // Parameters
    bool showUsbCam = true;
    ShowMode mode = ShowMode::All;
    double gamma = 1.0;
    const double g_dif = pow(2.0, 1.0 / 16.0);
    int th = 128;
    const int t_dif = 4;
    bool focus_gamma = true;


    // USB camera
    cv::VideoCapture capture;
    std::cout << "USB Camera is opening ... ";
    if (!capture.open(USB_CAM_ID))
        std::cout << "failed!\n" << std::endl;
    else {
        std::cout << "OK!" << std::endl;
        int w = (int)capture.get(CV_CAP_PROP_FRAME_WIDTH);
        int h = (int)capture.get(CV_CAP_PROP_FRAME_HEIGHT);
        std::cout << "Original USB Image Size: " << w << " x " << h << std::endl << std::endl;
    }

    // Polarization Camera Preparation
    std::cout << "Try Start Polarization Camera Acquisition\n";
    Arena::ISystem* pSystem = Arena::OpenSystem();
    pSystem->UpdateDevices(100);
    std::vector<Arena::DeviceInfo> deviceInfos = pSystem->GetDevices();
    if (deviceInfos.size() == 0)
    {
        std::cout << "No camera connected" << std::endl;
        return 1;
    }
    Arena::IDevice* pDevice = pSystem->CreateDevice(deviceInfos[0]);

    // get node values that will be changed in order to return their values
    // at the end
    GenICam::gcstring acquisitionModeInitial = Arena::GetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "AcquisitionMode");

    // Set acquisition mode
    std::cout << TAB1 << "Set acquisition mode to 'Continuous'\n";
    Arena::SetNodeValue<GenICam::gcstring>( pDevice->GetNodeMap(), "AcquisitionMode", "Continuous");

    // Set buffer handling mode
    std::cout << TAB1 << "Set buffer handling mode to 'NewestOnly'\n";
    Arena::SetNodeValue<GenICam::gcstring>( pDevice->GetTLStreamNodeMap(), "StreamBufferHandlingMode", "NewestOnly");


    // Start stream
    std::cout << "\nStart stream\n";
    pDevice->StartStream();


    // Main loop
    bool first_loop = true;
    while (1)
    {
        //////////////////////////////
        //        画像の取得          //
        //////////////////////////////

        // USB camera
        cv::Mat usbImg;
        if (capture.isOpened()) {
            capture >> usbImg;
        }

        // Get image
        Arena::IImage* pImage = pDevice->GetImage(TIMEOUT);
        if (first_loop) {
            // 初回ループの時にだけ，偏向カメラの画像サイズを表示
            size_t width = pImage->GetWidth();
            size_t height = pImage->GetHeight();
            std::cout << TAB1 << "Original Raw Image Size: " << width << " x " << height << std::endl;
        }

        // 原画像をOpenCVのMatに変換
        cv::Mat orgImg;
        GetMatFromIImage(pImage, orgImg);

        // 次の画像取得ための準備
        pDevice->RequeueBuffer(pImage);



        //////////////////////////////
        //    各種偏向画像の取り出し    //
        //////////////////////////////

        // 各角度の偏光画像の取得
        cv::Mat angleImgs[4];
        GetPolarizedImages(orgImg, angleImgs);

        // 平均画像の取得
        cv::Mat aveImg;
        GetAverageImage(angleImgs, aveImg);

        // ここから処理を軽くするため画像を縮小
        for (int i = 0; i < 4; i++)
            resize(angleImgs[i], angleImgs[i], cv::Size(), RESIZE_RATIO, RESIZE_RATIO);
        if (first_loop) {
            // 初回ループの時だけ処理画像サイズを表示
            int width = angleImgs[0].cols;
            int height = angleImgs[0].rows;
            std::cout << TAB1 << "Resize Ratio: " << RESIZE_RATIO << std::endl;
            std::cout << TAB1 << "Processing Image Size: " << width << " x " << height << std::endl;
        }

        // 角度画像から最大値，最小値画像を作成
        cv::Mat min, max;
        GetMinMaxImages(angleImgs, min, max);

        // 角度画像からストークスパラメータ画像を作成
        cv::Mat stkImgs[3];
        GetStokesImages( angleImgs, stkImgs);

        // 角度画像から絶対値ストークスパラメータ画像を作成
        cv::Mat aStkImgs[3];
        GetAbsStokesImages(angleImgs, aStkImgs);

        // 角度画像から DoLP,AoLPとその合成画像を取得する
        cv::Mat xoLPImgs[3];
        GetXoLPImages(angleImgs, xoLPImgs, focus_gamma, gamma, th);


        //////////////////////////////
        //     表示のための画像作成    //
        //////////////////////////////

        // すべて統合
        cv::Mat finalImg;
        cv::Size size(RESULT_W, RESULT_H);
        if (mode == ShowMode::Average) {
            MakeAverageImage(aveImg, size, finalImg);
        }
        else if (mode == ShowMode::Polarization) {
            MakePolarizationImage(angleImgs, size, finalImg);
        }
        else if (mode == ShowMode::Stokes) {
            MakeAbsStokesImage(aStkImgs, size, finalImg);
        }
        else if (mode == ShowMode::XoLP) {
            MakeXoPLImage(xoLPImgs, size, finalImg, focus_gamma, gamma, th);
        }
        else {
            //size = cv::Size((int)(1.5 * size.width), (int)(1.5 * size.height));
            MakeAllImage(aveImg, angleImgs, aStkImgs, xoLPImgs, size, finalImg, focus_gamma, gamma, th);
        }


        //////////////////////////////
        //          画像表示         //
        //////////////////////////////

        // 結果表示
        cv::imshow("Result", finalImg);

        // USBカメラ画像の表示
        if (showUsbCam && capture.isOpened())
            cv::imshow("USB Camera", usbImg);
        else
            cv::destroyWindow("USB Camera");


        //////////////////////////////
        //          Key処理          //
        //////////////////////////////
        {
            int key = cv::waitKeyEx(1);

            if (key == '1')
                mode = ShowMode::Average;
            else if (key == '2')
                mode = ShowMode::Polarization;
            else if (key == '3')
                mode = ShowMode::Stokes;
            else if (key == '4')
                mode = ShowMode::XoLP;
            else if (key == '5')
                mode = ShowMode::All;
            else if (key == 27) // Escで脱出
                break;
            else if (key == 'u')
                showUsbCam = !showUsbCam;

            // ガンマ補正と閾値処理
            if (mode == ShowMode::All || mode == ShowMode::XoLP) {
                // 上下
                if (key == KEY_U || key == KEY_D)
                    focus_gamma = !focus_gamma;
                // 左
                else if (key == KEY_L) {
                    if (focus_gamma)
                        gamma /= g_dif;
                    else
                        th -= t_dif;
                }
                // 右
                else if (key == KEY_R) {
                    if (focus_gamma)
                        gamma *= g_dif;
                    else
                        th += t_dif;
                }
                if (key == '0') {
                    if (focus_gamma) gamma = 1.0;
                    else th = 128;
                }
            }

        }// key処理


        // 初回ループのフラグを下ろす
        first_loop = false;
    }

    // Stop stream
    std::cout << "Stop stream\n";
    pDevice->StopStream();

    // return nodes to their initial values
    Arena::SetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "AcquisitionMode", acquisitionModeInitial);

    // clean up
    pSystem->DestroyDevice(pDevice);
    Arena::CloseSystem(pSystem);

    // OpenCVのウィンドウを全て開放
    cv::destroyAllWindows();
    
    // 終了
    std::cout << "\nProgram Successfully End" << std::endl;

    return 0;
}
