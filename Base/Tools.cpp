#include "Tools.hpp"

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

} // namespace RenderIt
