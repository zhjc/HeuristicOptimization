
#ifndef _HO_UTILITY_H
#define _HO_UTILITY_H

#include "config.h"
#include <sstream>
#include <iostream>

HO_NAMESPACE_BEGIN(utility)

template<class T>
std::string ToStr(T n)
{
    ostringstream stream;
    stream << n;
    return stream.str();
}

#ifdef PRINT_LOG
void LogInfo(const std::string& str)
{
    std::cout << str << std::endl;
}
#else
#define LogInfo(n)  
#endif

HO_NAMESPACE_END

#endif