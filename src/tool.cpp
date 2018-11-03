//
// tool.cpp
// 2018-11-03
// S.Nakamura
//


#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <time.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#include "tool.h"

// YYMMDD_hhmmss�`���Ō��݂̎����̕�������擾����
std::string getDateTimeStr(void)
{
    using namespace std;

    time_t t = time(NULL);
    struct tm tm;
    struct tm * ptm = &tm;
#ifdef _WIN32
    localtime_s(ptm, &t);
#else
    ptm = localtime(&t);
#endif

    std::ostringstream sout;
    sout << std::to_string(ptm->tm_year - 100); // �N
    sout << std::setfill('0') << std::setw(2) << ptm->tm_mon + 1;  // ��
    sout << std::setfill('0') << std::setw(2) << ptm->tm_mday; // ��
    sout << "_";
    sout << std::setfill('0') << std::setw(2) << ptm->tm_hour; // ��
    sout << std::setfill('0') << std::setw(2) << ptm->tm_min; // ��
    sout << std::setfill('0') << std::setw(2) << ptm->tm_sec;  // �b

    return sout.str();
}

// �f�B���N�g�����쐬����
// �f�B���N�g���������܂��͍쐬������ true ��Ԃ�
bool makeDirectry(std::string dir)
{
    // �f�B���N�g���̍쐬
#ifdef _WIN32
    _mkdir(dir.c_str());
#else
    mkdir(dir.c_str(), 0775);
#endif

    // �f�B���N�g���������true��Ԃ�
    struct stat statBuf;
    if (stat(dir.c_str(), &statBuf) == 0)
        return true;
    else
        return false;
}

// �w��̃C���^�[�o�����o�߂������`�F�b�N����
bool checkInterval(unsigned intvl_ms)
{
    // �O��X�^���v����̌o�ߎ��Ԍv�Z
    static std::chrono::system_clock::time_point stamp_time = std::chrono::system_clock::now();
    auto now = std::chrono::system_clock::now();
    auto dur = now - stamp_time;
    auto intvl = std::chrono::milliseconds(intvl_ms);

    // �C���^�[�o�����o�߂��Ă���
    if (dur >= intvl) {
        //stamp_time = now - (dur - intvl);
        stamp_time = now;
        return true;
    }

    // �C���^�[�o�����o�߂��Ă��Ȃ�
    return false;
}
