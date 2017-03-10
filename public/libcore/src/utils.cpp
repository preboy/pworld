#include "stdafx.h"
#include "utils.h"


namespace Utils
{

    void log2file(const char* name, std::list<std::string>& files)
    {
        FILE* f = fopen(name, "a+t");
        if (f)
        {
            for (auto& file : files)
            {
                fputs(file.c_str(), f);
                fputs("\r\n", f);
            }
            fclose(f);
        }
    }


    void ScanDir(const char* dir, std::list<std::string>& files)
    {
#ifdef PLAT_WIN
        std::string fn(dir);
        fn += "/*";
        WIN32_FIND_DATAA  FindFileData;
        HANDLE hFind = ::FindFirstFileA(fn.c_str(), &FindFileData);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            do
            {
                if (FindFileData.cFileName[0] == '.')
                {
                    continue;
                }
                std::string fname(dir);
                fname += "/";
                fname += FindFileData.cFileName;

                if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    ScanDir(fname.c_str(), files);
                }
                else
                {
                    files.push_back(fname);
                }

            } while (::FindNextFileA(hFind, &FindFileData));
        }
        ::FindClose(hFind);
#endif // PLAT_WIN
    }


    void Sleep(uint32 millisecond)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(millisecond));
    }

}