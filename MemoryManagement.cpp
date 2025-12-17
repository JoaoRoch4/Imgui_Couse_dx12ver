#include "pch.hpp"
#include "Classes.hpp"

MemoryManagement::MemoryManagement()
: command_line_args(nullptr),
  console_window(nullptr),
  dx_demos(nullptr),
  debug_window(nullptr),
  Example_Descriptor_Heap_Allocator(nullptr),
  font_manager(nullptr),
  font_manager_window(nullptr),
  frame_context(nullptr),
  window_class(nullptr),
  window_manager(nullptr),
  bCommand_line_args_allocated(false),
  bConsole_window_allocated(false),
  bDx_demos_allocated(false),
  bDebug_window_allocated(false),
  bExample_Descriptor_Heap_Allocator_allocated(false),
  bFont_manager_allocated(false),
  bFont_manager_window_allocated(false),
  bFrame_context_allocated(false),
  bWindow_class_allocated(false),
  bWindow_manager_allocated(false) {}


MemoryManagement::~MemoryManagement() {

	bCommand_line_args_allocated				 = false;
	bConsole_window_allocated					 = false;
	bDx_demos_allocated							 = false;
	bDebug_window_allocated						 = false;
	bExample_Descriptor_Heap_Allocator_allocated = false;
	bFont_manager_allocated						 = false;
	bFont_manager_window_allocated				 = false;
	bFrame_context_allocated					 = false;
	bWindow_class_allocated						 = false;
	bWindow_manager_allocated					 = false;
}

void MemoryManagement::AllocAll() {

	Alloc_command_line_args();
	Alloc_console_window();
	Alloc_dx_demos();
	Alloc_debug_window();
	Alloc_Example_Descriptor_Heap_Allocator();
	Alloc_font_manager();
	Alloc_font_manager_window();
	Alloc_frame_context();
	Alloc_window_class();
	Alloc_window_manager();
}

void MemoryManagement::Alloc_command_line_args() {

	if (!bCommand_line_args_allocated) {
		command_line_args			 = std::make_unique<CommandLineArguments>();
		bCommand_line_args_allocated = true;
	} else throw std::runtime_error("CommandLineArguments is already allocated");

	if (!command_line_args) throw std::runtime_error("CommandLineArguments failed to allocated");
}

CommandLineArguments* MemoryManagement::Get_CommandLineArguments() const {
	if (!bCommand_line_args_allocated)
		throw std::runtime_error("CommandLineArguments is not allocated");
	if (!command_line_args) throw std::runtime_error("command_line_args is nullptr!");
	return command_line_args.get();
}

ExampleDescriptorHeapAllocator* MemoryManagement::Get_ExampleDescriptorHeapAllocator() const {
	if (!bExample_Descriptor_Heap_Allocator_allocated)
		throw std::runtime_error("ExampleDescriptorHeapAllocator is not allocated");
	if (!Example_Descriptor_Heap_Allocator)
		throw std::runtime_error("Example_Descriptor_Heap_Allocator is nullptr!");
	return Example_Descriptor_Heap_Allocator.get();
}

WindowManager* MemoryManagement::Get_WindowManager() const {

    if (!bWindow_manager_allocated)
		throw std::runtime_error("window_manager is not allocated");
	if (!window_manager)
		throw std::runtime_error("window_manager is nullptr!");
	return window_manager.get();
}

void MemoryManagement::Alloc_console_window() {}

void MemoryManagement::Alloc_dx_demos() {}

void MemoryManagement::Alloc_debug_window() {}

void MemoryManagement::Alloc_Example_Descriptor_Heap_Allocator() {

	if (!bExample_Descriptor_Heap_Allocator_allocated) {
		Example_Descriptor_Heap_Allocator = std::make_unique<ExampleDescriptorHeapAllocator>();
		bExample_Descriptor_Heap_Allocator_allocated = true;
	} else throw std::runtime_error("ExampleDescriptorHeapAllocator is already allocated");

	if (!Example_Descriptor_Heap_Allocator)
		throw std::runtime_error("ExampleDescriptorHeapAllocator failed to alloated");
}

void MemoryManagement::Alloc_font_manager() {}

void MemoryManagement::Alloc_font_manager_window() {}

void MemoryManagement::Alloc_frame_context() {}

void MemoryManagement::Alloc_window_class() {}

void MemoryManagement::Alloc_window_manager() {

    if (!bWindow_manager_allocated) {
		window_manager			 = std::make_unique<WindowManager>();
		bWindow_manager_allocated = true;
	} else throw std::runtime_error("WindowManager is already allocated");

	if (!window_manager) throw std::runtime_error("window_manager failed to allocated");
}
