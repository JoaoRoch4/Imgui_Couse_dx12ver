#pragma once

#include "PCH.hpp"

namespace app {

/**
 * @brief RAII wrapper for ImWchar* strings with automatic memory management.
 * 
 * This class provides safe management of ImWchar strings allocated with ImGui::MemAlloc,
 * preventing memory leaks through automatic cleanup and providing convenient string operations.
 * 
 * Features:
 * - Automatic memory management (RAII)
 * - Move semantics support
 * - Conversion from various string types
 * - Safe copy operations
 * - Comparison operators
 * - Length tracking for performance
 */
class ImStr {
private:
    ImWchar* m_data;
    size_t   m_length; // Cached length (excluding null terminator)

    /**
     * @brief Internal helper to calculate string length.
     */
    static size_t CalcLength(const ImWchar* str) {
        if (!str) return 0;
        size_t len = 0;
        while (str[len]) len++;
        return len;
    }

    /**
     * @brief Internal helper to duplicate ImWchar string.
     */
    static ImWchar* Duplicate(const ImWchar* str, size_t len) {
        if (!str || len == 0) return nullptr;
        size_t allocSize = (len + 1) * sizeof(ImWchar);
        ImWchar* buf = (ImWchar*)ImGui::MemAlloc(allocSize);
        IM_ASSERT(buf);
        memcpy(buf, str, len * sizeof(ImWchar));
        buf[len] = 0; // Null terminator
        return buf;
    }

public:
    /**
     * @brief Default constructor - creates empty string.
     */
    ImStr() : m_data(nullptr), m_length(0) {}

    /**
     * @brief Construct from ImWchar* (takes ownership - does NOT copy).
     * @param data The ImWchar* to take ownership of (must be allocated with ImGui::MemAlloc).
     */
    explicit ImStr(ImWchar* data) : m_data(data), m_length(CalcLength(data)) {}

    /**
     * @brief Construct by copying from const ImWchar*.
     * @param str The ImWchar string to copy from.
     */
    ImStr(const ImWchar* str) {
        m_length = CalcLength(str);
        m_data = Duplicate(str, m_length);
    }

    /**
     * @brief Construct from UTF-8 string.
     * @param utf8_str The UTF-8 string to convert.
     */
    ImStr(const char* utf8_str) {
        if (!utf8_str || utf8_str[0] == '\0') {
            m_data = nullptr;
            m_length = 0;
            return;
        }

        // Allocate buffer for conversion (estimate size)
        size_t estimated_len = strlen(utf8_str) + 1;
        ImWchar* buf = (ImWchar*)ImGui::MemAlloc(estimated_len * sizeof(ImWchar));
        IM_ASSERT(buf);
        
        const char* str_end = nullptr;
        ImTextStrFromUtf8(buf, (int)estimated_len, utf8_str, nullptr, &str_end);
        
        m_data = buf;
        m_length = CalcLength(buf);
    }

    /**
     * @brief Construct from std::string (UTF-8).
     */
    ImStr(const std::string& str) : ImStr(str.c_str()) {}

    /**
     * @brief Construct from std::wstring.
     */
    ImStr(const std::wstring& wstr) {
        if (wstr.empty()) {
            m_data = nullptr;
            m_length = 0;
            return;
        }

        m_length = wstr.length();
        m_data = (ImWchar*)ImGui::MemAlloc((m_length + 1) * sizeof(ImWchar));
        IM_ASSERT(m_data);
        
        for (size_t i = 0; i < m_length; i++) {
            m_data[i] = (ImWchar)wstr[i];
        }
        m_data[m_length] = 0;
    }

    /**
     * @brief Copy constructor.
     */
    ImStr(const ImStr& other) {
        m_length = other.m_length;
        m_data = Duplicate(other.m_data, m_length);
    }

    /**
     * @brief Move constructor.
     */
    ImStr(ImStr&& other) noexcept 
        : m_data(other.m_data), m_length(other.m_length) {
        other.m_data = nullptr;
        other.m_length = 0;
    }

