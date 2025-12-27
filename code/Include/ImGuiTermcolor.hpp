#pragma once
#include "PCH.hpp"

namespace app {

// Forward declaration
class MemoryManagement;

// ImGui Termcolor - Provides termcolor-like API for ImGui console
// Uses MemoryManagement to store persistent color state

class ImGuiTermcolor {
public:
    // Color enum matching MemoryManagement::ImGuiConsoleColor
    using Color = MemoryManagement::ImGuiConsoleColor;

    // Convert color enum to ImGui color tag
    static const char* ColorToTag(Color color) {
        switch(color) {
            case Color::Reset:         return "[reset]";
            case Color::Grey:          return "[grey]";
            case Color::Red:           return "[red]";
            case Color::Green:         return "[green]";
            case Color::Yellow:        return "[yellow]";
            case Color::Blue:          return "[blue]";
            case Color::Magenta:       return "[magenta]";
            case Color::Cyan:          return "[cyan]";
            case Color::White:         return "[white]";
            case Color::BrightRed:     return "[bright_red]";
            case Color::BrightGreen:   return "[bright_green]";
            case Color::BrightYellow:  return "[bright_yellow]";
            case Color::BrightBlue:    return "[bright_blue]";
            case Color::BrightMagenta: return "[bright_magenta]";
            case Color::BrightCyan:    return "[bright_cyan]";
            case Color::BrightWhite:   return "[bright_white]";
            default:                   return "[reset]";
        }
    }

    // Convert color tag to ImVec4
    static ImVec4 TagToColor(const std::string& tag) {
        if(tag == "[reset]")
            return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);  // White (default)
        else if(tag == "[error]" || tag == "[red]")
            return ImVec4(1.0f, 0.3f, 0.3f, 1.0f);  // Bright red
        else if(tag == "[warning]" || tag == "[yellow]")
            return ImVec4(1.0f, 0.85f, 0.2f, 1.0f);  // Yellow
        else if(tag == "[success]" || tag == "[green]")
            return ImVec4(0.3f, 1.0f, 0.3f, 1.0f);  // Bright green
        else if(tag == "[info]" || tag == "[blue]" || tag == "[cyan]")
            return ImVec4(0.4f, 0.8f, 1.0f, 1.0f);  // Cyan/Blue
        else if(tag == "[cmd]")
            return ImVec4(0.6f, 1.0f, 0.6f, 1.0f);  // Light green
        else if(tag == "[history]" || tag == "[magenta]")
            return ImVec4(0.8f, 0.6f, 1.0f, 1.0f);  // Light purple
        else if(tag == "[grey]")
            return ImVec4(0.5f, 0.5f, 0.5f, 1.0f);  // Grey
        else if(tag == "[white]")
            return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);  // White
        else if(tag == "[bright_red]")
            return ImVec4(1.0f, 0.0f, 0.0f, 1.0f);  // Pure red
        else if(tag == "[bright_green]")
            return ImVec4(0.0f, 1.0f, 0.0f, 1.0f);  // Pure green
        else if(tag == "[bright_yellow]")
            return ImVec4(1.0f, 1.0f, 0.0f, 1.0f);  // Pure yellow
        else if(tag == "[bright_blue]")
            return ImVec4(0.0f, 0.5f, 1.0f, 1.0f);  // Bright blue
        else if(tag == "[bright_magenta]")
            return ImVec4(1.0f, 0.0f, 1.0f, 1.0f);  // Pure magenta
        else if(tag == "[bright_cyan]")
            return ImVec4(0.0f, 1.0f, 1.0f, 1.0f);  // Pure cyan
        else if(tag == "[bright_white]")
            return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);  // White
        else
            return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);  // Default white
    }
};

// ImGui termcolor namespace - provides termcolor-like manipulators for ImGui console
// Now uses MemoryManagement for persistent state
namespace itc {
    // Helper to get MemoryManagement instance
    inline MemoryManagement* GetMemory() {
        return MemoryManagement::Get_MemoryManagement_Singleton();
    }
    
    // Color manipulators that return color tags AND update MemoryManagement state
    inline const char* reset() {
        auto mem = GetMemory();
        if(mem) mem->SetConsoleColor(MemoryManagement::ImGuiConsoleColor::Reset);
        return "[reset]";
    }
    
    inline const char* grey() {
        auto mem = GetMemory();
        if(mem) mem->SetConsoleColor(MemoryManagement::ImGuiConsoleColor::Grey);
        return ImGuiTermcolor::ColorToTag(MemoryManagement::ImGuiConsoleColor::Grey);
    }
    
