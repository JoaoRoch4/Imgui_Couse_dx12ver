// ConsoleInputHandler.hpp
// Header file for console input handler with threaded command processing
// Allows console to receive and process user commands asynchronously

#pragma once

#include "PCH.hpp"
#include "Master.hpp"

class ConsoleInputHandler : public Master {
public:
    ConsoleInputHandler();
    
    virtual ~ConsoleInputHandler();
    
    virtual void Open() override;
    
    virtual void Tick() override;
    
    virtual void Close() override;
    
    bool IsRunning() const { return m_bIsRunning; }
    
    void StopInputThread();

private:
    void InputThreadFunction();
    
    void ProcessCommand(const std::string& command);
    
    void PrintHelp();
    
    void ListCommands();
    
    void ClearConsole();
    
    void ExecuteCustomCommand(const std::string& command);

    std::thread m_inputThread;
    
    std::atomic<bool> m_bIsRunning;
    
    std::atomic<bool> m_bShouldStop;
    
    std::mutex m_commandMutex;
    
    std::queue<std::string> m_commandQueue;
    
    std::map<std::string, std::string> m_commandDescriptions;
};