    /**
     * @brief Destructor - automatically frees memory.
     */
    ~ImStr() {
        if (m_data) {
            ImGui::MemFree(m_data);
            m_data = nullptr;
        }
    }

    /**
     * @brief Copy assignment operator.
     */
    ImStr& operator=(const ImStr& other) {
        if (this != &other) {
            if (m_data) ImGui::MemFree(m_data);
            m_length = other.m_length;
            m_data = Duplicate(other.m_data, m_length);
        }
        return *this;
    }

    /**
     * @brief Move assignment operator.
     */
    ImStr& operator=(ImStr&& other) noexcept {
        if (this != &other) {
            if (m_data) ImGui::MemFree(m_data);
            m_data = other.m_data;
            m_length = other.m_length;
            other.m_data = nullptr;
            other.m_length = 0;
        }
        return *this;
    }

    /**
     * @brief Assign from UTF-8 string.
     */
    ImStr& operator=(const char* utf8_str) {
        *this = ImStr(utf8_str);
        return *this;
    }

    /**
     * @brief Get raw pointer (const).
     */
    const ImWchar* c_str() const { 
        static const ImWchar empty[1] = {0};
        return m_data ? m_data : empty; 
    }

    /**
     * @brief Get raw pointer (non-const).
     */
    ImWchar* data() { return m_data; }

    /**
     * @brief Release ownership of the internal pointer.
     * @return The internal ImWchar* (caller is responsible for freeing).
     */
    ImWchar* release() {
        ImWchar* ptr = m_data;
        m_data = nullptr;
        m_length = 0;
        return ptr;
    }

    /**
     * @brief Check if string is empty.
     */
    bool empty() const { return m_length == 0 || m_data == nullptr; }

    /**
     * @brief Get string length (cached, O(1)).
     */
    size_t length() const { return m_length; }

    /**
     * @brief Get string length (cached, O(1)).
     */
    size_t size() const { return m_length; }

    /**
     * @brief Convert to UTF-8 std::string.
     */
    std::string toUtf8() const {
        if (!m_data || m_length == 0) return std::string();
        
        char buf[1024];
        ImTextStrToUtf8(buf, sizeof(buf), m_data, nullptr);
        return std::string(buf);
    }

    /**
     * @brief Convert to std::wstring.
     */
    std::wstring toWstring() const {
        if (!m_data || m_length == 0) return std::wstring();
        
        std::wstring result;
        result.reserve(m_length);
        for (size_t i = 0; i < m_length; i++) {
            result += (wchar_t)m_data[i];
        }
        return result;
    }

    /**
     * @brief Case-insensitive comparison.
     */
    int compareNoCase(const ImStr& other) const {
        if (!m_data && !other.m_data) return 0;
        if (!m_data) return -1;
        if (!other.m_data) return 1;
        
        const ImWchar* s1 = m_data;
        const ImWchar* s2 = other.m_data;
        
        while (*s1 && *s2) {
            ImWchar c1 = (*s1 >= 'A' && *s1 <= 'Z') ? *s1 + 32 : *s1;
            ImWchar c2 = (*s2 >= 'A' && *s2 <= 'Z') ? *s2 + 32 : *s2;
            if (c1 != c2) return c1 - c2;
            s1++;
            s2++;
        }
        return *s1 - *s2;
    }

    /**
     * @brief Comparison operators.
     */
    bool operator==(const ImStr& other) const {
        if (m_length != other.m_length) return false;
        if (!m_data && !other.m_data) return true;
        if (!m_data || !other.m_data) return false;
        
        for (size_t i = 0; i < m_length; i++) {
            if (m_data[i] != other.m_data[i]) return false;
        }
        return true;
    }

    bool operator!=(const ImStr& other) const {
        return !(*this == other);
    }

    /**
     * @brief Implicit conversion to const ImWchar*.
     */
    operator const ImWchar*() const { return c_str(); }

    /**
     * @brief Boolean conversion (true if non-empty).
     */
    explicit operator bool() const { return !empty(); }
};

} // namespace app
