// MemoryManagement.hpp
// Header file for m_memory management and object allocation
// Centralized management of all major application objects

#pragma once

#include "pch.hpp"
#include "Master.hpp"
namespace app {

// Forward declarations to avoid circular dependencies
// These tell the compiler that these types exist without including headers
class CommandLineArguments;
class ConsoleWindow;
class ConsoleInputHandler;
class ConfigManager;
class StyleManager;
class DX12Renderer;
class DxDemos;
class DebugWindow;
class ExampleDescriptorHeapAllocator;
class FontManager;
class FontManagerWindow;
struct FrameContext;
class WindowClass;
class WindowManager;
class OutputConsole;

class MemoryManagement : public Master {

	virtual void Open() override;
	virtual void Tick() override;
	virtual void Close() override;

public:
	MemoryManagement();

	virtual ~MemoryManagement();

	void AllocAll();

	// DX12Renderer*					Get_DX12Renderer() const;


	static MemoryManagement*		Get_MemoryManagement();
	ExampleDescriptorHeapAllocator* Get_ExampleDescriptorHeapAllocator() const;

	CommandLineArguments* Get_CommandLineArguments() const;
	ConsoleInputHandler*  Get_ConsoleInputHandler() const;
	ConsoleWindow*		  Get_ConsoleWindow() const;
	DebugWindow*		  Get_DebugWindow() const;
	DX12Renderer*		  Get_DX12Renderer() const;
	FontManager*		  Get_FontManager() const;
	FontManagerWindow*	  Get_FontManagerWindow() const;
	WindowClass*		  Get_WindowClass() const;
	WindowManager*		  Get_WindowManager() const;
	ConfigManager*		  Get_ConfigManager() const;
	StyleManager*		  Get_StyleManager() const;
	OutputConsole*		  Get_OutputConsole() const;


protected:
	HRESULT Alloc_command_line_args();
	HRESULT Alloc_console_window();
	HRESULT Alloc_console_input_handler();
	HRESULT Alloc_config_manager();
	HRESULT Alloc_style_manager();
	HRESULT Alloc_dx12_renderer();
	HRESULT Alloc_dx_demos();
	HRESULT Alloc_debug_window();
	HRESULT Alloc_Example_Descriptor_Heap_Allocator();
	HRESULT Alloc_font_manager();
	HRESULT Alloc_font_manager_window();
	HRESULT Alloc_frame_context();
	HRESULT Alloc_window_class();
	HRESULT Alloc_window_manager();
	HRESULT Alloc_output_console();


private:
	// UPtr<DX12Renderer>					 dx12_renderer;

	std::shared_ptr<MemoryManagement> m_memory_management;


	UPtr<ExampleDescriptorHeapAllocator> m_Example_Descriptor_Heap_Allocator;

	UPtr<CommandLineArguments> m_command_line_args;
	UPtr<ConsoleWindow>		   m_console_window;
	UPtr<ConsoleInputHandler>  m_console_input_handler;
	UPtr<ConfigManager>		   m_config_manager;
	UPtr<StyleManager>		   m_style_manager;
	UPtr<DxDemos>			   m_dx_demos;
	UPtr<DX12Renderer>		   m_dx12_renderer;
	UPtr<DebugWindow>		   m_debug_window;
	UPtr<FontManager>		   m_font_manager;
	UPtr<FontManagerWindow>	   m_font_manager_window;
	UPtr<FrameContext>		   m_frame_context;
	UPtr<WindowClass>		   m_window_class;
	UPtr<WindowManager>		   m_window_manager;
	UPtr<OutputConsole>		   m_output_console;

	bool m_bCommand_line_args_allocated;
	bool m_bConsole_window_allocated;
	bool m_bConsole_input_handler_allocated;
	bool m_bConfig_manager_allocated;
	bool m_bStyle_manager_allocated;
	bool m_bDx12_renderer_allocated;
	bool m_bDx_demos_allocated;
	bool m_bDebug_window_allocated;
	bool m_bExample_Descriptor_Heap_Allocator_allocated;
	bool m_bFont_manager_allocated;
	bool m_bFont_manager_window_allocated;
	bool m_bFrame_context_allocated;
	bool m_bmemory_management_allocated;
	bool m_bWindow_class_allocated;
	bool m_bWindow_manager_allocated;
	bool m_bOutput_console_allocated;


	// imgui flags
public:
	bool m_bShow_demo_window;
	bool m_bShow_another_window;
	bool m_bShow_FontManager_window;
	bool m_bShow_styleEditor_window;
	bool m_bShow_Debug_window;
	bool m_bShow_FileSys_window;
	bool m_bShow_Console_window;

	void Set_ImGuiIO(ImGuiIO* m_io);

	ImGuiIO* Get_ImGuiIO();

	ImVec4* Get_clear_color_ptr();
	ImVec4	Get_clear_color() { return m_clear_color; }

private:
	ImGuiIO* m_io;
	bool	 bIoPassed;

	ImVec4* m_clear_color_ptr;
	ImVec4	m_clear_color;
};
} // namespace app
