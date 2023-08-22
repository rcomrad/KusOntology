#include "core.hpp"

#include <iostream>

#include "domain/log.hpp"

#include "file_data/file.hpp"
#include "file_data/path.hpp"
#include "file_data/variable_storage.hpp"

#include "decoder.hpp"

core::Core::Core() noexcept
{
}

void
core::Core::run() noexcept
{
    file::Path::clearFolder("output");
    fill();
    print();
    check();
}

void
core::Core::fill() noexcept
{
    auto names = file::Path::getContent(file::Path::getPathUnsafe("data", ""));
    for (auto& i : names)
    {
        auto temp   = Decoder::processFile(i);
        auto name   = temp.getName();
        mWebs[name] = std::move(temp);
    }
}

void
core::Core::print() noexcept
{
    for (auto& i : mWebs)
    {
        i.second.print();
        i.second.makeMatrix();
    }
}

void
core::Core::check() noexcept
{
    auto baseName = file::VariableStorage::getInstance().getWordUnsafe("base");
    auto& base    = mWebs[baseName];
    for (auto& i : mWebs)
    {
        dom::writeInfo(baseName, "<->", i.first, "=", base.compare(i.second));
    }
}
