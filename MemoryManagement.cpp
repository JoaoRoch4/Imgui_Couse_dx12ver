// MemoryManagement.cpp
// Implementation of centralized memory management
// Handles allocation and retrieval of all major application objects

#include "pch.hpp"
#include "Classes.hpp"

/**
 * @brief Constructor - Initializes all pointers and flags
 * 
 * Sets all unique_ptr members to nullptr and all allocation flags to false.
 * This ensures objects start in a clean, unallocated state.
 */
MemoryManagement::MemoryManagement()
: command_line_args(nullptr),
  console_window(nullptr),
  console_input_handler(nullptr),
    config_manager(nullptr),
  // dx12_renderer(nullptr),
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
  bConsole_input_handler_allocated(false),
    bConfig_manager_allocated(false),
  //bDx12_renderer_allocated(false),
  bDx_demos_allocated(false),
  bDebug_window_allocated(false),
  bExample_Descriptor_Heap_Allocator_allocated(false),
  bFont_manager_allocated(false),
  bFont_manager_window_allocated(false),
  bFrame_context_allocated(false),
  bWindow_class_allocated(false),
  bWindow_manager_allocated(false),
  bShow_demo_window(false),
  bShow_another_window(false),
  bShow_FontManager_window(false),
  bShow_styleEditor_window(false),
  bShow_Debug_window(false),
  bShow_FileSys_window(false)

{}

/**
 * @brief Destructor - Resets all allocation flags
 * 
 * The unique_ptr members automatically clean up their owned objects.
 * We just need to reset the flags to indicate nothing is allocated.
 */
MemoryManagement::~MemoryManagement() {
	bCommand_line_args_allocated				 = false;
	bConsole_window_allocated					 = false;
	bConsole_input_handler_allocated			 = false;
	//bDx12_renderer_allocated					 = false;
	bDx_demos_allocated							 = false;
	bDebug_window_allocated						 = false;
	bExample_Descriptor_Heap_Allocator_allocated = false;
	bFont_manager_allocated						 = false;
	bFont_manager_window_allocated				 = false;
	bFrame_context_allocated					 = false;
	bWindow_class_allocated						 = false;
	bWindow_manager_allocated					 = false;
	bShow_demo_window							 = false;
	bShow_another_window						 = false;
	bShow_FontManager_window					 = false;
	bShow_styleEditor_window					 = false;
	bShow_Debug_window							 = false;
	bShow_FileSys_window						 = false;
}

/**
 * @brief Allocates all managed objects
 * 
 * This method calls all individual allocation methods to create
 * all managed objects at once.
 * 
 * Useful for initializing the entire application in one call.
 */
void MemoryManagement::AllocAll() {
	// Allocate command line arguments parser
	Alloc_command_line_args();

	// Allocate console window (ImGui console)
	Alloc_console_window();

	// Allocate console input handler (threaded command input)
	Alloc_console_input_handler();

	// Allocate DX12 renderer
	//Alloc_dx12_renderer();

	// Allocate DirectX demos
	Alloc_dx_demos();

	// Allocate debug window
	Alloc_debug_window();

	// Allocate descriptor heap allocator for ImGui
	Alloc_Example_Descriptor_Heap_Allocator();

	// Allocate font manager
	Alloc_font_manager();

	// Allocate font manager window
	Alloc_font_manager_window();

	// Allocate frame context
	Alloc_frame_context();

	// Allocate window class (file system browser)
	Alloc_window_class();

	// Allocate window manager
	Alloc_window_manager();
}

/**
 * @brief Allocates CommandLineArguments object
 * 
 * Creates a new CommandLineArguments instance using std::make_unique.
 * Throws exception if already allocated or allocation fails.
 * 
 * @throws std::runtime_error if already allocated or allocation fails
 */
void MemoryManagement::Alloc_command_line_args() {
	// Check if already allocated
	// Prevents double allocation which would cause a memory leak
	if (!bCommand_line_args_allocated) {
		// Create new instance using make_unique
		// make_unique is exception-safe and more efficient than new
		command_line_args = std::make_unique<CommandLineArguments>();

		// Mark as allocated
		bCommand_line_args_allocated = true;
	} else {
		// Already allocated, throw error
		throw std::runtime_error("CommandLineArguments is already allocated");
	}

	// Verify allocation succeeded
	// The pointer should not be null after make_unique
	if (!command_line_args) { throw std::runtime_error("CommandLineArguments failed to allocate"); }
}

