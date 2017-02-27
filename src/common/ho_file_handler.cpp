
#include "common/ho_file_handler.h"

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

#ifdef _WIN32
#include "ho_file_handler_windows.cpp"
#elif __linux__
#include "ho_file_handler_unix.cpp"
#endif

HO_NAMESPACE_END