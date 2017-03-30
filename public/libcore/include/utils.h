#pragma once

// 这里主要放一些public的函数，以及一些跨平台的东西
namespace Utils
{
    // for debug
    void log2file(const char* name, std::list<std::string>& files);
    
    // facility
    void ScanDir(const char* dir, std::list<std::string>& files);

    // cross platform
    void Sleep(uint32 millisecond);

};
