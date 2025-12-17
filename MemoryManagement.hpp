#pragma once

#include "pch.hpp"
#include "Classes.hpp"

class WindowManager;

class MemoryManagement {

public:
	MemoryManagement();

	virtual ~MemoryManagement();

	void AllocAll();

	CommandLineArguments*			Get_CommandLineArguments() const;
	ExampleDescriptorHeapAllocator* Get_ExampleDescriptorHeapAllocator() const;
	WindowManager*					Get_WindowManager() const;

protected:
	void Alloc_command_line_args();
	void Alloc_console_window();
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
	bool bDx_demos_allocated;
	bool bDebug_window_allocated;
	bool bExample_Descriptor_Heap_Allocator_allocated;
	bool bFont_manager_allocated;
	bool bFont_manager_window_allocated;
	bool bFrame_context_allocated;
	bool bWindow_class_allocated;
	bool bWindow_manager_allocated;
};
