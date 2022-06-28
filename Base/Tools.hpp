#pragma once
#include <termcolor/termcolor.hpp>

#include <iostream>
#include <string>

/** @file */

namespace RenderIt
{

/// Defines display message types
enum class MessageType
{
    INFO = 0,
    WARN = 1,
    ERROR = 2
};

/// Prints to terminal
template <typename... T> inline void println(T &&...args)
{
    (std::cout << ... << args) << std::endl;
}

/// Display message in terminal with type
void display_message(const std::string &title, const std::string &message, MessageType type);

/// Read full file content into string
std::string read_file_content(const std::string &path);

} // namespace RenderIt
