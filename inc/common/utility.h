
#ifndef _HO_UTILITY_H
#define _HO_UTILITY_H

#include "config.h"
#include <sstream>
#include <iostream>

#include "json/json.h"

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
#ifdef LOG_TO_FILE
#else
    std::cout << str << std::endl;
#endif // LOG_TO_FILE
}
#else
#define LogInfo(n)  
#endif

void PrintDistanceToFile(int nNode, double** distanceMatrix);

int ParseConfigFile();

HO_NAMESPACE_END

#endif