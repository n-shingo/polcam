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
#include <direct.h>
#include "tool.h"

// YYMMDD_hhmmss�`���Ō��݂̎����̕�������擾����
std::string getDateTimeStr(void)
{
    using namespace std;

    time_t t = time(NULL);
    struct tm tm;
    localtime_s(&tm, &t);

    std::ostringstream sout;
    sout << std::to_string(tm.tm_year - 100); // �N
    sout << std::setfill('0') << std::setw(2) << tm.tm_mon + 1;  // ��
    sout << std::setfill('0') << std::setw(2) << tm.tm_mday; // ��
    sout << "_";
    sout << std::setfill('0') << std::setw(2) << tm.tm_hour; // ��
    sout << std::setfill('0') << std::setw(2) << tm.tm_min; // ��
    sout << std::setfill('0') << std::setw(2) << tm.tm_sec;  // �b

    return sout.str();
}

// �f�B���N�g�����쐬����
// �f�B���N�g���������܂��͍쐬������ true ��Ԃ�
bool makeDirectry(std::string dir)
{
    // �f�B���N�g���̍쐬
    struct stat statBuf;
#ifdef _WIN32
    _mkdir(dir.c_str());
#elif
    mkdir(dir.c_str());
#endif

    // �f�B���N�g���������true��Ԃ�
    if (stat(dir.c_str(), &statBuf) == 0)
        return true;
    else
        return false;
}

// �w��̃C���^�[�o�����o�߂������`�F�b�N����
bool checkInterval(unsigned intvl_ms)
{
    // �O��X�^���v����̌o�ߎ��Ԍv�Z
    static auto stamp_time = std::chrono::system_clock::now();
    auto now = std::chrono::system_clock::now();
    auto dur = now - stamp_time;
    auto intvl = std::chrono::milliseconds(intvl_ms);

    // �C���^�[�o�����o�߂��Ă���
    if (dur >= intvl) {
        stamp_time = now - (dur - intvl);
        return true;
    }

    // �C���^�[�o�����o�߂��Ă��Ȃ�
    return false;
}