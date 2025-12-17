// ConsoleInputHandler.cpp
// Implementation of console input handler with threaded command processing
// Provides asynchronous command input and processing for console applications

#include "PCH.hpp"
#include "ConsoleInputHandler.hpp"

/**
 * @brief Default constructor - Initializes all member variables
 * 
 * Sets up the initial state:
 * - No input thread running
 * - Flags set to false (not running, not stopping)
 * - Empty command queue
 * - Initializes command descriptions map
 */
ConsoleInputHandler::ConsoleInputHandler()
    : m_inputThread(),
      m_bIsRunning(false),
      m_bShouldStop(false),
      m_commandMutex(),
      m_commandQueue(),
      m_commandDescriptions() {
    
    // Initialize the command descriptions map
    // This map stores command names as keys and their descriptions as values
    // Used for the help system to display available commands
    m_commandDescriptions["help"] = "Display this help message";
    m_commandDescriptions["list"] = "List all available commands";
    m_commandDescriptions["clear"] = "Clear the console screen";
    m_commandDescriptions["exit"] = "Exit the application";
    m_commandDescriptions["quit"] = "Exit the application (alias for exit)";
    m_commandDescriptions["status"] = "Show current application status";
    m_commandDescriptions["echo"] = "Echo the text you type (usage: echo <text>)";
}

/**
 * @brief Destructor - Ensures proper cleanup of resources
 * 
 * This destructor:
 * 1. Signals the input thread to stop via m_bShouldStop flag
 * 2. Waits for the thread to finish if it's joinable
 * 3. Cleans up the command queue
 * 
 * Thread safety: Uses atomic flag to safely signal thread termination
 */
ConsoleInputHandler::~ConsoleInputHandler() {
    // Signal the input thread to stop
    // This is an atomic operation, so it's thread-safe
    m_bShouldStop = true;
    
    // Check if the thread is joinable (i.e., it's running and can be joined)
    // A thread is joinable if it has been started and not yet joined or detached
    if (m_inputThread.joinable()) {
        // Wait for the thread to finish its execution
        // This blocks until the thread terminates
        // Important: Always join or detach threads before destruction
        m_inputThread.join();
    }
    
    // Set running flag to false
    // This indicates that the handler is no longer active
    m_bIsRunning = false;
}

/**
 * @brief Opens the console input handler and starts the input thread
 * 
 * This method:
 * 1. Sets the running flag to true
 * 2. Resets the stop flag
 * 3. Creates and starts a new thread for input handling
 * 4. Prints a welcome message to the console
 * 
 * The input thread runs InputThreadFunction() which continuously
 * reads user input from the console.
 * 
 * Thread safety: Uses atomic flags for thread synchronization
 */
void ConsoleInputHandler::Open() {
    // Set the running flag to true
    // This atomic operation is thread-safe
    // Indicates that the handler is now active
    m_bIsRunning = true;
    
    // Reset the stop flag to false
    // Ensures the thread will run (not be told to stop immediately)
    m_bShouldStop = false;
    
    // Create and start a new thread
    // std::thread constructor takes a function pointer and optional arguments
    // &ConsoleInputHandler::InputThreadFunction: pointer to member function
    // this: pointer to the current object (needed for member function call)
    // The thread will start executing InputThreadFunction immediately
    m_inputThread = std::thread(&ConsoleInputHandler::InputThreadFunction, this);
    
    // Print welcome message to console
    // std::cout is thread-safe for individual operations
    // std::endl flushes the output buffer and adds a newline
    std::cout << "\n=== Console Input Handler Started ===" << std::endl;
    std::cout << "Type 'help' for available commands" << std::endl;
    std::cout << "Type 'exit' or 'quit' to close the application\n" << std::endl;
}

/**
 * @brief Tick method - Processes queued commands during the main loop
 * 
 * This method is called repeatedly from the main application loop.
 * It processes all commands that have been queued by the input thread.
 * 
 * Process flow:
 * 1. Lock the command mutex to ensure thread-safe access to the queue
 * 2. Process all commands currently in the queue
 * 3. Remove each command from the queue after processing
 * 4. Unlock the mutex automatically when scope ends
 * 
 * Thread safety: Uses std::lock_guard for automatic mutex management
 * Performance: Processes all queued commands in one Tick() call
 */
