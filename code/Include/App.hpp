#pragma once

#include "PCH.hpp"

// Forward declarations
namespace app {
    class MemoryManagement;
    class OutputConsole;
    class CommandLineArguments;
    class ConsoleWindow;
    class WindowManager;
    class FontManager;
    class FontManagerWindow;
    class DebugWindow;
    class ConfigManager;
    class WindowClass;
    class ExampleDescriptorHeapAllocator;
    class DX12Renderer;
    }

namespace app {

/**
 * @brief Main application class that manages the entire application lifecycle
 * 
 * This class encapsulates all application state and provides methods for:
 * - Initialization of all subsystems
 * - Main rendering loop
 * - Resource cleanup
 * - Window message handling
 */
class App {
public:
    /**
     * @brief Construct the application
     */
    App();
    
    /**
     * @brief Destroy the application and cleanup resources
     */
    ~App();

    // Prevent copying
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    /**
     * @brief Initialize and run the application
     * @param hInstance Application instance handle from WinMain
     * @return EXIT_SUCCESS on successful execution, error code otherwise
     */
    int Run(_In_ HINSTANCE hInstance);

    /**
     * @brief Window procedure for handling Windows messages
     * @param hWnd Window handle
     * @param msg Message identifier
     * @param wParam Additional message information
     * @param lParam Additional message information
     * @return Result of message processing
     */
    static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    /**
     * @brief Get the singleton application instance
     * @return Pointer to the application instance
     */
    static App* GetInstance() { return s_instance; }

    /**
     * @brief Get the DirectX 12 renderer instance
     * @return Pointer to the DX12Renderer
     */
    DX12Renderer* GetRenderer() const { return m_renderer; }

private:
    /**
     * @brief Initialize all application subsystems
     * @param hInstance Application instance handle
     */
    void Initialize(_In_ HINSTANCE hInstance);

    /**
     * @brief Create and display the application window
     * @param hInstance Application instance handle
     */
    void OpenWindow(_In_ HINSTANCE hInstance);

    /**
     * @brief Setup ImGui context and styling
     */
    void SetupImGui();

    /**
     * @brief Initialize DirectX 12 renderer backend for ImGui
     */
    void SetupImGuiBackend();

    /**
     * @brief Main rendering loop
     * 
     * Loads fonts, initializes configuration, and runs the main message loop.
     * Handles window messages, renders ImGui windows, and manages frame presentation.
     */
    void MainLoop();

    /**
     * @brief Render a single frame
     * @param clear_color Background clear color
     * @return true if should continue, false if application should exit
     */
    bool RenderFrame(ImVec4& clear_color, bool& colorModified);

    /**
     * @brief Render ImGui UI elements
     * @param clear_color Background clear color (input/output)
     * @param colorModified Set to true if color was modified this frame
     */
    void RenderUI(ImVec4& clear_color, bool& colorModified);

    /**
     * @brief Cleanup all application resources
     */
    void Cleanup();

    HRESULT Alloc();

private:
    // Singleton instance
    static App* s_instance;

    // Application objects
    MemoryManagement* m_memory;
    OutputConsole* m_console;
    CommandLineArguments* m_cmdArgs;
    ConsoleWindow* m_consoleWindow;
    WindowManager* m_window;
    FontManager* m_font_manager;
    FontManagerWindow* m_font_manager_window;
    DebugWindow* m_debug_window;
    ConfigManager* m_configManager;
    WindowClass* m_window_obj;

    // DirectX 12 Renderer and related objects
    DX12Renderer* m_renderer;
    ExampleDescriptorHeapAllocator* m_HeapAlloc;

    // ImGui context
    ImGuiIO* m_io;
    ImGuiStyle* m_style;
};

} // namespace app
