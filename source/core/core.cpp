#include "core.hpp"

#include <iostream>

#include "domain/log.hpp"

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
    auto names = file::Path::getContent(file::Path::getPathUnsafe("data", ""));
    for (auto& i : names)
    {
        process(i);
    }
}

void
core::Core::process(const std::string& aName) noexcept
{
    Web w(aName);
    w.print();
    w.makeMatrix();
}
