// MemoryManagement.cpp
// Implementation of centralized memory management
// Handles allocation and retrieval of all major application objects

#include "pch.hpp"
#include "Classes.hpp"
#include "MemoryManagement.hpp"


/**
 * @brief Constructor - Initializes all pointers and flags
 * 
 * Sets all unique_ptr members to nullptr and all allocation flags to false.
 * This ensures objects start in a clean, unallocated state.
 */
MemoryManagement::MemoryManagement()
: m_command_line_args(nullptr),
  m_console_window(nullptr),
  m_console_input_handler(nullptr),
  m_config_manager(nullptr),
  // dx12_renderer(nullptr),
  m_dx_demos(nullptr),
  m_debug_window(nullptr),
  m_Example_Descriptor_Heap_Allocator(nullptr),
  m_font_manager(nullptr),
  m_font_manager_window(nullptr),
  m_frame_context(nullptr),
  m_window_class(nullptr),
  m_window_manager(nullptr),
  m_output_console(nullptr),
  m_bCommand_line_args_allocated(false),
  m_bConsole_window_allocated(false),
  m_bConsole_input_handler_allocated(false),
  m_bConfig_manager_allocated(false),
  //bDx12_renderer_allocated(false),
  m_bDx_demos_allocated(false),
  m_bDebug_window_allocated(false),
  m_bExample_Descriptor_Heap_Allocator_allocated(false),
  m_bFont_manager_allocated(false),
  m_bFont_manager_window_allocated(false),
  m_bFrame_context_allocated(false),
  m_bWindow_class_allocated(false),
  m_bWindow_manager_allocated(false),
  m_bShow_demo_window(false),
  m_bShow_another_window(false),
  m_bShow_FontManager_window(false),
  m_bShow_styleEditor_window(false),
  m_bShow_Debug_window(false),
  m_bShow_FileSys_window(false),
  m_bOutput_console_allocated(false),
  m_io(nullptr),
  bIoPassed(false),
  m_clear_color_ptr(nullptr)

{}

/**
 * @brief Destructor - Resets all allocation flags
 * 
 * The unique_ptr members automatically clean up their owned objects.
 * We just need to reset the flags to indicate nothing is allocated.
 */
MemoryManagement::~MemoryManagement() {
	m_bCommand_line_args_allocated				   = false;
	m_bConsole_window_allocated					   = false;
	m_bConsole_input_handler_allocated			   = false;
	//bDx12_renderer_allocated					 = false;
	m_bDx_demos_allocated						   = false;
	m_bDebug_window_allocated					   = false;
	m_bExample_Descriptor_Heap_Allocator_allocated = false;
	m_bFont_manager_allocated					   = false;
	m_bFont_manager_window_allocated			   = false;
	m_bFrame_context_allocated					   = false;
	m_bWindow_class_allocated					   = false;
	m_bWindow_manager_allocated					   = false;
	m_bShow_demo_window							   = false;
	m_bShow_another_window						   = false;
	m_bShow_FontManager_window					   = false;
	m_bShow_styleEditor_window					   = false;
	m_bShow_Debug_window						   = false;
	m_bShow_FileSys_window						   = false;
	m_bOutput_console_allocated					   = false;
	bIoPassed									   = false;
	m_io										   = nullptr;
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

	Alloc_config_manager();

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

	// Allocate m_console_window
	Alloc_output_console();
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
	if (!m_bCommand_line_args_allocated) {
		// Create new instance using make_unique
		// make_unique is exception-safe and more efficient than new
		m_command_line_args = std::make_unique<CommandLineArguments>();

		// Mark as allocated
		m_bCommand_line_args_allocated = true;
	} else {
		// Already allocated, throw error
		throw std::runtime_error("CommandLineArguments is already allocated");
	}

	// Verify allocation succeeded
	// The pointer should not be null after make_unique
	if (!m_command_line_args) {
		throw std::runtime_error("CommandLineArguments failed to allocate");
	}
}


