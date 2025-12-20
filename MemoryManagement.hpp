// MemoryManagement.hpp
// Header file for memory management and object allocation
// Centralized management of all major application objects

#pragma once

#include "pch.hpp"

// Forward declarations to avoid circular dependencies
// These tell the compiler that these types exist without including headers
class CommandLineArguments;
class ConsoleWindow;
class ConsoleInputHandler;
class ConfigManager;
//class DX12Renderer;
class DxDemos;
class DebugWindow;
class ExampleDescriptorHeapAllocator;
class FontManager;
class FontManagerWindow;
struct FrameContext;
class WindowClass;
class WindowManager;
class OutputConsole;

class MemoryManagement {

public:
	MemoryManagement();

	virtual ~MemoryManagement();

	void AllocAll();

	//DX12Renderer*					Get_DX12Renderer() const;


	CommandLineArguments*			Get_CommandLineArguments() const;
	ConsoleInputHandler*			Get_ConsoleInputHandler() const;
	ConsoleWindow*					Get_ConsoleWindow() const;
	ExampleDescriptorHeapAllocator* Get_ExampleDescriptorHeapAllocator() const;
	WindowManager*					Get_WindowManager() const;
	ConfigManager*					Get_ConfigManager() const;
	OutputConsole*					Get_OutputConsole() const;

protected:
	void Alloc_command_line_args();
	void Alloc_console_window();
	void Alloc_console_input_handler();
	void Alloc_config_manager();
	//void Alloc_dx12_renderer();
	void Alloc_dx_demos();
	void Alloc_debug_window();
	void Alloc_Example_Descriptor_Heap_Allocator();
	void Alloc_font_manager();
	void Alloc_font_manager_window();
	void Alloc_frame_context();
	void Alloc_window_class();
	void Alloc_window_manager();
	void Alloc_output_console();


private:
	//UPtr<DX12Renderer>					 dx12_renderer;

	UPtr<ExampleDescriptorHeapAllocator> m_Example_Descriptor_Heap_Allocator;


	UPtr<CommandLineArguments> m_command_line_args;
	UPtr<ConsoleWindow>		   m_console_window;
	UPtr<ConsoleInputHandler>  m_console_input_handler;
	UPtr<ConfigManager>		   m_config_manager;
	UPtr<DxDemos>			   m_dx_demos;
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
	//bool bDx12_renderer_allocated;
	bool m_bDx_demos_allocated;
	bool m_bDebug_window_allocated;
	bool m_bExample_Descriptor_Heap_Allocator_allocated;
	bool m_bFont_manager_allocated;
	bool m_bFont_manager_window_allocated;
	bool m_bFrame_context_allocated;
	bool m_bWindow_class_allocated;
	bool m_bWindow_manager_allocated;
	bool m_bOutput_console_allocated;


	//imgui flags
public:
	bool m_bShow_demo_window;
	bool m_bShow_another_window;
	bool m_bShow_FontManager_window;
	bool m_bShow_styleEditor_window;
	bool m_bShow_Debug_window;
	bool m_bShow_FileSys_window;
};
