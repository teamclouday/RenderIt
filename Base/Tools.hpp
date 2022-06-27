#pragma once
#include <termcolor/termcolor.hpp>

#include <iostream>
#include <string>

namespace RenderIt
{

enum class MessageType
{
    INFO = 0,
    WARN = 1,
    ERROR = 2
};

template <typename... T> inline void println(T &&...args)
{
    (std::cout << ... << args) << std::endl;
}

void display_message(const std::string &title, const std::string &message, MessageType type);

} // namespace RenderIt
