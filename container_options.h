/*!
    \file
    \brief Опции различных контейнеров.
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


