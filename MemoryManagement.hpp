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
//class DX12Renderer;
class DxDemos;
class DebugWindow;
class ExampleDescriptorHeapAllocator;
class FontManager;
class FontManagerWindow;
struct FrameContext;
class WindowClass;
class WindowManager;

class MemoryManagement {

public:
	MemoryManagement();

	virtual ~MemoryManagement();

	void AllocAll();

	CommandLineArguments*			Get_CommandLineArguments() const;
	ConsoleInputHandler*			Get_ConsoleInputHandler() const;
	//DX12Renderer*					Get_DX12Renderer() const;
	ExampleDescriptorHeapAllocator* Get_ExampleDescriptorHeapAllocator() const;
	WindowManager*					Get_WindowManager() const;

protected:
	void Alloc_command_line_args();
	void Alloc_console_window();
	void Alloc_console_input_handler();
	//void Alloc_dx12_renderer();
	void Alloc_dx_demos();
	void Alloc_debug_window();
	void Alloc_Example_Descriptor_Heap_Allocator();
	void Alloc_font_manager();
	void Alloc_font_manager_window();
	void Alloc_frame_context();
	void Alloc_window_class();
	void Alloc_window_manager();

private:
	UPtr<CommandLineArguments>			 command_line_args;
	UPtr<ConsoleWindow>					 console_window;
	UPtr<ConsoleInputHandler>			 console_input_handler;
	//UPtr<DX12Renderer>					 dx12_renderer;
	UPtr<DxDemos>						 dx_demos;
	UPtr<DebugWindow>					 debug_window;
	UPtr<ExampleDescriptorHeapAllocator> Example_Descriptor_Heap_Allocator;
	UPtr<FontManager>					 font_manager;
	UPtr<FontManagerWindow>				 font_manager_window;
	UPtr<FrameContext>					 frame_context;
	UPtr<WindowClass>					 window_class;
	UPtr<WindowManager>					 window_manager;

	bool bCommand_line_args_allocated;
	bool bConsole_window_allocated;
	bool bConsole_input_handler_allocated;
	//bool bDx12_renderer_allocated;
	bool bDx_demos_allocated;
	bool bDebug_window_allocated;
	bool bExample_Descriptor_Heap_Allocator_allocated;
	bool bFont_manager_allocated;
	bool bFont_manager_window_allocated;
	bool bFrame_context_allocated;
	bool bWindow_class_allocated;
	bool bWindow_manager_allocated;
};