void ConsoleInputHandler::Tick() {
    // Create a lock guard to automatically lock and unlock the mutex
    // std::lock_guard locks the mutex in its constructor
    // It automatically unlocks the mutex when it goes out of scope (RAII)
    // This ensures the mutex is always unlocked, even if an exception occurs
    std::lock_guard<std::mutex> lock(m_commandMutex);
    
    // Process all commands currently in the queue
    // We continue looping while the queue is not empty
    while (!m_commandQueue.empty()) {
        // Get the first command from the front of the queue
        // std::queue is FIFO (First In, First Out)
        // front() returns a reference to the first element
        std::string command = m_commandQueue.front();
        
        // Remove the first command from the queue
        // pop() removes the front element
        // We must call pop() after getting the value with front()
        m_commandQueue.pop();
        
        // Process the command
        // This calls ProcessCommand which executes the command logic
        ProcessCommand(command);
    }
    
    // The lock_guard automatically unlocks the mutex here
    // when it goes out of scope at the end of the function
}

/**
 * @brief Closes the console input handler and stops the input thread
 * 
 * This method:
 * 1. Signals the input thread to stop
 * 2. Waits for the thread to finish
 * 3. Sets the running flag to false
 * 4. Cleans up resources
 * 
 * This method is safe to call multiple times.
 * 
 * Thread safety: Uses atomic flags and proper thread joining
 */
void ConsoleInputHandler::Close() {
    // Call the helper method to stop the input thread
    // This encapsulates the stopping logic
    StopInputThread();
    
    // Print farewell message
    std::cout << "\n=== Console Input Handler Stopped ===" << std::endl;
}

/**
 * @brief Stops the input thread safely
 * 
 * This method:
 * 1. Sets the stop flag to signal the thread
 * 2. Checks if the thread is joinable
 * 3. Waits for the thread to finish
 * 4. Sets the running flag to false
 * 
 * This is a helper method that can be called from multiple places.
 * 
 * Thread safety: Uses atomic operations and proper synchronization
 */
void ConsoleInputHandler::StopInputThread() {
    // Signal the thread to stop by setting the atomic flag
    // The input thread checks this flag in its loop
    m_bShouldStop = true;
    
    // Check if the thread is joinable (running and not yet joined)
    if (m_inputThread.joinable()) {
        // Wait for the thread to finish
        // join() blocks until the thread terminates
        // This ensures proper cleanup before continuing
        m_inputThread.join();
    }
    
    // Mark the handler as not running
    // This atomic operation is thread-safe
    m_bIsRunning = false;
}

/**
 * @brief Input thread function - Runs in a separate thread
 * 
 * This function continuously reads input from the console.
 * It runs in its own thread, separate from the main application thread.
 * 
 * Process flow:
 * 1. Display a prompt (">")
 * 2. Wait for user to type a line and press Enter
 * 3. Add the command to the queue (thread-safe)
 * 4. Check if we should stop
 * 5. Repeat
 * 
 * Thread lifetime:
 * - Starts when the thread is created in Open()
 * - Runs until m_bShouldStop is set to true
 * - Exits cleanly, allowing the thread to be joined
 * 
 * Thread safety: Uses mutex to protect the command queue
 */
