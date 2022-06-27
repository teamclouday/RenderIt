#include "Tools.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

namespace RenderIt
{

void display_message(const std::string &title, const std::string &message, MessageType type)
{
    switch (type)
    {
    case MessageType::INFO:
        std::cout << "I[" << termcolor::green << title << termcolor::reset << "]: " << message << std::endl;
        break;
    case MessageType::WARN:
        std::cout << "W[" << termcolor::yellow << title << termcolor::reset << "]: " << message << std::endl;
        break;
    case MessageType::ERROR:
        std::cout << "E[" << termcolor::red << termcolor::blink << title << termcolor::reset << "]: " << message
                  << std::endl;
        break;
    }
}

std::string read_file_content(const std::string &path)
{
    if (!fs::is_regular_file(path))
        return "";
    std::ifstream f(path);
    std::stringstream sstr;
    sstr << f.rdbuf();
    return sstr.str();
}

} // namespace RenderIt
