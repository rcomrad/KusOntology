#include "core.hpp"

#include <iostream>

#include "file_data/file.hpp"
#include "file_data/path.hpp"
#include "file_data/variable_storage.hpp"

#include "web.hpp"

core::Core::Core() noexcept
{
}

void
core::Core::run() noexcept
{
    Web web;
    web.print();
}
