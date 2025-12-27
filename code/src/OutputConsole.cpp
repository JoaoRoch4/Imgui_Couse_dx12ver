#include "PCH.hpp"
#include "OutputConsole.hpp"
#include "ConsoleWindow.hpp"
#include "Classes.hpp"
#include "App.hpp"
#include "DX12Renderer.hpp"
#include <psapi.h>
#include <iomanip>
#include <dxgi1_4.h>

namespace app {

void OutputConsole::Open() {
    
	CreateConsole();

  
    ShowConsole(m_args->m_bShowCmd);
}

void OutputConsole::Tick() {

    ShowConsole(m_memory->m_bShowCmd);
}

void OutputConsole::Close() {}

OutputConsole::OutputConsole() :
m_bWasConsoleCreated(false),
m_bShouldCreateConsole(false),
m_bShowConsole(false),
m_hWnd_console(nullptr),
m_memory(nullptr),
m_args(nullptr) {

    m_memory = MemoryManagement::Get_MemoryManagement_Singleton();
    if(m_memory) m_args = m_memory->Get_CommandLineArguments();
    else throw std::runtime_error("m_memory is nullptr");
}

OutputConsole::~OutputConsole() {

	m_bWasConsoleCreated   = false;
	m_bShouldCreateConsole = false;
	m_bShowConsole		   = false;

	m_hWnd_console = nullptr;
	FreeConsole();
}

CustomOutput::CustomOutput() : m_consoleWindow(nullptr) {}

void CustomOutput::SetConsoleWindow(ConsoleWindow* consoleWindow) {
	m_consoleWindow = consoleWindow;
}

void CustomOutput::FlushToConsoleWindow() {
	if (m_consoleWindow && !m_buffer.empty()) {
		// Check if buffer ends with newline
		if (m_buffer.back() == '\n') {
			// Remove the newline before sending to ConsoleWindow
			m_buffer.pop_back();
			if (!m_buffer.empty()) { m_consoleWindow->AddLog("%s", m_buffer.c_str()); }
			m_buffer.clear();
		}
	}
}

void OutputConsole::CreateConsole() {


	if (!AllocConsole()) throw std::runtime_error("Cannot AllocConsole!");
	// 1. Redireciona stdout para o m_console
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);

	// Set console to use UTF-8 for both input and output
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
	
	// Set the console mode to support virtual terminal sequences (for better UTF-8 support)
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	GetConsoleMode(hOut, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hOut, dwMode);

	// Sync stdio - don't use locale as it can cause encoding issues
	std::ios::sync_with_stdio(true);

	m_hWnd_console		 = GetConsoleWindow();
	m_bWasConsoleCreated = true;

	ShowConsole(m_bShowConsole);
	// Use regular string and direct cout for initial message
	std::cout << "Console Window Created!" << std::endl;
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
		Out.WriteLine("Font size set to " + std::to_string(size) + "!");
	} else {
		throw std::runtime_error("can't set m_console font size!");
	}
}

void OutputConsole::ShowConsole(bool bShow) {

	m_bShowConsole = bShow;

	if (m_hWnd_console) ShowWindow(m_hWnd_console, m_bShowConsole ? SW_SHOW : SW_HIDE);
}

void OutputConsole::SetConsoleWindow(ConsoleWindow* consoleWindow) {
	Out.SetConsoleWindow(consoleWindow);
}

CustomOutput& CustomOutput::operator<<(const std::string& dado) {
	std::cout << dado;
	m_buffer += dado;
	FlushToConsoleWindow();
	return *this;
}

CustomOutput& CustomOutput::operator<<(const long long& valor) {
	{
		std::cout << valor;
		m_buffer += std::to_string(valor);
		FlushToConsoleWindow();
		return *this;
	}
}

CustomOutput& CustomOutput::operator<<(const int& valor) {
	std::cout << valor;
	m_buffer += std::to_string(valor);
	FlushToConsoleWindow();
	return *this;
}

CustomOutput& CustomOutput::operator<<(const long double& valor) {

	std::cout << valor;
	m_buffer += std::to_string(valor);
	FlushToConsoleWindow();
	return *this;
}

CustomOutput& CustomOutput::operator<<(const float& valor) {
	std::cout << valor;
	m_buffer += std::to_string(valor);
	FlushToConsoleWindow();
	return *this;
}