    inline const char* red() {
        auto mem = GetMemory();
        if(mem) mem->SetConsoleColor(MemoryManagement::ImGuiConsoleColor::Red);
        return ImGuiTermcolor::ColorToTag(MemoryManagement::ImGuiConsoleColor::Red);
    }
    
    inline const char* green() {
        auto mem = GetMemory();
        if(mem) mem->SetConsoleColor(MemoryManagement::ImGuiConsoleColor::Green);
        return ImGuiTermcolor::ColorToTag(MemoryManagement::ImGuiConsoleColor::Green);
    }
    
    inline const char* yellow() {
        auto mem = GetMemory();
        if(mem) mem->SetConsoleColor(MemoryManagement::ImGuiConsoleColor::Yellow);
        return ImGuiTermcolor::ColorToTag(MemoryManagement::ImGuiConsoleColor::Yellow);
    }
    
    inline const char* blue() {
        auto mem = GetMemory();
        if(mem) mem->SetConsoleColor(MemoryManagement::ImGuiConsoleColor::Blue);
        return ImGuiTermcolor::ColorToTag(MemoryManagement::ImGuiConsoleColor::Blue);
    }
    
    inline const char* magenta() {
        auto mem = GetMemory();
        if(mem) mem->SetConsoleColor(MemoryManagement::ImGuiConsoleColor::Magenta);
        return ImGuiTermcolor::ColorToTag(MemoryManagement::ImGuiConsoleColor::Magenta);
    }
    
    inline const char* cyan() {
        auto mem = GetMemory();
        if(mem) mem->SetConsoleColor(MemoryManagement::ImGuiConsoleColor::Cyan);
        return ImGuiTermcolor::ColorToTag(MemoryManagement::ImGuiConsoleColor::Cyan);
    }
    
    inline const char* white() {
        auto mem = GetMemory();
        if(mem) mem->SetConsoleColor(MemoryManagement::ImGuiConsoleColor::White);
        return ImGuiTermcolor::ColorToTag(MemoryManagement::ImGuiConsoleColor::White);
    }
    
    // Bright colors
    inline const char* bright_red() {
        auto mem = GetMemory();
        if(mem) mem->SetConsoleColor(MemoryManagement::ImGuiConsoleColor::BrightRed);
        return ImGuiTermcolor::ColorToTag(MemoryManagement::ImGuiConsoleColor::BrightRed);
    }
    
    inline const char* bright_green() {
        auto mem = GetMemory();
        if(mem) mem->SetConsoleColor(MemoryManagement::ImGuiConsoleColor::BrightGreen);
        return ImGuiTermcolor::ColorToTag(MemoryManagement::ImGuiConsoleColor::BrightGreen);
    }
    
    inline const char* bright_yellow() {
        auto mem = GetMemory();
        if(mem) mem->SetConsoleColor(MemoryManagement::ImGuiConsoleColor::BrightYellow);
        return ImGuiTermcolor::ColorToTag(MemoryManagement::ImGuiConsoleColor::BrightYellow);
    }
    
    inline const char* bright_blue() {
        auto mem = GetMemory();
        if(mem) mem->SetConsoleColor(MemoryManagement::ImGuiConsoleColor::BrightBlue);
        return ImGuiTermcolor::ColorToTag(MemoryManagement::ImGuiConsoleColor::BrightBlue);
    }
    
    inline const char* bright_magenta() {
        auto mem = GetMemory();
        if(mem) mem->SetConsoleColor(MemoryManagement::ImGuiConsoleColor::BrightMagenta);
        return ImGuiTermcolor::ColorToTag(MemoryManagement::ImGuiConsoleColor::BrightMagenta);
    }
    
    inline const char* bright_cyan() {
        auto mem = GetMemory();
        if(mem) mem->SetConsoleColor(MemoryManagement::ImGuiConsoleColor::BrightCyan);
        return ImGuiTermcolor::ColorToTag(MemoryManagement::ImGuiConsoleColor::BrightCyan);
    }
    
    inline const char* bright_white() {
        auto mem = GetMemory();
        if(mem) mem->SetConsoleColor(MemoryManagement::ImGuiConsoleColor::BrightWhite);
        return ImGuiTermcolor::ColorToTag(MemoryManagement::ImGuiConsoleColor::BrightWhite);
    }
    
    // Query current color state from MemoryManagement
    inline MemoryManagement::ImGuiConsoleColor current_color() {
        auto mem = GetMemory();
        return mem ? mem->GetConsoleColor() : MemoryManagement::ImGuiConsoleColor::Reset;
    }
    
    inline const char* current_tag() {
        auto mem = GetMemory();
        if(!mem) return "[reset]";
        return ImGuiTermcolor::ColorToTag(mem->GetConsoleColor());
    }
}

} // namespace app
