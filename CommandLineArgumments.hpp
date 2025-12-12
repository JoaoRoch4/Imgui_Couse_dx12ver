#pragma once
#include "PCH.hpp"
#include "Classes.hpp"
class CommandLineArgumments {
public:
	CommandLineArgumments();
	virtual ~CommandLineArgumments();

	int GetInitArgs();

	std::wstring ToPrint;

    std::map<std::wstring, uint64_t> &GetInitMap() {
		return Args;
	}

private:
        std::map<std::wstring, uint64_t> Args;

    std::wstring toLower(std::wstring s);
	LPWSTR *szArgList;
	int argCount;
};
