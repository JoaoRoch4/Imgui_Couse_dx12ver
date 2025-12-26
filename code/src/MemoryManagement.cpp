// MemoryManagement.cpp
// Implementation of centralized m_memory management
// Handles allocation and retrieval of all major application objects

#include "pch.hpp"
#include "Classes.hpp"
#include "MemoryManagement.hpp"

namespace app {

/**
 * @brief Opens and initializes the memory management system
 * 
 * Currently a placeholder for future initialization logic.
 */
void MemoryManagement::Open() {}

/**
 * @brief Updates the memory management system per frame
 * 
 * Currently a placeholder for future per-frame logic.
 */
void MemoryManagement::Tick() {}

/**
 * @brief Closes and cleans up the memory management system
 * 
 * Currently a placeholder for future cleanup logic.
 */
void MemoryManagement::Close() {}

/**
* @brief Constructor - Initializes all pointers and flags
* 
* Initializes all unique_ptr members to nullptr and allocation flags to false.
* This ensures the memory management system starts in a clean, unallocated state.
* 
* All managed objects must be explicitly allocated using their respective
* Alloc_* methods before they can be retrieved with Get_* methods.
*/
MemoryManagement::MemoryManagement()
: m_command_line_args(nullptr),
m_console_window(nullptr),
m_console_input_handler(nullptr),
m_config_manager(nullptr),
m_style_manager(nullptr),
m_dx12_renderer(nullptr),
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
  m_bStyle_manager_allocated(false),
  m_bDx12_renderer_allocated(false),
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
  m_bShow_Console_window(true),
  m_bOutput_console_allocated(false),
  m_io(nullptr),
  bIoPassed(false),
  m_clear_color_ptr(nullptr),
  m_currentConsoleColor(ImGuiConsoleColor::Reset)

{}

/**
* @brief Destructor - Resets all allocation flags
* 
* The unique_ptr members automatically clean up their owned objects through RAII.
* This destructor explicitly resets all flags to false for clarity and safety,
* although the automatic cleanup handles the actual memory deallocation.
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
* @brief Allocates all managed objects in the correct order
* 
* This method calls all individual Alloc_* methods to create and initialize
* all managed objects in a single call. This is the recommended way to
* initialize the entire application's memory management system.
* 
* Each allocation is independent and handles its own error checking.
* Already-allocated objects will be skipped with appropriate error codes.
*/
void MemoryManagement::AllocAll() {
	// Allocate command line arguments parser
	Alloc_command_line_args();

	// Allocate m_console m_window (ImGui m_console)
	Alloc_console_window();

	// Allocate m_console input handler (threaded command input)
	Alloc_console_input_handler();

	Alloc_config_manager();
	
	Alloc_style_manager();

	// Allocate DX12 renderer
	Alloc_dx12_renderer();

	// Allocate DirectX demos
	Alloc_dx_demos();

	// Allocate debug m_window
	Alloc_debug_window();

	// Allocate descriptor heap allocator for ImGui
	Alloc_Example_Descriptor_Heap_Allocator();

	// Allocate font manager
	Alloc_font_manager();

	// Allocate font manager m_window
	Alloc_font_manager_window();

	// Allocate frame context
	Alloc_frame_context();

	// Allocate m_window class (file system browser)
	Alloc_window_class();

	// Allocate m_window manager
	Alloc_window_manager();

	// Allocate m_console_window
	Alloc_output_console();
}

/**
* @brief Allocates CommandLineArguments object
* 
* Creates a new CommandLineArguments instance for parsing and storing
* command line arguments passed to the application.
*
* @return S_OK on successful allocation
* @return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED) if already allocated
* @return E_OUTOFMEMORY if memory allocation fails
* @return E_POINTER if pointer is null after allocation
*/
HRESULT MemoryManagement::Alloc_command_line_args() {
	// Check if already allocated to prevent m_memory leaks or re-initialization
	if (m_bCommand_line_args_allocated) {
		MessageBoxA(NULL, "CommandLineArguments is already allocated.", "Memory Error",
					MB_OK | MB_ICONERROR);
		return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED);
	}

