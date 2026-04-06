/*! \file
    \author Alexander Martynov (Marty AKA al-martyn1) <amart@mail.ru>
    \copyright (c) 2014-2026 Alexander Martynov
    \brief Опции различных контейнеров.

    Repository: https://github.com/al-martyn1/marty_containers
*/

#pragma once

#include <cstddef>
#include <cstdint>

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// marty::containers::
namespace marty {
namespace containers {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
enum class UpdateStrategy
{
    updateRestrict,         // Restrict/forbid elements update
    updateChangeOrder,      // Updated elements chanhes it's oreder (moved to end, as sample)
    updateInplace           // Updated elements keeps it's position


}; // enum class UpdateStrategy

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace contyainers
} // namespace marty


