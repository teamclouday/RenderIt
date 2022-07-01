#pragma once

#include <iostream>
#include <string>

/** @file */

namespace RenderIt
{

struct Tools
{
    /// Defines display message types
    enum class MessageType
    {
        INFO = 0,
        WARN = 1,
        ERROR = 2
    };

    /// Prints to terminal
    template <typename... T> static void println(T &&...args)
    {
        (std::cout << ... << args) << '\n';
    }

    /// Display message in terminal with type
    static void display_message(const std::string &title, const std::string &message, MessageType type);

    /// Read full file content into string
    static std::string read_file_content(const std::string &path);

    /// Select file from explorer
    static std::string select_file_in_explorer();

    /// Ensure separators are valid
    static void ensure_path_separators(std::string &path);

    /// Enable/Disable OpenGL debug output
    static void set_gl_debug(bool enable, bool filterNotifications = true);
};

} // namespace RenderIt