	try {
		// Create new instance using make_unique (exception-safe)
		m_command_line_args = std::make_unique<CommandLineArguments>();
	} catch (const std::bad_alloc&) {
		// Handle m_memory exhaustion
		MessageBoxA(NULL, "Failed to allocate m_memory for CommandLineArguments.", "Critical Error",
					MB_OK | MB_ICONSTOP);
		return E_OUTOFMEMORY;
	}

	// Safety check to ensure the pointer is valid
	if (!m_command_line_args) {
		MessageBoxA(NULL, "Unexpected error during allocation.", "Allocation Error",
					MB_OK | MB_ICONERROR);
		return E_POINTER;
	}

	// Mark as successfully allocated
	m_bCommand_line_args_allocated = true;

	return S_OK;
}

/**
* @brief Gets the singleton instance of MemoryManagement
* 
* Uses the Meyer's Singleton pattern with a static local unique_ptr.
* The instance is created on first access and persists for the lifetime
* of the application.
* 
* @return Raw pointer to the singleton MemoryManagement instance
*/
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
* @brief Allocates the ConsoleInputHandler object
*
* Creates a new ConsoleInputHandler instance for threaded m_console input processing.
* This handler manages asynchronous input from the m_console in a separate thread.
*
* @return S_OK on successful allocation
* @return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED) if already allocated
* @return E_OUTOFMEMORY if memory allocation fails
* @return E_POINTER if pointer is null after allocation
*/
HRESULT MemoryManagement::Alloc_console_input_handler() {
    // 1. Check if already allocated to prevent logic errors and m_memory leaks
    if(m_bConsole_input_handler_allocated) {
        MessageBoxA(NULL, "ConsoleInputHandler is already allocated.", "Initialization Error", MB_OK | MB_ICONERROR);
        return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED);
    }

    try {
        // 2. Create new instance using make_unique (exception-safe)
        m_console_input_handler = std::make_unique<ConsoleInputHandler>();
    } catch(const std::bad_alloc&) {
        // 3. Handle physical m_memory exhaustion
        MessageBoxA(NULL, "Failed to allocate m_memory for ConsoleInputHandler.", "Critical Memory Error", MB_OK | MB_ICONSTOP);
        return E_OUTOFMEMORY;
    }

    // 4. Verify allocation success
    if(!m_console_input_handler) {
        return E_POINTER;
    }

    // 5. Mark as successfully allocated
    m_bConsole_input_handler_allocated = true;

    return S_OK;
}

/**
* @brief Allocates ConfigManager object
* 
* Creates a new ConfigManager instance for loading, saving, and managing
* application configuration settings.
*
* @return S_OK on successful allocation
* @return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED) if already allocated
* @return E_OUTOFMEMORY if memory allocation fails
* @return E_POINTER if pointer is null after allocation
*/
HRESULT MemoryManagement::Alloc_config_manager() {
	// Check if the ConfigManager is already allocated to prevent double allocation
	if (m_bConfig_manager_allocated) {
		MessageBoxA(NULL, "ConfigManager is already allocated.", "Initialization Error",
					MB_OK | MB_ICONERROR);
		return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED);
	}

	try {
		// Create new instance using make_unique
		// This handles m_memory allocation safely
		m_config_manager = std::make_unique<ConfigManager>();
	} catch (const std::bad_alloc&) {
		// Handle physical m_memory exhaustion
		MessageBoxA(NULL, "Failed to allocate m_memory for ConfigManager.", "Memory Error",
					MB_OK | MB_ICONSTOP);
		return E_OUTOFMEMORY;
	}

	// Verify if the unique_ptr holds a valid object
	if (!m_config_manager) {
		MessageBoxA(NULL, "ConfigManager pointer is null after allocation.", "Critical Error",
					MB_OK | MB_ICONERROR);
		return E_POINTER;
	}

	// Mark as successfully allocated
	m_bConfig_manager_allocated = true;

	return S_OK;
}