void ConsoleInputHandler::InputThreadFunction() {
    // Print initial message indicating thread has started
    std::cout << "Console input thread started. Waiting for commands..." << std::endl;
    
    // Main input loop - continues until m_bShouldStop becomes true
    // m_bShouldStop is an atomic bool, so this check is thread-safe
    while (!m_bShouldStop) {
        // Display the command prompt
        // std::cout << ">" means "print > without a newline"
        // std::flush ensures the prompt is displayed immediately (flushes buffer)
        std::cout << "> " << std::flush;
        
        // String to store the user's input
        std::string input;
        
        // Read a line of input from the console
        // std::getline reads from std::cin until a newline character
        // It stores the result in 'input' (excluding the newline)
        // This call blocks (waits) until the user presses Enter
        std::getline(std::cin, input);
        
        // Check if input stream is in a good state
        // std::cin.good() returns false if:
        // - End of file was reached (Ctrl+D on Unix, Ctrl+Z on Windows)
        // - An error occurred
        // - The stream is in a bad state
        if (!std::cin.good()) {
            // Stream is not good, break out of the loop
            // This handles cases like EOF or stream errors
            break;
        }
        
        // Trim whitespace from both ends of the input string
        // This removes leading and trailing spaces, tabs, etc.
        
        // Find the first non-whitespace character from the beginning
        // std::string::find_first_not_of returns the index of the first character
        // that is NOT in the specified set (spaces and tabs)
        // Returns std::string::npos if only whitespace is found
        size_t start = input.find_first_not_of(" \t");
        
        // Find the last non-whitespace character from the end
        // std::string::find_last_not_of searches from the end backwards
        size_t end = input.find_last_not_of(" \t");
        
        // Check if the string contains any non-whitespace characters
        // If start == std::string::npos, the string is all whitespace
        if (start == std::string::npos) {
            // Empty or whitespace-only input, skip to next iteration
            continue;
        }
        
        // Extract the trimmed substring
        // substr(start, length) extracts a substring
        // end - start + 1 calculates the length of the non-whitespace portion
        input = input.substr(start, end - start + 1);
        
        // Convert the command to lowercase for case-insensitive comparison
        // This allows users to type "HELP", "help", or "Help" and get the same result
        std::string lowerInput = input;
        
        // std::transform applies a function to each character
        // Parameters:
        // 1. lowerInput.begin(): start of the range to transform
        // 2. lowerInput.end(): end of the range
        // 3. lowerInput.begin(): where to store the result (same string)
        // 4. ::tolower: function to apply to each character (convert to lowercase)
        std::transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        
        // Check for exit commands
        // If user types "exit" or "quit", signal the application to stop
        if (lowerInput == "exit" || lowerInput == "quit") {
            // Print exit message
            std::cout << "Exit command received. Shutting down..." << std::endl;
            
            // Set the stop flag to true
            // This will cause this loop to exit on the next iteration
            m_bShouldStop = true;
            
            // Also set running flag to false
            m_bIsRunning = false;
            
            // Break out of the loop immediately
            break;
        }
        
        // Add the command to the queue for processing in the main thread
        // We need to lock the mutex because multiple threads access the queue
        
        // Create a lock guard to automatically manage the mutex
        // The mutex protects the command queue from concurrent access
        // Lock is acquired here
        {
            std::lock_guard<std::mutex> lock(m_commandMutex);
            
            // Add the command to the back of the queue
            // push() adds an element to the end of the queue
            m_commandQueue.push(input);
            
            // The lock is automatically released here when lock goes out of scope
        }
        
        // Loop continues, displaying the prompt again
    }
    
    // Print message when thread is exiting
    std::cout << "Console input thread exiting..." << std::endl;
}

/**
 * @brief Processes a command entered by the user
 * 
 * This method takes a command string and executes the appropriate action.
 * 
 * Process flow:
 * 1. Convert command to lowercase for case-insensitive matching
 * 2. Check against known commands
 * 3. Execute the corresponding action
 * 4. If unknown, report error
 * 
 * Supported commands:
 * - help: Display help message
 * - list: List all commands
 * - clear: Clear the console
 * - status: Show application status
 * - echo <text>: Echo back the text
 * - Any other: Report unknown command
 * 
 * @param command The command string to process
 */
void ConsoleInputHandler::ProcessCommand(const std::string& command) {
    // Convert command to lowercase for case-insensitive comparison
    // This allows "HELP" and "help" to work the same way
    std::string lowerCommand = command;
    
    // Transform each character to lowercase
    std::transform(lowerCommand.begin(), lowerCommand.end(), lowerCommand.begin(),
                  [](unsigned char c) { return std::tolower(c); });
    
    // Check for "help" command
    if (lowerCommand == "help") {
        // Display the help message
        PrintHelp();
    }
    // Check for "list" command
    else if (lowerCommand == "list") {
        // List all available commands
        ListCommands();
    }
    // Check for "clear" command
    else if (lowerCommand == "clear" || lowerCommand == "cls") {
        // Clear the console screen
        ClearConsole();
    }
    // Check for "status" command
    else if (lowerCommand == "status") {
        // Display current application status
        std::cout << "\n=== Application Status ===" << std::endl;
        std::cout << "Console Input Handler: " << (m_bIsRunning ? "Running" : "Stopped") << std::endl;
        std::cout << "Commands in Queue: " << m_commandQueue.size() << std::endl;
        std::cout << "=========================\n" << std::endl;
    }
    // Check for "echo" command (echoes back the text)
    else if (lowerCommand.substr(0, 5) == "echo ") {
        // Extract the text after "echo "
        // substr(5) gets everything from index 5 to the end
        std::string echoText = command.substr(5);
        
        // Echo the text back to the console
        std::cout << "Echo: " << echoText << std::endl;
    }
    // Check for empty command
    else if (lowerCommand.empty()) {
        // Empty command, do nothing
        // Just return without printing anything
        return;
    }
    // Unknown command
    else {
        // Print error message for unknown command
        std::cout << "Unknown command: '" << command << "'" << std::endl;
        std::cout << "Type 'help' for a list of available commands." << std::endl;
        
        // Optionally, you can add custom command handling here
        // ExecuteCustomCommand(command);
    }
}

