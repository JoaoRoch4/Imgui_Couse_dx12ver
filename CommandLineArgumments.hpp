#pragma once
#include "PCH.hpp"
#include "Classes.hpp"
class CommandLineArgumments {

public:

	CommandLineArgumments();
	virtual ~CommandLineArgumments();

	int GetInitArgs();

    std::map<std::wstring, uint64_t> &GetMap() {
		return Args;
	}

private:

    std::wstring toLower(std::wstring s);
    std::map<std::wstring, uint64_t> Args;
	LPWSTR *szArgList;
	int argCount;
};