/**
* @brief Allocates the StyleManager object
* 
* Creates a new StyleManager instance for managing ImGui style configuration
* with persistent storage using reflectcpp serialization.
*
* @return S_OK on successful allocation
* @return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED) if already allocated
* @return E_OUTOFMEMORY if memory allocation fails
* @return E_POINTER if pointer is null after allocation
*/
HRESULT MemoryManagement::Alloc_style_manager() {
	// Check if the StyleManager is already allocated to prevent double allocation
	if (m_bStyle_manager_allocated) {
		MessageBoxA(NULL, "StyleManager is already allocated.", "Initialization Error",
					MB_OK | MB_ICONERROR);
		return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED);
	}

	try {
		// Create new instance using make_unique
		// This handles memory allocation safely
		m_style_manager = std::make_unique<StyleManager>();
	} catch (const std::bad_alloc&) {
		// Handle physical memory exhaustion
		MessageBoxA(NULL, "Failed to allocate memory for StyleManager.", "Memory Error",
					MB_OK | MB_ICONSTOP);
		return E_OUTOFMEMORY;
	}

	// Verify if the unique_ptr holds a valid object
	if (!m_style_manager) {
		MessageBoxA(NULL, "StyleManager pointer is null after allocation.", "Critical Error",
					MB_OK | MB_ICONERROR);
		return E_POINTER;
	}

	// Mark as successfully allocated
	m_bStyle_manager_allocated = true;

	return S_OK;
}

/**
* @brief Gets the ConsoleInputHandler instance
* 
* Returns a raw pointer to the managed ConsoleInputHandler object.
* The object must be allocated via Alloc_console_input_handler() before calling this.
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

/**
 * @brief Gets the ConsoleWindow instance
 * 
 * Returns a raw pointer to the managed ConsoleWindow object.
 * The object must be allocated via Alloc_console_window() before calling this.
 * 
 * @return Raw pointer to ConsoleWindow
 * @throws std::runtime_error if not allocated or pointer is null
 */
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

/**
 * @brief Gets the DebugWindow instance
 * 
 * Returns a raw pointer to the managed DebugWindow object.
 * The object must be allocated via Alloc_debug_window() before calling this.
 * 
 * @return Raw pointer to DebugWindow
 * @throws std::runtime_error if not allocated or pointer is null
 */
DebugWindow* MemoryManagement::Get_DebugWindow() const {
	// Check if allocated
	if (!m_bDebug_window_allocated) { throw std::runtime_error("m_debug_window is not allocated"); }

	// Check if pointer is valid
	if (!m_debug_window) { throw std::runtime_error("m_debug_window is nullptr!"); }

	// Return raw pointer
	return m_debug_window.get();
}

/**
 * @brief Gets the FontManager instance
 * 
 * Returns a raw pointer to the managed FontManager object.
 * The object must be allocated via Alloc_font_manager() before calling this.
 * 
 * @return Raw pointer to FontManager
 * @throws std::runtime_error if not allocated or pointer is null
 */
FontManager* MemoryManagement::Get_FontManager() const {
	// Check if allocated
	if (!m_bFont_manager_allocated) { throw std::runtime_error("m_font_manager is not allocated"); }

	// Check if pointer is valid
	if (!m_font_manager) { throw std::runtime_error("m_font_manager is nullptr!"); }

	// Return raw pointer
	return m_font_manager.get();
}