CustomOutput& CustomOutput::operator<<(const char* dado) {
	std::cout << dado;
	
	// Check if this is an ImGui color tag
	if (dado && dado[0] == '[' && strchr(dado, ']')) {
		m_currentColorTag = dado;
		// Add color tag to buffer for ImGui console
		m_buffer += dado;
		m_buffer += " "; // Space after tag for cleaner appearance
	} else {
		m_buffer += dado;
	}
	
	FlushToConsoleWindow();
	return *this;
}

CustomOutput& CustomOutput::operator<<(const std::wstring& dado) {
	std::wcout << dado;
	// Convert wstring to UTF-8 string for ConsoleWindow
	if (m_consoleWindow && !dado.empty()) {
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, dado.c_str(), (int)dado.size(), nullptr, 0, nullptr, nullptr);
		if (size_needed > 0) {
			std::string str(size_needed, 0);
			WideCharToMultiByte(CP_UTF8, 0, dado.c_str(), (int)dado.size(), &str[0], size_needed, nullptr, nullptr);
			m_buffer += str;
			FlushToConsoleWindow();
		}
	}
	return *this;
}

CustomOutput& CustomOutput::operator<<(const wchar_t* dado) {
	std::wcout << dado;
	// Convert wchar_t* to UTF-8 string for ConsoleWindow
	if (m_consoleWindow && dado && dado[0] != L'\0') {
		int len = (int)wcslen(dado);
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, dado, len, nullptr, 0, nullptr, nullptr);
		if (size_needed > 0) {
			std::string str(size_needed, 0);
			WideCharToMultiByte(CP_UTF8, 0, dado, len, &str[0], size_needed, nullptr, nullptr);
			m_buffer += str;
			FlushToConsoleWindow();
		}
	}
	return *this;
}

void CustomOutput::WriteLine(const std::string& message) {
	// Output to Windows console
	std::cout << message << std::endl;
	std::cout.flush();
	
	// Output to ImGui console window
	if (m_consoleWindow) {
		m_consoleWindow->AddLog("%s", message.c_str());
	}
}

void CustomOutput::WriteLine(const std::wstring& message) {
	// Output to Windows console
	std::wcout << message << std::endl;
	std::wcout.flush();
	
	// Output to ImGui console window (convert to UTF-8)
	if (m_consoleWindow && !message.empty()) {
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, message.c_str(), (int)message.size(), nullptr, 0, nullptr, nullptr);
		if (size_needed > 0) {
			std::string str(size_needed, 0);
			WideCharToMultiByte(CP_UTF8, 0, message.c_str(), (int)message.size(), &str[0], size_needed, nullptr, nullptr);
			m_consoleWindow->AddLog("%s\n", str.c_str());
		}
	}
}

void CustomOutput::Write(const std::string& message) {
	// Output to Windows console without newline
	std::cout << message;
	std::cout.flush();
	
	// Add to buffer for ImGui console
	m_buffer += message;
	// Check if we should flush (if message contains newline)
	if (message.find('\n') != std::string::npos) {
		ForceFlush();
	}
}

void CustomOutput::Write(const std::wstring& message) {
	// Output to Windows console without newline
	std::wcout << message;
	std::wcout.flush();
	
	// Convert and add to buffer for ImGui console
	if (!message.empty()) {
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, message.c_str(), (int)message.size(), nullptr, 0, nullptr, nullptr);
		if (size_needed > 0) {
			std::string str(size_needed, 0);
			WideCharToMultiByte(CP_UTF8, 0, message.c_str(), (int)message.size(), &str[0], size_needed, nullptr, nullptr);
			m_buffer += str;
			// Check if we should flush (if message contains newline)
			if (str.find('\n') != std::string::npos) {
				ForceFlush();
			}
		}
	}
}

void CustomOutput::ForceFlush() {
	if (m_consoleWindow && !m_buffer.empty()) {
		// Remove trailing newline if present
		if (m_buffer.back() == '\n') {
			m_buffer.pop_back();
		}
		if (!m_buffer.empty()) {
			m_consoleWindow->AddLog("%s", m_buffer.c_str());
		}
		m_buffer.clear();
	}
}

CustomOutput& CustomOutput::SetImGuiColor(const char* colorTag) {
	if (colorTag) {
		m_currentColorTag = colorTag;
		// Add color tag to buffer
		m_buffer += colorTag;
		m_buffer += " ";
	}
	return *this;
}

