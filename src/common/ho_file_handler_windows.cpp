
#include "common/ho_file_handler.h"

#include <io.h>

using namespace std;
using namespace utility;

int HoFileHandler::BrouseDirectory(const std::string& strPath)
{
    _finddata_t fileDir;

    long lfDir;

    if ((lfDir = _findfirst(strPath.c_str(), &fileDir)) == -1l)
    {
        //printf("No file is found\n");
    }
    else
    {
        do
        {
            m_vecFiles.push_back(fileDir.name);

        } while (_findnext(lfDir, &fileDir) == 0);
    }

    _findclose(lfDir);

    return m_vecFiles.size();
}

/*
void show_file(WCHAR path[], int level = 0)
{
    WCHAR find_path[128];
    wsprintf(find_path, L"%s*", path);
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;
    bool bContinue = true;
    hFind = FindFirstFile(find_path, &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE)
        return;
    while (bContinue)
    {
        if (stricmp(FindFileData.cFileName, "..") && stricmp(FindFileData.cFileName, "."))
        {
            for (int i = 0; i < level; ++i)
                cout << "  ";
            cout << FindFileData.cFileName << endl;
            if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
            {
                wsprintf(find_path, L"%s%s\\", path, FindFileData.cFileName);
                show_file(find_path, level + 1);
            }
        }
        bContinue = FindNextFile(hFind, &FindFileData);
    }
}
*/
