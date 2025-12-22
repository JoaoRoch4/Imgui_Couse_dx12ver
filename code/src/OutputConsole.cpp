#include "PCH.hpp"
#include "OutputConsole.hpp"

namespace app {

void OutputConsole::Open() {

	if (!m_bShouldCreateConsole) m_bShouldCreateConsole = true;
	if (!m_bShowConsole) m_bShowConsole = true;
	CreateConsole();
}

void OutputConsole::Tick() {}

void OutputConsole::Close() {}

OutputConsole::OutputConsole()
:

  m_bWasConsoleCreated(false),
  m_bShouldCreateConsole(false),
  m_bShowConsole(false),
  m_hWnd_console(nullptr)

{}

OutputConsole::~OutputConsole() {

	m_bWasConsoleCreated   = false;
	m_bShouldCreateConsole = false;
	m_bShowConsole		   = false;

	m_hWnd_console = nullptr;
	FreeConsole();
}

void OutputConsole::CreateConsole() {


	if (!AllocConsole()) throw std::runtime_error("Cannot AllocConsole!");
	// 1. Redireciona stdout para o console
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);

	SetConsoleOutputCP(CP_UTF8);
	std::wcout.imbue(std::locale(""));

	// 2. CONFIGURAÇÃO CRUCIAL PARA WCOUT:
	// Define o modo de saída do console para Unicode (UTF-16)
	//_setmode(_fileno(stdout), _O_U8TEXT);

	std::ios::sync_with_stdio(true);
	std::wcout.clear();

	m_hWnd_console		 = GetConsoleWindow();
	m_bWasConsoleCreated = true;

	ShowConsole(m_bShowConsole);
	std::wcout << L"Console Window Created!\n\n";
	setConsoleFontSize(24);
}

void OutputConsole::setConsoleFontSize(int size) {

	// 1. Obter o handle de saída do console
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	// 2. Preparar a estrutura de informações da fonte
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont  = 0;
	cfi.dwFontSize.X =
		0; // Largura (0 permite que o Windows escolha a melhor largura para a altura)
	cfi.dwFontSize.Y = size; // Altura da fonte
	cfi.FontFamily	 = FF_DONTCARE;
	cfi.FontWeight	 = FW_NORMAL;			// Use FW_BOLD para negrito
	std::wcscpy(cfi.FaceName, L"Consolas"); // Nome da fonte (precisa ser wchar_t)

	// 3. Aplicar a nova fonte
	if (SetCurrentConsoleFontEx(hOut, FALSE, &cfi)) {
		std::cout << "Font size set to " << std::to_string(size) << "! \n";
	} else {
		throw std::runtime_error("can't set console font size!");
	}
}

void OutputConsole::ShowConsole(bool bShow) {

	m_bShowConsole = bShow;

	if (m_hWnd_console) ShowWindow(m_hWnd_console, m_bShowConsole ? SW_SHOW : SW_HIDE);
}

CustomOutput& CustomOutput::operator<<(const std::string& dado) {
	std::cout << dado;
	return *this;
}

CustomOutput& CustomOutput::operator<<(const long long& valor) {
	{
		std::cout << valor;
		return *this;
	}
}

CustomOutput& CustomOutput::operator<<(const long double& valor) {

	std::cout << valor;
	return *this;
}

CustomOutput& CustomOutput::operator<<(const float& valor) {
	std::cout << valor;
	return *this;
}

CustomOutput& CustomOutput::operator<<(const char* dado) {
	std::cout << dado;
	return *this;
}

CustomOutput& CustomOutput::operator<<(const std::wstring& dado) {
	std::wcout << dado;
	return *this;
}

CustomOutput& CustomOutput::operator<<(const wchar_t* dado) {
	std::wcout << dado;
	return *this;
}

//CustomOutput& CustomOutput::operator<<(std::ostream& (*manip)(std::ostream&)) {
//
//	manip(std::cout);
//	return *this;
//}
//
//CustomOutput& CustomOutput::operator<<(std::wostream& (*manip)(std::wostream&)) {
//
//	manip(std::wcout);
//	return *this;
//}

} // namespace app