/**
 * @brief Gets the FontManagerWindow instance
 * 
 * Returns a raw pointer to the managed FontManagerWindow object.
 * The object must be allocated via Alloc_font_manager_window() before calling this.
 * 
 * @return Raw pointer to FontManagerWindow
 * @throws std::runtime_error if not allocated or pointer is null
 */
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
HRESULT MemoryManagement::Alloc_dx12_renderer() {

    // Check if the ConfigManager is already allocated to prevent double allocation
    if(m_bDx12_renderer_allocated) {
        MessageBoxA(NULL, "m_dx12_renderer is already allocated.", "Initialization Error",
            MB_OK | MB_ICONERROR);
        return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED);
    }

    try {
        // Create new instance using make_unique
        // This handles m_memory allocation safely
        m_dx12_renderer = std::make_unique<DX12Renderer>();
    } catch(const std::bad_alloc&) {
        // Handle physical m_memory exhaustion
        MessageBoxA(NULL, "Failed to allocate m_memory for m_dx12_renderer.", "Memory Error",
            MB_OK | MB_ICONSTOP);
        return E_OUTOFMEMORY;
    }

    // Verify if the unique_ptr holds a valid object
    if(!m_dx12_renderer) {
        MessageBoxA(NULL, "m_dx12_renderer pointer is null after allocation.", "Critical Error",
            MB_OK | MB_ICONERROR);
        return E_POINTER;
    }

    // Mark as successfully allocated
    m_bDx12_renderer_allocated = true;

    return S_OK;
}

/**
 * @brief Gets the DX12Renderer instance
 * 
 * @return Raw pointer to DX12Renderer
 * @throws std::runtime_error if not allocated or pointer is null
 */
DX12Renderer* MemoryManagement::Get_DX12Renderer() const {
	// Check if allocated
	if (!m_bDx12_renderer_allocated) {
		throw std::runtime_error("m_dx12_renderer is not allocated");
	}

	// Check if pointer is valid
	if (!m_dx12_renderer) {
		throw std::runtime_error("m_dx12_renderer is nullptr!");
	}

	// Return raw pointer
	return m_dx12_renderer.get();
}

/**
* @brief Allocates the ExampleDescriptorHeapAllocator object
*
* Creates the descriptor heap allocator used by ImGui for DirectX 12 rendering.
* This allocator manages GPU descriptor handles for ImGui textures and resources.
*
* @return S_OK on successful allocation
* @return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED) if already allocated
* @return E_OUTOFMEMORY if memory allocation fails
* @return E_POINTER if pointer is null after allocation
*/
HRESULT MemoryManagement::Alloc_Example_Descriptor_Heap_Allocator() {
    // Check if already allocated to prevent double allocation in DX12 context
    if(m_bExample_Descriptor_Heap_Allocator_allocated) {
        MessageBoxA(NULL, "ExampleDescriptorHeapAllocator is already allocated.", "Initialization Error", MB_OK | MB_ICONERROR);
        return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED);
    }

    try {
        // Create new instance using make_unique (exception-safe for bad_alloc)
        m_Example_Descriptor_Heap_Allocator = std::make_unique<ExampleDescriptorHeapAllocator>();
    } catch(const std::bad_alloc&) {
        // Handle m_memory exhaustion (critical for GPU-related allocators)
        MessageBoxA(NULL, "Failed to allocate m_memory for ExampleDescriptorHeapAllocator.", "Memory Error", MB_OK | MB_ICONSTOP);
        return E_OUTOFMEMORY;
    }

    // Safety check to ensure the pointer is valid after construction
    if(!m_Example_Descriptor_Heap_Allocator) {
        return E_POINTER;
    }

    // Mark as successfully allocated
    m_bExample_Descriptor_Heap_Allocator_allocated = true;

    return S_OK;
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

/**
 * @brief Gets the WindowClass instance
 * 
 * Returns a raw pointer to the managed WindowClass object.
 * The object must be allocated via Alloc_window_class() before calling this.
 * 
 * @return Raw pointer to WindowClass
 * @throws std::runtime_error if not allocated or pointer is null
 */
WindowClass* MemoryManagement::Get_WindowClass() const {
	// Check if allocated
	if (!m_bWindow_class_allocated) { throw std::runtime_error("m_window_class is not allocated"); }

	// Check if pointer is valid
	if (!m_window_class) { throw std::runtime_error("m_window_class is nullptr!"); }

	// Return raw pointer
	return m_window_class.get();
}

