/*!
    \file
    \brief Используемые в библиотеке исключения
 */

#pragma once

#include <exception>
#include <stdexcept>
#include <string>

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// marty::containers::
namespace marty {
namespace containers {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
class update_error : public std::runtime_error
{

public: //ctors

    explicit update_error(const std::string& message) 
    : std::runtime_error(message)
    {}

    explicit update_error(const char* message)
        : std::runtime_error(message)
    {}

    update_error() = delete;
    update_error(const update_error &) = default;
    update_error(update_error &&) = default;
    update_error& operator=(const update_error &) = default;
    update_error& operator=(update_error &&) = default;

};
//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace contyainers
} // namespace marty


