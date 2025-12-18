#include "PCH.hpp"
#include "Conv.hpp"
#include "Classes.hpp"

std::wstring Conv::strtoWstr(const str& Txt) {
   
	const char*	 LVal	  = Txt.c_str();
	size_t		 charSize = std::strlen(LVal) + 1;
	std::wstring wcharResult(charSize, L'#');
	size_t		 convertedChars = std::mbstowcs(&wcharResult[0], LVal, charSize);

    try {
        if(convertedChars != static_cast<size_t>(-1)) {
            wcharResult.resize(convertedChars);
        } else throw std::runtime_error("Conversion error!");
    } catch(std::runtime_error &e) {
        MessageBox(NULL, L"Conversion error!", L"Error", MB_ICONERROR | MB_OK);
        std::abort();
    }
   
    return wcharResult;
}

std::wstring Conv::strtoWstr(const char* Txt) {
    const char*	 LVal	  = Txt;
	size_t		 charSize = std::strlen(LVal) + 1;
	std::wstring wcharResult(charSize, L'#');
	size_t		 convertedChars = std::mbstowcs(&wcharResult[0], LVal, charSize);

    try {
        if(convertedChars != static_cast<size_t>(-1)) {
            wcharResult.resize(convertedChars);
        } else throw std::runtime_error("Conversion error!");
    } catch(std::runtime_error &e) {
        MessageBox(NULL, L"Conversion error!", L"Error", MB_ICONERROR | MB_OK);
        std::abort();
    }
   
    return wcharResult;
}

std::string Conv::WStrToStr(const wstr& Txt) {

    // Setup converter
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;

    // Use converter to_bytes (wstring -> string)
    return converter.to_bytes(Txt);

}

std::string Conv::WStrToStr(const wchar_t* Txt) {
   // Setup converter
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;

    std::wstring Lval = Txt;
    // Use converter to_bytes (wstring -> string)
    return converter.to_bytes(Lval);
}

std::wstring ConvStatic::strtoWstr(const str& Txt) {
    static Conv conv;
    static std::wstring res;
    res = conv.strtoWstr(Txt);
    return res;
}

std::wstring ConvStatic::strtoWstr(const char* Txt) {
   static Conv conv;
    static std::wstring res;
      res = conv.strtoWstr(Txt);
    return res;
}