/**
 * @brief Gets the CommandLineArguments instance
 * 
 * Returns a raw pointer to the managed CommandLineArguments object.
 * 
 * @return Raw pointer to CommandLineArguments
 * @throws std::runtime_error if not allocated or pointer is null
 */
CommandLineArguments* MemoryManagement::Get_CommandLineArguments() const {
	// Check if allocated
	if (!bCommand_line_args_allocated) {
		throw std::runtime_error("CommandLineArguments is not allocated");
	}

	// Check if pointer is valid
	if (!command_line_args) { throw std::runtime_error("command_line_args is nullptr!"); }

	// Return raw pointer from unique_ptr
	// get() returns the raw pointer without releasing ownership
	return command_line_args.get();
}

/**
 * @brief Allocates ConsoleInputHandler object
 * 
 * Creates a new ConsoleInputHandler instance for threaded console input.
 * 
 * @throws std::runtime_error if already allocated or allocation fails
 */
void MemoryManagement::Alloc_console_input_handler() {
	// Check if already allocated
	if (!bConsole_input_handler_allocated) {
		// Create new instance
		console_input_handler = std::make_unique<ConsoleInputHandler>();

		// Mark as allocated
		bConsole_input_handler_allocated = true;
	} else {
		throw std::runtime_error("ConsoleInputHandler is already allocated");
	}

	// Verify allocation
	if (!console_input_handler) {
		throw std::runtime_error("ConsoleInputHandler failed to allocate");
	}
}

/**
 * @brief Allocates ConsoleInputHandler object
 *
 * 
 *
 * @throws std::runtime_error if already allocated or allocation fails
 */

void MemoryManagement::Alloc_config_manager() {

    if(!bConfig_manager_allocated) {
        // Create new instance
        config_manager = std::make_unique<ConfigManager>();

        // Mark as allocated
        bConfig_manager_allocated = true;
    } else {
        throw std::runtime_error("config_manager is already allocated");
    }

    // Verify allocation
    if(!config_manager) {
        throw std::runtime_error("config_manager failed to allocate");
    }
}

/**
 * @brief Gets the ConsoleInputHandler instance
 * 
 * @return Raw pointer to ConsoleInputHandler
 * @throws std::runtime_error if not allocated or pointer is null
 */
ConsoleInputHandler* MemoryManagement::Get_ConsoleInputHandler() const {
	// Check if allocated
	if (!bConsole_input_handler_allocated) {
		throw std::runtime_error("ConsoleInputHandler is not allocated");
	}

	// Check if pointer is valid
	if (!console_input_handler) { throw std::runtime_error("console_input_handler is nullptr!"); }

	// Return raw pointer
	return console_input_handler.get();
}

ConsoleWindow* MemoryManagement::Get_ConsoleWindow() const {

	// Check if allocated
	if (!bConsole_window_allocated) { throw std::runtime_error("console_window is not allocated"); }

	// Check if pointer is valid
	if (!console_window) { throw std::runtime_error("console_window is nullptr!"); }

	// Return raw pointer
	return console_window.get();
}

/**
 * @brief Allocates DX12Renderer object
 * 
 * Creates a new DX12Renderer instance for DirectX 12 rendering.
 * 
 * @throws std::runtime_error if already allocated or allocation fails
 */
//void MemoryManagement::Alloc_dx12_renderer() {
//	// Check if already allocated
//	if (!bDx12_renderer_allocated) {
//		// Create new instance
//		dx12_renderer = std::make_unique<DX12Renderer>();
//
//		// Mark as allocated
//		bDx12_renderer_allocated = true;
//	} else {
//		throw std::runtime_error("DX12Renderer is already allocated");
//	}
//
//	// Verify allocation
//	if (!dx12_renderer) {
//		throw std::runtime_error("DX12Renderer failed to allocate");
//	}
//}

/**
 * @brief Gets the DX12Renderer instance
 * 
 * @return Raw pointer to DX12Renderer
 * @throws std::runtime_error if not allocated or pointer is null
 */
//DX12Renderer* MemoryManagement::Get_DX12Renderer() const {
//	// Check if allocated
//	if (!bDx12_renderer_allocated) {
//		throw std::runtime_error("DX12Renderer is not allocated");
//	}
//
//	// Check if pointer is valid
//	if (!dx12_renderer) {
//		throw std::runtime_error("dx12_renderer is nullptr!");
//	}
//
//	// Return raw pointer
//	return dx12_renderer.get();
//}