/**
* @brief Allocates the WindowManager object
*
* Creates the m_window manager responsible for Win32 m_window creation and management.
* Handles m_window lifecycle, DPI awareness, and m_window state (windowed/fullscreen).
*
* @return S_OK on successful allocation
* @return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED) if already allocated
* @return E_OUTOFMEMORY if memory allocation fails
* @return E_POINTER if pointer is null after allocation
*/
HRESULT MemoryManagement::Alloc_window_manager() {
    // Check if already allocated to prevent re-initialization
    if(m_bWindow_manager_allocated) {
        MessageBoxA(NULL, "WindowManager is already allocated.", "Initialization Error", MB_OK | MB_ICONERROR);
        return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED);
    }

    try {
        // Create new instance using make_unique (exception-safe)
        m_window_manager = std::make_unique<WindowManager>();
    } catch(const std::bad_alloc&) {
        // Handle m_memory exhaustion
        MessageBoxA(NULL, "Failed to allocate m_memory for WindowManager.", "Memory Error", MB_OK | MB_ICONSTOP);
        return E_OUTOFMEMORY;
    }

    // Verify if the unique_ptr holds a valid object
    if(!m_window_manager) {
        return E_POINTER;
    }

    // Mark as successfully allocated
    m_bWindow_manager_allocated = true;

    return S_OK;
}

/**
* @brief Allocates the OutputConsole object
*
* Creates the output m_console m_window for application logging and debug output.
* Provides a visual interface for viewing application messages and errors.
*
* @return S_OK on successful allocation
* @return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED) if already allocated
* @return E_OUTOFMEMORY if memory allocation fails
* @return E_POINTER if pointer is null after allocation
*/
HRESULT MemoryManagement::Alloc_output_console() {
    // Check if already allocated to prevent m_memory leaks or logic errors
    if(m_bOutput_console_allocated) {
        MessageBoxA(NULL, "OutputConsole is already allocated.", "Initialization Error", MB_OK | MB_ICONERROR);
        return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED);
    }

    try {
        // Create new instance using make_unique
        m_output_console = std::make_unique<OutputConsole>();
    } catch(const std::bad_alloc&) {
        // Handle physical m_memory exhaustion
        MessageBoxA(NULL, "Failed to allocate m_memory for OutputConsole.", "Memory Error", MB_OK | MB_ICONSTOP);
        return E_OUTOFMEMORY;
    }

    // Safety check to ensure the pointer is valid
    if(!m_output_console) {
        return E_POINTER;
    }

    // Mark as successfully allocated
    m_bOutput_console_allocated = true;

    return S_OK;


}

/**
 * @brief Sets the ImGuiIO pointer
 * 
 * Stores a pointer to the ImGui IO context for later retrieval.
 * This pointer is managed externally and must remain valid.
 * 
 * @param io Pointer to ImGuiIO instance
 * @throws std::runtime_error if io is nullptr
 */
void MemoryManagement::Set_ImGuiIO(ImGuiIO* io) {
	const auto line = __LINE__;

	if (io == nullptr) {
		str err{"ImGuiIO pointer passed at "};
		err.append(__FUNCTION__);
		err.append(" line: ");
		err.append(std::to_string(line));
		err.append(" is nullptr!");
		throw std::runtime_error(err.c_str());
	}
	m_io	  = io;
	bIoPassed = true;
}

/**
 * @brief Gets the ImGuiIO pointer
 * 
 * Returns the stored ImGui IO context pointer that was previously
 * set via Set_ImGuiIO().
 * 
 * @return Pointer to ImGuiIO instance
 * @throws std::runtime_error if not yet set or if pointer is null
 */