CustomOutput& CustomOutput::ResetImGuiColor() {
	m_currentColorTag.clear();
	return *this;
}

void CustomOutput::ShowSystemStatus() {
	auto app = app::App::GetInstance();
	if (!app) {
		WriteLine("[ERROR] Application instance not available!");
		return;
	}

	WriteLine(std::string(60, '='));
	WriteLine("           APPLICATION STATUS REPORT");
	WriteLine(std::string(60, '='));

	// DirectX 12 Renderer Status
	WriteLine("\n[DIRECTX 12 RENDERER]");
	auto renderer = app->GetRenderer();
	if (renderer && renderer->GetDevice()) {
		WriteLine("  Device: Initialized");
		
		// Get adapter information
		ComPtr<IDXGIFactory4> factory;
		if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(factory.put())))) {
			ComPtr<IDXGIAdapter1> adapter;
			if (SUCCEEDED(factory->EnumAdapters1(0, adapter.put()))) {
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);
				
				// Convert wide string to UTF-8
				std::wstring wAdapterName(desc.Description);
				int size_needed = WideCharToMultiByte(CP_UTF8, 0, wAdapterName.c_str(), (int)wAdapterName.size(), nullptr, 0, nullptr, nullptr);
				std::string adapterName;
				if (size_needed > 0) {
					adapterName.resize(size_needed);
					WideCharToMultiByte(CP_UTF8, 0, wAdapterName.c_str(), (int)wAdapterName.size(), &adapterName[0], size_needed, nullptr, nullptr);
				}
				WriteLine("  GPU: " + adapterName);
				
				// Video Memory
				WriteLine("  Dedicated Video Memory: " + std::to_string(desc.DedicatedVideoMemory / (1024 * 1024)) + " MB");
				WriteLine("  Dedicated System Memory: " + std::to_string(desc.DedicatedSystemMemory / (1024 * 1024)) + " MB");
				WriteLine("  Shared System Memory: " + std::to_string(desc.SharedSystemMemory / (1024 * 1024)) + " MB");
				
				std::ostringstream oss;
				oss << "  Vendor ID: 0x" << std::hex << std::uppercase << desc.VendorId;
				WriteLine(oss.str());
				
				oss.str("");
				oss << "  Device ID: 0x" << std::hex << std::uppercase << desc.DeviceId;
				WriteLine(oss.str());
			}
		}
		
		// Swap Chain Info
		if (renderer->GetSwapChain()) {
			DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
			renderer->GetSwapChain()->GetDesc1(&swapChainDesc);
			
			WriteLine("  Swap Chain Format: " + std::to_string(swapChainDesc.Format));
			WriteLine("  Swap Chain Buffers: " + std::to_string(swapChainDesc.BufferCount));
			WriteLine("  Swap Chain Size: " + std::to_string(swapChainDesc.Width) + "x" + std::to_string(swapChainDesc.Height));
			WriteLine("  Tearing Support: " + std::string(renderer->GetSwapChainTearingSupport() ? "Yes" : "No"));
			WriteLine("  Occluded: " + std::string(renderer->GetSwapChainOccluded() ? "Yes" : "No"));
		}
		
		// Frame sync info
		WriteLine("  Frame Index: " + std::to_string(renderer->GetFrameIndex()));
		WriteLine("  Fence Value: " + std::to_string(renderer->GetFenceLastSignaledValue()));
		
		// Command Queue
		if (renderer->GetCommandQueue()) {
			WriteLine("  Command Queue: Active");
		}
		
		// Command List
		if (renderer->GetCommandList()) {
			WriteLine("  Command List: Active");
		}
		
		// Descriptor Heaps
		if (renderer->GetSrvDescHeap()) {
			D3D12_DESCRIPTOR_HEAP_DESC heapDesc = renderer->GetSrvDescHeap()->GetDesc();
			WriteLine("  SRV Heap Descriptors: " + std::to_string(heapDesc.NumDescriptors));
			WriteLine("  SRV Heap Type: " + std::to_string(heapDesc.Type));
		}
		
		// Render Targets
		int validRenderTargets = 0;
		for (UINT i = 0; i < APP_NUM_BACK_BUFFERS; i++) {
			if (renderer->GetRenderTarget(i)) {
				validRenderTargets++;
			}
		}
		WriteLine("  Render Targets: " + std::to_string(validRenderTargets) + "/" + std::to_string(APP_NUM_BACK_BUFFERS));
		
	} else {
		WriteLine("  Device: Not Initialized");
	}

	// ImGui Status
	WriteLine("\n[IMGUI CONTEXT]");
	if (ImGui::GetCurrentContext()) {
		ImGuiIO& io = ImGui::GetIO();
		WriteLine("  Context: Initialized");
		WriteLine("  Frame Count: " + std::to_string(ImGui::GetFrameCount()));
		
		std::ostringstream oss;
		oss << std::fixed << std::setprecision(1);
		oss << "  FPS: " << io.Framerate;
		WriteLine(oss.str());
		
		oss.str("");
		oss << std::fixed << std::setprecision(4);
		oss << "  Delta Time: " << io.DeltaTime << "s";
		WriteLine(oss.str());
		
		WriteLine("  Display Size: " + std::to_string((int)io.DisplaySize.x) + "x" + std::to_string((int)io.DisplaySize.y));
		WriteLine("  Mouse Position: (" + std::to_string((int)io.MousePos.x) + ", " + std::to_string((int)io.MousePos.y) + ")");
		WriteLine("  Backend Platform: " + std::string(io.BackendPlatformName ? io.BackendPlatformName : "None"));
		WriteLine("  Backend Renderer: " + std::string(io.BackendRendererName ? io.BackendRendererName : "None"));
		WriteLine("  Fonts Loaded: " + std::to_string(io.Fonts->Fonts.Size));
	} else {
		WriteLine("  Context: Not Initialized");
	}

	// ImPlot Status
	WriteLine("\n[IMPLOT CONTEXT]");
	if (ImPlot::GetCurrentContext()) {
		WriteLine("  Context: Initialized");
	} else {
		WriteLine("  Context: Not Initialized");
	}

	// Memory Info
	WriteLine("\n[MEMORY STATUS]");
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	if (GlobalMemoryStatusEx(&memInfo)) {
		WriteLine("  Physical Memory Usage: " + std::to_string(memInfo.dwMemoryLoad) + "%");
		WriteLine("  Total Physical: " + std::to_string(memInfo.ullTotalPhys / (1024 * 1024)) + " MB");
		WriteLine("  Available Physical: " + std::to_string(memInfo.ullAvailPhys / (1024 * 1024)) + " MB");
		WriteLine("  Total Virtual: " + std::to_string(memInfo.ullTotalVirtual / (1024 * 1024)) + " MB");
		WriteLine("  Available Virtual: " + std::to_string(memInfo.ullAvailVirtual / (1024 * 1024)) + " MB");
	}

	// Process Memory
	PROCESS_MEMORY_COUNTERS_EX pmc;
	if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
		WriteLine("  Process Working Set: " + std::to_string(pmc.WorkingSetSize / (1024 * 1024)) + " MB");
		WriteLine("  Process Private Bytes: " + std::to_string(pmc.PrivateUsage / (1024 * 1024)) + " MB");
		WriteLine("  Page Fault Count: " + std::to_string(pmc.PageFaultCount));
	}

	// System Time
	WriteLine("\n[SYSTEM INFORMATION]");
	auto now = std::chrono::system_clock::now();
	auto time = std::chrono::system_clock::to_time_t(now);
	std::string timeStr = std::ctime(&time);
	if (!timeStr.empty() && timeStr.back() == '\n') timeStr.pop_back();
	WriteLine("  Current Time: " + timeStr);
	
	// CPU Info
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	WriteLine("  Processor Count: " + std::to_string(sysInfo.dwNumberOfProcessors));
	WriteLine("  Page Size: " + std::to_string(sysInfo.dwPageSize / 1024) + " KB");
	WriteLine("  Hardware Concurrency: " + std::to_string(std::thread::hardware_concurrency()));

	WriteLine("\n" + std::string(60, '='));
	WriteLine("           END OF STATUS REPORT");
	WriteLine(std::string(60, '='));
}

// CustomOutput& CustomOutput::operator<<(std::ostream& (*manip)(std::ostream&)) {
//
//	manip(std::cout);
//	return *this;
// }
//
// CustomOutput& CustomOutput::operator<<(std::wostream& (*manip)(std::wostream&)) {
//
//	manip(std::wcout);
//	return *this;
// }

} // namespace app
