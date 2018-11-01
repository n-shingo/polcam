//
// polcam.cpp
// 2018-10-31
// S.Nakamura
//

#include "opencv2/opencv.hpp"
#include "ArenaApi.h"
#include "polcam_funcs.h"

#define TAB1 "  "
#define TAB2 "    "
#define TAB3 "      "

#define TIMEOUT 2000

#define RESIZE_RATIO 0.25


int main( int argc, char *argv[] )
{
    std::cout << "Try Start Polarization Camera Acquisition\n";

    // preparation
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
    bool first = true;
    while (1)
    {
        // Get image
        Arena::IImage* pImage = pDevice->GetImage(TIMEOUT);

        // ���񃋁[�v�̎��ɂ����C�摜�T�C�Y��\��
        if (first) {
            size_t width = pImage->GetWidth();
            size_t height = pImage->GetHeight();
            std::cout << TAB1 << "Original Raw Image Size: " << width << "x" << height << std::endl;
        }

        // ���摜��OpenCV��Mat�ɕϊ�
        cv::Mat orgImg;
        GetMatFromIImage(pImage, orgImg);

        // ���̉摜�擾���߂̏���
        pDevice->RequeueBuffer(pImage);

        // �e�p�x�̕Ό��摜�̎擾
        cv::Mat angleImgs[4];
        GetPolarizedImages(orgImg, angleImgs);

        // �摜�k��
        for (int i = 0; i < 4; i++)
            resize(angleImgs[i], angleImgs[i], cv::Size(), RESIZE_RATIO, RESIZE_RATIO);

        // ���񃋁[�v�̎�����
        if (first) {
            int width = angleImgs[0].cols;
            int height = angleImgs[0].rows;
            std::cout << TAB1 << "Each Resized Image Size: " << width << "x" << height << std::endl;
        }


        // OpenCV�ŕ\��
        cv::imshow("original", orgImg);

        // �L�[����
        int key = cv::waitKey(10);
        if (key == 'q' || key == 27)
            break;

        // ���񃋁[�v�̃t���O���~�낷
        first = false;
    }

    // Stop stream
    std::cout << "Stop stream\n";
    pDevice->StopStream();

    // return nodes to their initial values
    Arena::SetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "AcquisitionMode", acquisitionModeInitial);

    // clean up
    pSystem->DestroyDevice(pDevice);
    Arena::CloseSystem(pSystem);

    // OpenCV�̃E�B���h�E��S�ĊJ��
    cv::destroyAllWindows();


    std::cout << "\nProgram Successfully End" << std::endl;

    return 0;
}