MemoryManagement* MemoryManagement::Get_MemoryManagement() {
	static std::unique_ptr<MemoryManagement> instance = std::make_unique<MemoryManagement>();
	return instance.get();
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
	if (!m_bCommand_line_args_allocated) {
		throw std::runtime_error("CommandLineArguments is not allocated");
	}

	// Check if pointer is valid
	if (!m_command_line_args) { throw std::runtime_error("m_command_line_args is nullptr!"); }

	// Return raw pointer from unique_ptr
	// get() returns the raw pointer without releasing ownership
	return m_command_line_args.get();
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
	if (!m_bConsole_input_handler_allocated) {
		// Create new instance
		m_console_input_handler = std::make_unique<ConsoleInputHandler>();

		// Mark as allocated
		m_bConsole_input_handler_allocated = true;
	} else {
		throw std::runtime_error("ConsoleInputHandler is already allocated");
	}

	// Verify allocation
	if (!m_console_input_handler) {
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

	if (!m_bConfig_manager_allocated) {
		// Create new instance
		m_config_manager = std::make_unique<ConfigManager>();

		// Mark as allocated
		m_bConfig_manager_allocated = true;
	} else {
		throw std::runtime_error("m_config_manager is already allocated");
	}

	// Verify allocation
	if (!m_config_manager) { throw std::runtime_error("m_config_manager failed to allocate"); }
}

/**
 * @brief Gets the ConsoleInputHandler instance
 * 
 * @return Raw pointer to ConsoleInputHandler
 * @throws std::runtime_error if not allocated or pointer is null
 */
ConsoleInputHandler* MemoryManagement::Get_ConsoleInputHandler() const {
	// Check if allocated
	if (!m_bConsole_input_handler_allocated) {
		throw std::runtime_error("ConsoleInputHandler is not allocated");
	}

	// Check if pointer is valid
	if (!m_console_input_handler) {
		throw std::runtime_error("m_console_input_handler is nullptr!");
	}

	// Return raw pointer
	return m_console_input_handler.get();
}

ConsoleWindow* MemoryManagement::Get_ConsoleWindow() const {

	// Check if allocated
	if (!m_bConsole_window_allocated) {
		throw std::runtime_error("m_console_window is not allocated");
	}

	// Check if pointer is valid
	if (!m_console_window) { throw std::runtime_error("m_console_window is nullptr!"); }

	// Return raw pointer
	return m_console_window.get();
}

DebugWindow* MemoryManagement::Get_DebugWindow() const {
	// Check if allocated
	if (!m_bDebug_window_allocated) { throw std::runtime_error("m_debug_window is not allocated"); }

	// Check if pointer is valid
	if (!m_debug_window) { throw std::runtime_error("m_debug_window is nullptr!"); }

	// Return raw pointer
	return m_debug_window.get();
}

FontManager* MemoryManagement::Get_FontManager() const {
	// Check if allocated
	if (!m_bFont_manager_allocated) { throw std::runtime_error("m_font_manager is not allocated"); }

	// Check if pointer is valid
	if (!m_font_manager) { throw std::runtime_error("m_font_manager is nullptr!"); }

	// Return raw pointer
	return m_font_manager.get();
}

FontManagerWindow* MemoryManagement::Get_FontManagerWindow() const {
	// Check if allocated
	if (!m_bFont_manager_window_allocated) {
		throw std::runtime_error("m_font_manager_window is not allocated");
	}

	// Check if pointer is valid
	if (!m_font_manager_window) { throw std::runtime_error("m_font_manager_window is nullptr!"); }

	// Return raw pointer
	return m_font_manager_window.get();
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
	if (!m_bExample_Descriptor_Heap_Allocator_allocated) {
		// Create new instance
		m_Example_Descriptor_Heap_Allocator = std::make_unique<ExampleDescriptorHeapAllocator>();

		// Mark as allocated
		m_bExample_Descriptor_Heap_Allocator_allocated = true;
	} else {
		throw std::runtime_error("ExampleDescriptorHeapAllocator is already allocated");
	}

	// Verify allocation
	if (!m_Example_Descriptor_Heap_Allocator) {
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
	if (!m_bExample_Descriptor_Heap_Allocator_allocated) {
		throw std::runtime_error("ExampleDescriptorHeapAllocator is not allocated");
	}

	// Check if pointer is valid
	if (!m_Example_Descriptor_Heap_Allocator) {
		throw std::runtime_error("m_Example_Descriptor_Heap_Allocator is nullptr!");
	}

	// Return raw pointer
	return m_Example_Descriptor_Heap_Allocator.get();
}

WindowClass* MemoryManagement::Get_WindowClass() const {
	// Check if allocated
	if (!m_bWindow_class_allocated) { throw std::runtime_error("m_window_class is not allocated"); }

	// Check if pointer is valid
	if (!m_window_class) { throw std::runtime_error("m_window_class is nullptr!"); }

	// Return raw pointer
	return m_window_class.get();
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
	if (!m_bWindow_manager_allocated) {
		// Create new instance
		m_window_manager = std::make_unique<WindowManager>();

		// Mark as allocated
		m_bWindow_manager_allocated = true;
	} else {
		throw std::runtime_error("WindowManager is already allocated");
	}

	// Verify allocation
	if (!m_window_manager) { throw std::runtime_error("m_window_manager failed to allocate"); }
}

/**
 * @brief Allocates OutputConsole object
 *
 * Creates the window console.
 *
 * @throws std::runtime_error if already allocated or allocation fails
 */
void MemoryManagement::Alloc_output_console() {

	// Check if already allocated
	if (!m_bOutput_console_allocated) {
		// Create new instance
		m_output_console = std::make_unique<OutputConsole>();

		// Mark as allocated
		m_bOutput_console_allocated = true;
	} else {
		throw std::runtime_error("OutputConsole is already allocated");
	}

	// Verify allocation
	if (!m_output_console) { throw std::runtime_error("m_output_console failed to allocate"); }
}

void MemoryManagement::Set_ImGuiIO(ImGuiIO* m_io) {
	const auto line = __LINE__;

	if (m_io == nullptr) {
		str err{"m_io passed at "};
		err.append(__FUNCTION__);
		err.append(" line: ");
		err.append(std::to_string(line));
		err.append("is nullptr! ");
		throw std::runtime_error(err.c_str());
	}
	m_io	  = m_io;
	bIoPassed = true;
}

ImGuiIO* MemoryManagement::Get_ImGuiIO() {
	const auto line = __LINE__;

	if (!bIoPassed) throw std::runtime_error("m_io was wet not passed to be geted");
	if (m_io == nullptr) {
		str err{"m_io passed at "};
		err.append(__FUNCTION__);
		err.append(" line: ");
		err.append(std::to_string(line));
		err.append("is nullptr! ");
		throw std::runtime_error(err.c_str());
	}
	return m_io;
}

ImVec4* MemoryManagement::Get_clear_color_ptr() {

	static std::unique_ptr<ImVec4> clear_color_singleton = std::make_unique<ImVec4>();
	return m_clear_color_ptr								 = clear_color_singleton.get();
}

/**
 * @brief Gets the WindowManager instance
 * 
 * @return Raw pointer to WindowManager
 * @throws std::runtime_error if not allocated or pointer is null
 */
WindowManager* MemoryManagement::Get_WindowManager() const {
	// Check if allocated
	if (!m_bWindow_manager_allocated) {
		throw std::runtime_error("m_window_manager is not allocated");
	}

	// Check if pointer is valid
	if (!m_window_manager) { throw std::runtime_error("m_window_manager is nullptr!"); }

	// Return raw pointer
	return m_window_manager.get();
}

ConfigManager* MemoryManagement::Get_ConfigManager() const {

	// Check if allocated
	if (!m_bConfig_manager_allocated) {
		throw std::runtime_error("m_config_manager is not allocated");
	}

	// Check if pointer is valid
	if (!m_config_manager) { throw std::runtime_error("m_config_manager is nullptr!"); }

	// Return raw pointer
	return m_config_manager.get();
}

OutputConsole* MemoryManagement::Get_OutputConsole() const {

	// Check if allocated
	if (!m_bOutput_console_allocated) {
		throw std::runtime_error("m_output_console is not allocated");
	}

	// Check if pointer is valid
	if (!m_output_console) { throw std::runtime_error("m_output_console is nullptr!"); }

	// Return raw pointer
	return m_output_console.get();
}


void MemoryManagement::Alloc_console_window() {

	// Check if already allocated
	if (!m_bConsole_window_allocated) {
		// Create new instance
		m_console_window = std::make_unique<ConsoleWindow>();

		// Mark as allocated
		m_bConsole_window_allocated = true;
	} else {
		throw std::runtime_error("m_console_window is already allocated");
	}

	// Verify allocation
	if (!m_console_window) { throw std::runtime_error("m_console_window failed to allocate"); }
}

void MemoryManagement::Alloc_dx_demos() {
	// TODO: Implement DX demos allocation
}

void MemoryManagement::Alloc_debug_window() {

	// Check if already allocated
	if (!m_bDebug_window_allocated) {
		// Create new instance
		m_debug_window = std::make_unique<DebugWindow>();

		// Mark as allocated
		m_bDebug_window_allocated = true;
	} else {
		throw std::runtime_error("m_debug_window is already allocated");
	}

	// Verify allocation
	if (!m_debug_window) { throw std::runtime_error("m_debug_window failed to allocate"); }
}

void MemoryManagement::Alloc_font_manager() {

	// Check if already allocated
	if (!m_bFont_manager_allocated) {
		// Create new instance
		m_font_manager = std::make_unique<FontManager>();

		// Mark as allocated
		m_bFont_manager_allocated = true;
	} else {
		throw std::runtime_error("m_font_manager is already allocated");
	}

	// Verify allocation
	if (!m_console_window) { throw std::runtime_error("m_console_window failed to allocate"); }
}

void MemoryManagement::Alloc_font_manager_window() {
	// Check if already allocated
	if (!m_bFont_manager_window_allocated) {
		// Create new instance
		m_font_manager_window = std::make_unique<FontManagerWindow>();

		// Mark as allocated
		m_bFont_manager_window_allocated = true;
	} else {
		throw std::runtime_error("m_font_manager_window is already allocated");
	}

	// Verify allocation
	if (!m_font_manager_window) {
		throw std::runtime_error("m_font_manager_window failed to allocate");
	}
}

void MemoryManagement::Alloc_frame_context() {
	// TODO: Implement frame context allocation
}

void MemoryManagement::Alloc_window_class() {
	// Check if already allocated
	if (!m_bWindow_class_allocated) {
		// Create new instance
		m_window_class = std::make_unique<WindowClass>();

		// Mark as allocated
		m_bWindow_class_allocated = true;
	} else {
		throw std::runtime_error("m_window_class is already allocated");
	}

	// Verify allocation
	if (!m_window_class) { throw std::runtime_error("m_window_class failed to allocate"); }
}