ImGuiIO* MemoryManagement::Get_ImGuiIO() {
	const auto line = __LINE__;

	if (!bIoPassed) throw std::runtime_error("ImGuiIO was not yet passed via Set_ImGuiIO()");
	if (m_io == nullptr) {
		str err{"ImGuiIO pointer at "};
		err.append(__FUNCTION__);
		err.append(" line: ");
		err.append(std::to_string(line));
		err.append(" is nullptr!");
		throw std::runtime_error(err.c_str());
	}
	return m_io;
}

/**
 * @brief Gets the clear color pointer
 * 
 * Returns a pointer to a singleton ImVec4 that stores the background clear color.
 * The singleton is created on first access and persists for the application lifetime.
 * 
 * @return Pointer to ImVec4 clear color singleton
 */
ImVec4* MemoryManagement::Get_clear_color_ptr() {

	static std::unique_ptr<ImVec4> clear_color_singleton = std::make_unique<ImVec4>();
	return m_clear_color_ptr							 = clear_color_singleton.get();
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

/**
 * @brief Gets the ConfigManager instance
 * 
 * Returns a raw pointer to the managed ConfigManager object.
 * The object must be allocated via Alloc_config_manager() before calling this.
 * 
 * @return Raw pointer to ConfigManager
 * @throws std::runtime_error if not allocated or pointer is null
 */
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

/**
 * @brief Gets the StyleManager instance
 * 
 * Returns a raw pointer to the managed StyleManager object.
 * The object must be allocated via Alloc_style_manager() before calling this.
 * 
 * @return Raw pointer to StyleManager
 * @throws std::runtime_error if not allocated or pointer is null
 */
StyleManager* MemoryManagement::Get_StyleManager() const {

	// Check if allocated
	if (!m_bStyle_manager_allocated) {
		throw std::runtime_error("m_style_manager is not allocated");
	}

	// Check if pointer is valid
	if (!m_style_manager) { throw std::runtime_error("m_style_manager is nullptr!"); }

	// Return raw pointer
	return m_style_manager.get();
}

/**
 * @brief Gets the OutputConsole instance
 * 
 * Returns a raw pointer to the managed OutputConsole object.
 * The object must be allocated via Alloc_output_console() before calling this.
 * 
 * @return Raw pointer to OutputConsole
 * @throws std::runtime_error if not allocated or pointer is null
 */
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


/**
* @brief Allocates the ConsoleWindow object
* 
* Creates the ImGui-based m_console m_window for command input and output display.
* 
* @return S_OK on successful allocation
* @return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED) if already allocated
* @return E_OUTOFMEMORY if memory allocation fails
* @return E_POINTER if pointer is null after allocation
*/
HRESULT MemoryManagement::Alloc_console_window() {
	if (m_bConsole_window_allocated) {
		MessageBoxA(NULL, "ConsoleWindow is already allocated.", "Initialization Error",
					MB_OK | MB_ICONERROR);
		return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED);
	}

	try {
		m_console_window = std::make_unique<ConsoleWindow>();
	} catch (const std::bad_alloc&) {
		MessageBoxA(NULL, "Failed to allocate m_memory for ConsoleWindow.", "Memory Error",
					MB_OK | MB_ICONSTOP);
		return E_OUTOFMEMORY;
	}

	if (!m_console_window) return E_POINTER;

	m_bConsole_window_allocated = true;
	return S_OK;
}

/**
* @brief Allocates the DirectX demos object
* 
* Currently a placeholder for future DirectX demonstration features.
* 
* @return S_OK (placeholder implementation)
*/
HRESULT MemoryManagement::Alloc_dx_demos() {
	// TODO: Implement DX demos allocation
	return S_OK;
}

