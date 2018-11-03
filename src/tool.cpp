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

// YYMMDD_hhmmss形式で現在の時刻の文字列を取得する
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
    sout << std::to_string(ptm->tm_year - 100); // 年
    sout << std::setfill('0') << std::setw(2) << ptm->tm_mon + 1;  // 月
    sout << std::setfill('0') << std::setw(2) << ptm->tm_mday; // 日
    sout << "_";
    sout << std::setfill('0') << std::setw(2) << ptm->tm_hour; // 時
    sout << std::setfill('0') << std::setw(2) << ptm->tm_min; // 分
    sout << std::setfill('0') << std::setw(2) << ptm->tm_sec;  // 秒

    return sout.str();
}

// ディレクトリを作成する
// ディレクトリが既存または作成されれば true を返す
bool makeDirectry(std::string dir)
{
    // ディレクトリの作成
#ifdef _WIN32
    _mkdir(dir.c_str());
#else
    mkdir(dir.c_str(), 0775);
#endif

    // ディレクトリがあればtrueを返す
    struct stat statBuf;
    if (stat(dir.c_str(), &statBuf) == 0)
        return true;
    else
        return false;
}

// 指定のインターバルが経過したかチェックする
bool checkInterval(unsigned intvl_ms)
{
    // 前回スタンプからの経過時間計算
    static std::chrono::system_clock::time_point stamp_time = std::chrono::system_clock::now();
    auto now = std::chrono::system_clock::now();
    auto dur = now - stamp_time;
    auto intvl = std::chrono::milliseconds(intvl_ms);

    // インターバルを経過している
    if (dur >= intvl) {
        //stamp_time = now - (dur - intvl);
        stamp_time = now;
        return true;
    }

    // インターバルを経過していない
    return false;
}