/**
 * @brief Prints the help message showing all available commands
 * 
 * This method displays a formatted list of all commands with descriptions.
 * It reads from the m_commandDescriptions map which stores command info.
 * 
 * Output format:
 * === Available Commands ===
 * command1 - description1
 * command2 - description2
 * ...
 */
void ConsoleInputHandler::PrintHelp() {
    // Print header
    std::cout << "\n=== Available Commands ===" << std::endl;
    
    // Iterate through all commands in the descriptions map
    // The map stores <command_name, description> pairs
    // const auto& means: automatically deduce type, use constant reference
    // This avoids copying and allows us to read but not modify
    for (const auto& cmd : m_commandDescriptions) {
        // cmd.first is the command name (key)
        // cmd.second is the description (value)
        // Format: "  command - description"
        std::cout << "  " << cmd.first << " - " << cmd.second << std::endl;
    }
    
    // Print footer
    std::cout << "==========================\n" << std::endl;
}

/**
 * @brief Lists all available commands (without descriptions)
 * 
 * This method provides a compact list of command names only.
 * Useful when you just want to see what commands exist.
 * 
 * Output format:
 * Available commands: command1, command2, command3, ...
 */
void ConsoleInputHandler::ListCommands() {
    // Print header
    std::cout << "\nAvailable commands: ";
    
    // Track whether this is the first command (for comma formatting)
    bool first = true;
    
    // Iterate through all commands
    for (const auto& cmd : m_commandDescriptions) {
        // If not the first command, print a comma separator
        if (!first) {
            std::cout << ", ";
        }
        
        // Print the command name
        std::cout << cmd.first;
        
        // After first command, set first to false
        first = false;
    }
    
    // Print newline at the end
    std::cout << "\n" << std::endl;
}

/**
 * @brief Clears the console screen
 * 
 * This method clears the console output by calling the system clear command.
 * 
 * Platform-specific behavior:
 * - Windows: Uses "cls" command
 * - Unix/Linux/Mac: Uses "clear" command
 * 
 * Note: This uses system() which creates a new process to run the command.
 * For production code, you might want to use platform-specific APIs instead.
 */
void ConsoleInputHandler::ClearConsole() {
    // Check the operating system using preprocessor directives
    // _WIN32 is defined on Windows (both 32-bit and 64-bit)
#ifdef _WIN32
    // Windows: Use "cls" command
    // system() executes a shell command
    // "cls" is the Windows command to clear the console
    system("cls");
#else
    // Unix/Linux/Mac: Use "clear" command
    // "clear" is the Unix command to clear the console
    system("clear");
#endif
    
    // Alternative method without system() - print many newlines
    // This is more portable but less clean:
    // for (int i = 0; i < 50; i++) {
    //     std::cout << "\n";
    // }
}

/**
 * @brief Executes a custom command (placeholder for user extensions)
 * 
 * This method can be customized to handle application-specific commands.
 * It's called when ProcessCommand doesn't recognize a command.
 * 
 * You can extend this to:
 * - Execute game-specific commands
 * - Call application functions
 * - Trigger debug actions
 * - Load/save data
 * - Control application behavior
 * 
 * @param command The custom command to execute
 */
void ConsoleInputHandler::ExecuteCustomCommand(const std::string& command) {
    // This is a placeholder method for custom command handling
    // You can implement your own logic here
    
    // Example: Parse command and arguments
    // std::stringstream ss(command);
    // std::string cmd;
    // ss >> cmd;
    
    // Example: Check for custom commands
    // if (cmd == "spawn") {
    //     std::string objectName;
    //     ss >> objectName;
    //     // Spawn object logic here
    // }
    
    // For now, just print a message
    std::cout << "Custom command execution not implemented: " << command << std::endl;
}
