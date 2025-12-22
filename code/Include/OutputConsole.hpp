#pragma once
#include "pch.hpp"
#include "Classes.hpp"

namespace app {

class CustomOutput{
public:
	template <class elem, class _Traits = std::char_traits<char>>
	friend std::basic_ostream<char, std::char_traits<char>>& operator<<(
		std::basic_ostream<elem, _Traits>& _Ostr, elem _Val) {

		_Ostr << _Val;
		return _Ostr;
	}


	template <class elem = char, class _Traits = std::char_traits<char>>
	std::basic_ostream<char, std::char_traits<char>>& operator<<(
		std::basic_ostream<char, std::char_traits<char>>&(__cdecl* _Pfn)(
			std::basic_ostream<char, std::char_traits<char>>&)) {
		return _Pfn(std::cout);
	}


	CustomOutput& operator<<(const long long& valor);
	CustomOutput& operator<<(const long double& valor);
	CustomOutput& operator<<(const float& valor);
	CustomOutput& operator<<(const char* dado);
	CustomOutput& operator<<(const wchar_t* dado);
	CustomOutput& operator<<(const std::string& dado);
	CustomOutput& operator<<(const std::wstring& dado);

	/* template<typename Func>
    auto operator<<(Func&& manipulator) -> decltype(manipulator(std::declval<std::ostream&>()), *this) {
        manipulator(std::cout);
        return *this;
    }*/
};

class OutputConsole : public Master {
public:
	CustomOutput Out;
	virtual void Open() override;
	virtual void Tick() override;
	virtual void Close() override;
	OutputConsole();
	virtual ~OutputConsole();

	void ShowConsole(bool bShow);

private:
	void CreateConsole();

	void setConsoleFontSize(int size);
	bool m_bWasConsoleCreated;
	bool m_bShouldCreateConsole;
	bool m_bShowConsole;
	HWND m_hWnd_console;
};

} // namespace app