/**
* @brief Allocates the DebugWindow object
* 
* Creates the debug m_window for displaying application debug information and metrics.
* 
* @return S_OK on successful allocation
* @return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED) if already allocated
* @return E_OUTOFMEMORY if memory allocation fails
* @return E_POINTER if pointer is null after allocation
*/
HRESULT MemoryManagement::Alloc_debug_window() {
	if (m_bDebug_window_allocated) {
		MessageBoxA(NULL, "DebugWindow is already allocated.", "Initialization Error",
					MB_OK | MB_ICONERROR);
		return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED);
	}

	try {
		m_debug_window = std::make_unique<DebugWindow>();
	} catch (const std::bad_alloc&) {
		MessageBoxA(NULL, "Failed to allocate m_memory for DebugWindow.", "Memory Error",
					MB_OK | MB_ICONSTOP);
		return E_OUTOFMEMORY;
	}

	if (!m_debug_window) return E_POINTER;

	m_bDebug_window_allocated = true;
	return S_OK;
}

/**
* @brief Allocates the FontManager object
* 
* Creates the font manager for loading, managing, and switching between different fonts.
* 
* @return S_OK on successful allocation
* @return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED) if already allocated
* @return E_OUTOFMEMORY if memory allocation fails
* @return E_POINTER if pointer is null after allocation
*/
HRESULT MemoryManagement::Alloc_font_manager() {
	if (m_bFont_manager_allocated) {
		MessageBoxA(NULL, "FontManager is already allocated.", "Initialization Error",
					MB_OK | MB_ICONERROR);
		return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED);
	}

	try {
		m_font_manager = std::make_unique<FontManager>();
	} catch (const std::bad_alloc&) {
		MessageBoxA(NULL, "Failed to allocate m_memory for FontManager.", "Memory Error",
					MB_OK | MB_ICONSTOP);
		return E_OUTOFMEMORY;
	}

	if (!m_font_manager) return E_POINTER;

	m_bFont_manager_allocated = true;
	return S_OK;
}

/**
* @brief Allocates the FontManagerWindow object
* 
* Creates the UI m_window for the font manager, allowing users to select and preview fonts.
* 
* @return S_OK on successful allocation
* @return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED) if already allocated
* @return E_OUTOFMEMORY if memory allocation fails
* @return E_POINTER if pointer is null after allocation
*/
HRESULT MemoryManagement::Alloc_font_manager_window() {
	if (m_bFont_manager_window_allocated) {
		MessageBoxA(NULL, "FontManagerWindow is already allocated.", "Initialization Error",
					MB_OK | MB_ICONERROR);
		return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED);
	}

	try {
		m_font_manager_window = std::make_unique<FontManagerWindow>();
	} catch (const std::bad_alloc&) {
		MessageBoxA(NULL, "Failed to allocate m_memory for FontManagerWindow.", "Memory Error",
					MB_OK | MB_ICONSTOP);
		return E_OUTOFMEMORY;
	}

	if (!m_font_manager_window) return E_POINTER;

	m_bFont_manager_window_allocated = true;
	return S_OK;
}

/**
* @brief Allocates the frame context object
* 
* Currently a placeholder for future frame context management.
* 
* @return S_OK (placeholder implementation)
*/
HRESULT MemoryManagement::Alloc_frame_context() {
	// TODO: Implement frame context allocation
	return S_OK;
}

/**
* @brief Allocates the WindowClass object
* 
* Creates the m_window class wrapper, typically used for file system browser or other utility windows.
* 
* @return S_OK on successful allocation
* @return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED) if already allocated
* @return E_OUTOFMEMORY if memory allocation fails
* @return E_POINTER if pointer is null after allocation
*/
HRESULT MemoryManagement::Alloc_window_class() {
	if (m_bWindow_class_allocated) {
		MessageBoxA(NULL, "WindowClass is already allocated.", "Initialization Error",
					MB_OK | MB_ICONERROR);
		return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED);
	}

	try {
		m_window_class = std::make_unique<WindowClass>();
	} catch (const std::bad_alloc&) {
		MessageBoxA(NULL, "Failed to allocate m_memory for WindowClass.", "Memory Error",
					MB_OK | MB_ICONSTOP);
		return E_OUTOFMEMORY;
	}

	if (!m_window_class) return E_POINTER;

	m_bWindow_class_allocated = true;
	return S_OK;
}
} // namespace app
