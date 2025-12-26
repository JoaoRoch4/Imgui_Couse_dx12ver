#pragma once
#include "PCH.hpp"
class Conv {

public:

    std::wstring strtoWstr(const str& Txt);
    std::wstring strtoWstr(const char* Txt);

    std::string WStrToStr(const wstr& Txt);
    std::string WStrToStr(const wchar_t* Txt);

};

class ConvStatic {

    
public:

    static std::wstring strtoWstr(const str& Txt);
    static std::wstring strtoWstr(const char* Txt);

};


