#pragma once
#include "PCH.hpp"



class Helpers {

    public:
        static void ShowExampleMenuFile();

       static  void ShowExampleAppMainMenuBar();

        static int ErrorMsg(std::exception& e);

        static  int UnkExcpt();

        static std::string strtoLower(std::string s);

        static std::wstring strtoLowerW(std::wstring s);

        static void charToLower(char* c);
        static const char* constCharToLower(const char* c);
};

