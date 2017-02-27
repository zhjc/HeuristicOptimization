
#ifndef HO_FILE_HANDLER_H
#define HO_FILE_HANDLER_H

#include "config.h"
#include <string>
#include <vector>

HO_NAMESPACE_BEGIN(utility)

class HoFileHandler
{
public:
    HoFileHandler();
    ~HoFileHandler();

public:
    int BrouseDirectory(const std::string& strPath);

    int GetFilesNum() const
    {
        return m_vecFiles.size();
    }

    void GetFilesSet(std::vector<std::string>* pvec) const
    {
        pvec->insert(pvec->begin(), m_vecFiles.begin(), m_vecFiles.end());
    }

    hoStatus GetFilesSetFromFile(const std::string& strFilePath, std::vector<std::string>* pvec);

private:
    bool m_bBrouseRecursion;
    std::string m_strCurDir;

    std::vector<std::string> m_vecFiles;
};


HO_NAMESPACE_END

#endif