/**
 * @brief Allocates ExampleDescriptorHeapAllocator object
 * 
 * Creates the descriptor heap allocator used by ImGui for DirectX 12.
 * 
 * @throws std::runtime_error if already allocated or allocation fails
 */
void MemoryManagement::Alloc_Example_Descriptor_Heap_Allocator() {
	// Check if already allocated
	if (!bExample_Descriptor_Heap_Allocator_allocated) {
		// Create new instance
		Example_Descriptor_Heap_Allocator = std::make_unique<ExampleDescriptorHeapAllocator>();

		// Mark as allocated
		bExample_Descriptor_Heap_Allocator_allocated = true;
	} else {
		throw std::runtime_error("ExampleDescriptorHeapAllocator is already allocated");
	}

	// Verify allocation
	if (!Example_Descriptor_Heap_Allocator) {
		throw std::runtime_error("ExampleDescriptorHeapAllocator failed to allocate");
	}
}

/**
 * @brief Gets the ExampleDescriptorHeapAllocator instance
 * 
 * @return Raw pointer to ExampleDescriptorHeapAllocator
 * @throws std::runtime_error if not allocated or pointer is null
 */
ExampleDescriptorHeapAllocator* MemoryManagement::Get_ExampleDescriptorHeapAllocator() const {
	// Check if allocated
	if (!bExample_Descriptor_Heap_Allocator_allocated) {
		throw std::runtime_error("ExampleDescriptorHeapAllocator is not allocated");
	}

	// Check if pointer is valid
	if (!Example_Descriptor_Heap_Allocator) {
		throw std::runtime_error("Example_Descriptor_Heap_Allocator is nullptr!");
	}

	// Return raw pointer
	return Example_Descriptor_Heap_Allocator.get();
}

/**
 * @brief Allocates WindowManager object
 * 
 * Creates the window manager for Win32 window handling.
 * 
 * @throws std::runtime_error if already allocated or allocation fails
 */
void MemoryManagement::Alloc_window_manager() {
	// Check if already allocated
	if (!bWindow_manager_allocated) {
		// Create new instance
		window_manager = std::make_unique<WindowManager>();

		// Mark as allocated
		bWindow_manager_allocated = true;
	} else {
		throw std::runtime_error("WindowManager is already allocated");
	}

	// Verify allocation
	if (!window_manager) { throw std::runtime_error("window_manager failed to allocate"); }
}

/**
 * @brief Gets the WindowManager instance
 * 
 * @return Raw pointer to WindowManager
 * @throws std::runtime_error if not allocated or pointer is null
 */
WindowManager* MemoryManagement::Get_WindowManager() const {
	// Check if allocated
	if (!bWindow_manager_allocated) { throw std::runtime_error("window_manager is not allocated"); }

	// Check if pointer is valid
	if (!window_manager) { throw std::runtime_error("window_manager is nullptr!"); }

	// Return raw pointer
	return window_manager.get();
}

ConfigManager* MemoryManagement::Get_ConfigManager() const {

    // Check if allocated
    if(!bConfig_manager_allocated) { throw std::runtime_error("config_manager is not allocated"); }

    // Check if pointer is valid
    if(!config_manager) { throw std::runtime_error("config_manager is nullptr!"); }

    // Return raw pointer
    return config_manager.get();
}

// Placeholder implementations for other allocation methods
// These can be implemented as needed

void MemoryManagement::Alloc_console_window() {

	// Check if already allocated
	if (!bConsole_window_allocated) {
		// Create new instance
		console_window = std::make_unique<ConsoleWindow>();

		// Mark as allocated
		bConsole_window_allocated = true;
	} else {
		throw std::runtime_error("console_window is already allocated");
	}

	// Verify allocation
	if (!console_window) { throw std::runtime_error("console_window failed to allocate"); }
}

void MemoryManagement::Alloc_dx_demos() {
	// TODO: Implement DX demos allocation
}

void MemoryManagement::Alloc_debug_window() {
	// TODO: Implement debug window allocation
}

void MemoryManagement::Alloc_font_manager() {
	// TODO: Implement font manager allocation
}

void MemoryManagement::Alloc_font_manager_window() {
	// TODO: Implement font manager window allocation
}

void MemoryManagement::Alloc_frame_context() {
	// TODO: Implement frame context allocation
}

void MemoryManagement::Alloc_window_class() {
	// TODO: Implement window class allocation
}
