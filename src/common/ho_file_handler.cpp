
#include "common/ho_file_handler.h"
#include <fstream>
#include <iostream>

using namespace std;

HO_NAMESPACE_BEGIN(utility)

HoFileHandler::HoFileHandler()
: m_bBrouseRecursion(true)
{
    // nothing to do
}

HoFileHandler::~HoFileHandler()
{
    // nothing to do
}

hoStatus HoFileHandler::GetFilesSetFromFile(const string& strFilePath, vector<std::string>* pvec)
{
    ifstream infile;

    infile.open(strFilePath);
    if (!infile.is_open())
    {
        return hoError;
    }

    string strFileName;
    char ch[100];
    while (infile.getline(ch,100))
    {
        pvec->push_back(ch);
    }

    infile.close();

    return hoOK;
}

#ifdef _WIN32
#include "ho_file_handler_windows.cpp"
#elif __linux__
#include "ho_file_handler_unix.cpp"
#endif

HO_NAMESPACE_END