#pragma once

// functional refer to differ platform
namespace Utils
{
    // for debug
    void log2file(const char* name, std::list<std::string>& files);
    
    // facility
    void ScanDir(const char* dir, std::list<std::string>& files);

    // cross platform
    void Sleep(uint32 millisecond);

};
